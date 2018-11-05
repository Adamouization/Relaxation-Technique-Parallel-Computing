/*
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * Relaxation technique for solving differential equations in a shared memory architecture using pthreads
 * Author: aj645
 * Date: 19-Nov-2018
 */

#include <stdio.h>
#include <stdlib.h>
#define dim 4


// Function definitions
double double_random(double low, double high);
void print_array(double **array);


/*
 * Program entry.
 */
int main() {
	// allocate space for a 1D array of double pointers.
	double **square_array = malloc(dim * sizeof(double*));
	if (square_array == NULL) {
		printf("square_array array is NULL\n");
	}

	// allocate space for multiple 1D arrays of doubles
	for (int i = 0; i < dim; i++){
		square_array[i] = malloc(dim * sizeof(double));
		if (square_array[i] == NULL) {
			printf("square_array array %d is NULL\n", i);
		}
	}

	// populate arrays with random doubles
	for (int i = 0; i < dim; i++){
		for (int j = 0; j < dim; j++) {
			square_array[i][j] = double_random(1.0, 10.0);
		}
	}

	// print array the array
	print_array(square_array);

 	free(square_array);
   	return 0;
}


/*
 * Returns a random double ranging from 'low' to 'high'.
 */
double double_random (double low, double high) {
    return ((double)rand() * (high-low)) / (double)RAND_MAX + low;
}


/*
 * Prints a square array to the command line.
 */
void print_array(double **array) {
	for (int i = 0; i < dim; i++) {
 		for (int j = 0; j < dim; j++) {
 			printf("%f ", array[i][j]);
 		}
 		printf("\n");
 	}
}
