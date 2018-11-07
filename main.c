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
#include <stdbool.h>
#include <math.h>
#define dim 4


// Function definitions
double ** create_square_array(void);
// double ** relaxation(double **square_array, double precision);
double ** relaxation(double **square_array, double precision);
void check_malloc(double **sq_array);
double double_random(double low, double high);
void print_initial_data(double **array, int dimension, int threads, double precision);
void print_array(double **array);


/*
 * Program entry.
 */
int main() {
	// initial values
	double **square_array = create_square_array();
	int array_dimension = dim;
	int number_of_threads = 2;
	double precision = 0.1;
	print_initial_data(square_array, array_dimension, number_of_threads, precision);

	// iterate averaging until precision reached
	square_array = relaxation(square_array, precision);
	printf("\n------------------------ Final square array\n");
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
			//sq_array[i][j] = rand() % 10;
			sq_array[i][j] = double_random(1.0, 10.0);
		}
	}

	return sq_array;
}


/**
 * Loops through the square array to replace a value with an average of its 4
 * neighbouring values (left, right, up and down). Does not ((update boundary
 * values.
 * Returns a new square array with the updated values.
 */
double ** relaxation(double **temp_array, double precision) {
	bool is_above_precision = true;
	int precision_counter = 0;
	int iteration_counter = 0;
	double difference = 0.0; // different between old and new value
	int number_of_values_to_change = ((dim-2) * (dim-2));

	while (is_above_precision) {
		precision_counter = 0;
		printf("\n------------------------ Iteration #%d\n", iteration_counter);
		for (int i = 0; i < dim; i++) {
			for (int j = 0; j < dim; j++) {
				// filter out border values
				if ( (!((i==0) || (i==dim-1))) && (!((j==0) || (j==dim-1))) ) {
					// value to replace
					double old_value = temp_array[i][j]; 

					// get 4 surrounding values needed to average
					double ele_left = temp_array[i][j-1];
					double ele_right = temp_array[i][j+1];
					double ele_up = temp_array[i-1][j];
					double ele_down = temp_array[i+1][j];

					// perform the calculation
					double new_value = (ele_left + ele_right + ele_up + ele_down) / 4;

					// check if difference is smaller than precision
					difference = fabs(old_value - new_value);
					if (difference < precision) {
						precision_counter++;
					}

					// if difference smaller than precision for all values, stop
					if (precision_counter == number_of_values_to_change) {
						is_above_precision = false;
					}

					// replace the old value with the new one
					temp_array[i][j] = new_value;

					// print data
					printf("Value to replace: %f\n", old_value);
					/*printf("ele_left: %f\n", ele_left);
					printf("ele_right: %f\n", ele_right);
					printf("ele_up: %f\n", ele_up);
					printf("ele_down: %f\n", ele_down);*/
					printf("New value = %f\n", new_value);
					printf("precision_counter: %d\n", precision_counter);
				}
			}
		}
		iteration_counter++;
	}
	return temp_array;
}


/*
 * Check that malloc correctly allocated the amount of space required for the 
 * square array of doubles.
 * If not, exit the program with a failure.
 */
void check_malloc(double **sq_array) {
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


/**
 * Prints the initial data values used to initiate the program.
 */
void print_initial_data(double **array, int dimension, int threads, double precision) {
	printf("------------------------ Initial data:\n");
	printf("Array dimension: %d\n", dimension);
	printf("Number of threads: %d\n", threads);
	printf("Precision: %f\n", precision);
	printf("Square array:\n");
	print_array(array);
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