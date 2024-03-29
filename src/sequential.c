/**
 * SEQUENTIAL VERSION
 * author: Adam Jaamour
 *
 * gcc sequential.c -o sequential.exe -Wall -Wextra -Wconversion
 * ./sequential
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>


// Function definitions
void initialise_square_array(void);
void relaxation(double precision);
void check_malloc(void);
double double_random(double low, double high);
void print_initial_data(double precision);
void print_array(void);
void print_relaxation_data(double old, double l, double r, double u, double d, double new, int precision);


// Global variables
bool DEBUG = false;				// print data to the command line
#define dim 7					// square array dimensions
#define SEED 1000
struct timeval time1, time2;	// structure used to calculate program time
double **square_array;			// global square array of double


/*
 * Program entry.
 */
int main() {
	// initialise values
	double precision = 1; // precision to perform relaxation at
	initialise_square_array();
	print_initial_data(precision);

	// iterate averaging until precision reached
	gettimeofday(&time1, NULL);	// start recording time
	relaxation(precision);
	gettimeofday(&time2, NULL);	// stop recording time
	printf ("Total time = %f seconds\n\n", 
		(double) (time2.tv_usec - time1.tv_usec) / 1000000 +
		(double) (time2.tv_sec - time1.tv_sec));
	
	if (DEBUG) {
		printf("\n------------------------ Final square array\n");
		print_array();
	}

	// free allocated array space and successfully exit program
 	free(square_array);
   	return 0;
}


/*
 * Initialises a square array by creating an initial array of pointers, each 
 * looking at 1D arrays of doubles.
 */
void initialise_square_array(void) {
	srand(SEED);
	int i, j;
	
	// allocate space for a 1D array of double pointers.
	square_array = malloc(dim * sizeof(double*));
	check_malloc();

	// allocate space for multiple 1D arrays of doubles
	for (i = 0; i < dim; i++) {
		square_array[i] = malloc(dim * sizeof(double));
		check_malloc();
	}

	// populate the array with random doubles
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			//square_array[i][j] = (i*dim) + (j*j*j*j) + 10;
			//square_array[i][j] = double_random(1.0, 10.0);
			square_array[i][j] = (double)(rand() % 10);
		}
	}
}


/*
 * Loops through the square array to replace a value with an average of its 4
 * neighbouring values (left, right, up and down). Loops until all the values 
 * changed differ by less than the precision specified at the start of the
 * program. Does not update boundary values.
 * Returns a new square array with the updated values.
 */
void relaxation(double precision) {
	bool is_above_precision = true;
	int precision_counter = 0;
	int iteration_counter = 0;
	double difference = 0.0; // different between old and new value
	int number_of_values_to_change = ((dim-2) * (dim-2));
	int i, j;

	while (is_above_precision) {
		precision_counter = 0;
		if (DEBUG) printf("\n------------------------ Iteration #%d\n", iteration_counter);
		for (i = 1; i < dim - 1; i++) {
			for (j = 1; j < dim - 1; j++) {
				// value to replace
				double old_value = square_array[i][j]; 

				// get 4 surrounding values needed to average
				double v_left = square_array[i][j-1];
				double v_right = square_array[i][j+1];
				double v_up = square_array[i-1][j];
				double v_down = square_array[i+1][j];

				// perform the calculation
				double new_value = (v_left + v_right + v_up + v_down) / 4;
				
				// replace the old value with the new one
				square_array[i][j] = new_value;

				// check if difference is smaller than precision
				difference = (double)fabs(old_value - new_value);
				if (difference < precision) {
					precision_counter++;
				} else { // reset precision counter if diff smaller than prec
					precision_counter = 0;
				}

				// if difference smaller than precision for all values, stop
				if (precision_counter >= number_of_values_to_change) {
					is_above_precision = false;
				}

				// print data
				if (DEBUG) {
					print_relaxation_data(old_value, v_left, v_right, v_up, 
					v_down, new_value, precision_counter);
				}
			}
			if (!(is_above_precision)) { // check at end of current array iter
				break;
			}
		}
		iteration_counter++;
	}
}


/*
 * Check that malloc correctly allocated the amount of space required for the 
 * square array of doubles.
 * If not, exit the program with a failure.
 */
void check_malloc(void) {
	if (square_array == NULL) {
		fprintf(stderr, "Failed to allocate space for the array.\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * Prints the initial data values used to initiate the program.
 */
void print_initial_data(double precision) {
	printf("\nArray dimension: %d\n", dim);
	printf("Precision: %f\n\n", precision);
	if (DEBUG) {
		printf("Initial square array:\n");
		print_array();
	}
}


/*
 * Prints an array to the command line.
 */
void print_array(void) {
	int i, j;
	for (i = 0; i < dim; i++) {
 		for (j = 0; j < dim; j++) {
 			printf("%f ", square_array[i][j]);
 		}
 		printf("\n");
 	}
}


/*
 * Prints the values used for each iteration of the relaxation technique.
 */
void print_relaxation_data(double old, double l, double r, double u, double d, double new, int precision) {
	printf("Value to replace: %f\n", old);
	printf("l: %f, r: %f, u: %f, d: %f\n", l, r, u, d);
	printf("New value = %f\n", new);
	printf("precision_counter: %d\n", precision);
	printf("------\n");
}
