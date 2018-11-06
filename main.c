/*
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * 
 * Relaxation technique for solving differential equations in a shared memory 
 * architecture using pthreads
 *
 * Local usage: 
 * 1) "gcc main.c -o main.exe -Wall -Wextra -Wconversion"
 * 2) "main.exe"
 * 
 * Author: aj645
 * Date: 19-Nov-2018
 */

#include <stdio.h>
#include <stdlib.h>
#define dim 4


// Function definitions
double ** create_square_array(void);
double ** average_array(double **square_array);
void check_malloc(double **);
double double_random(double low, double high);
void print_array(double **array);


/*
 * Program entry.
 */
int main() {
	double **square_array = create_square_array();
	printf("initial array:\n");
	print_array(square_array);

	square_array = average_array(square_array);
	printf("updated array:\n");
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
	for (int i = 0; i < dim; i++) {
		sq_array[i] = malloc(dim * sizeof(double));
		check_malloc(sq_array);
	}

	// populate the array with random doubles
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			sq_array[i][j] = double_random(1.0, 10.0);
		}
	}

	return sq_array;
}


/**
 * Loops through the square array to replace a value with an average of its 4
 * neighbouring values (left, right, up and down). Does not update boundary
 * values.
 * Returns a new square array with the updated values.
 */
double ** average_array(double **temp_array) {
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			if ( (!((i==0) || (i==dim-1))) && (!((j==0) || (j==dim-1))) ) {
				// element to average
				double element = temp_array[i][j]; 

				// get 4 surrounding elements needed to average
				double ele_left = temp_array[i][j-1];
				double ele_right = temp_array[i][j+1];
				double ele_up = temp_array[i-1][j];
				double ele_down = temp_array[i+1][j];

				// print data
				printf("element of interest: %f\n", element);
				printf("ele_left: %f\n", ele_left);
				printf("ele_right: %f\n", ele_right);
				printf("ele_up: %f\n", ele_up);
				printf("ele_down: %f\n", ele_down);

				// perform the calculation
				double new_value = (ele_left + ele_right + ele_up + ele_down) / 4;
				printf("New value = %f\n", new_value);
				printf("\n");

				// replace the old value with the new one
				temp_array[i][j] = new_value;
			}
		}
	}
	return temp_array;
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
 	printf("\n");
}