/*--------------------------------------------------
    Consid Solution - Find Duplicate Regnr
    Author: Jonathan Bosson, jonathan.bosson@gmail.com
    Date: 2016-10-11
    Compiler flags: -fopenmp -std=c++11 -O3
	Running instructions:	> set OMP_NUM_THREADS=8
							> out.exe Rgn02.txt
---------------------------------------------------*/
#include <stdlib.h>         // malloc, free
#include <stdio.h>          // printf, read file
#include <libiomp/omp.h>			// parallel computation

#define cMAX 18279 // 26^3 + 26^2 + 26 + 1
#define nMAX 1000

/* A = 1, B = 2, C = 3 ... Y = 25, Z = 26 
Using index as above, I convert the s[3] to a base26-index using ASCII hack, max index = cMAX - 1.
Could be base25 (since 'I' doesn't exist), however this forces a loop through s to check side of 'I'.  
Might be worth the saved memory in matrix if 'U' and 'O' didn't exist either to allow for base23
CAB = 3*26^2 + 1*26 + 2 = 2056 */
int charIndex(const char* s) {
    return ((s[0] - '@')*676 + (s[1] - '@')*26 + (s[2] - '@'));
}

// Efficiently returns index from char[3] s
int intIndex(const char* s) {
    return ((s[0] - '0')*100 + (s[1] - '0')*10 + (s[2] - '0'));
}

int main(int argc, char* argv[]) {
	double t0, t1;
	FILE* pFile;
	long lSize;
	char* buffer;
    int n, c;
    auto matrix = new bool[cMAX][nMAX](); // 2D bool matrix initialised as false
    bool abort = false;
	t0 = omp_get_wtime(); // Start timer

    // Open file with sent argument
    pFile = fopen(argv[1], "rb");
    if (pFile == NULL) {
        fputs("Error opening file\n", stderr); 
        exit(1);
    }

    // Get number of characters in file
    fseek(pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    // Allocate memory to contain the whole file
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL) {
        fputs("Failed to allocate memory\n", stderr); 
        exit (2);
    }
	
    // Copy the file into the buffer
    if (fread(buffer, 1, lSize, pFile) != lSize) {
        fputs("Error reading file\n", stderr); 
        exit (3);
    }

    // Check if duplicates exist
    #pragma omp parallel
    {
        // Flip value at [c][n] where c is base26-index and n normal base10 if regnr exists
        #pragma omp for schedule (static) private (n, c)
        for(int i = 0; i < lSize; i+=8) {
            if (!abort) {
                c = charIndex(&buffer[i]);
                n = intIndex(&buffer[i] + 3);
                if (!(matrix[c][n] = !matrix[c][n])) { // duplicate found, abort parallel loop
                    abort = true;
                    #pragma omp flush (abort)
                }
            } 
        }
    }
	t1 = omp_get_wtime(); // Stop timer
    if (abort)
        printf("Dubbletter\n");
    else
        printf("Ej dubblett\n");

    // Print execution time
    printf("Execution time was %g seconds with %d threads\n", (t1-t0), omp_get_max_threads());

    fclose(pFile);
    free(buffer);
    delete[] matrix;
	return 0;
}