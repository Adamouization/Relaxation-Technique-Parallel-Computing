/*
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * 
 * Relaxation technique for solving differential equations in a shared memory 
 * architecture using pthreads
 * 
 * Author: aj645
 * Date: 19-Nov-2018
 */

#include <stdio.h>
#include <stdlib.h>
#define dim 4


// Function definitions
double ** create_square_array(void);
void check_malloc(double **);
double double_random(double low, double high);
void print_array(double **array);


/*
 * Program entry.
 */
int main() {
	double **square_array = create_square_array();
	print_array(square_array);

 	free(square_array);
   	return 0;
}


/*
 * Creates an square array by creating an initial array of pointers, each 
 * looking at 1D arrays of doubles.
 */
double ** create_square_array(void) {
	// allocate space for a 1D array of double pointers.
	double **sq_array = malloc(dim * sizeof(double*));
	check_malloc(sq_array);

	// allocate space for multiple 1D arrays of doubles
	for (int i = 0; i < dim; i++){
		sq_array[i] = malloc(dim * sizeof(double));
		check_malloc(sq_array);
	}

	// populate the array with random doubles
	for (int i = 0; i < dim; i++){
		for (int j = 0; j < dim; j++) {
			sq_array[i][j] = double_random(1.0, 10.0);
		}
	}

	return sq_array;
}


/*
 * Check that malloc correctly allocated the amount of space required for the 
 * square array of doubles.
 * If not, exit the program with a failure.
 */
void check_malloc(double ** sq_array) {
	if (sq_array == NULL) {
		fprintf(stderr, "Failed to allocate space for the array.\n");
		exit(EXIT_FAILURE);
	}
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
