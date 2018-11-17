/**
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * 
 * Source file for functions used to print data on the command line.
 * 
 * Author: aj645
 * Date: 19-Nov-2018
 */
 
#include <stdio.h>
#include "print_helpers.h"


/*
 * Prints the initial data values used to initiate the program.
 */
void print_parameters(int dimension, int num_thr, float precision, 
	float** square_array) {
	printf("\nArray dimension: %d\n", dimension);
	printf("Number of threads: %d\n", num_thr);
	printf("Precision: %f\n", precision);
	printf("Initial square array:\n");
	print_array(dimension, square_array);
	printf("\n");
}


/*
 * Prints an array to the command line.
 */
void print_array(int dimension, float** square_array) {
	int i, j;
	for (i = 0; i < dimension; i++) {
 		for (j = 0; j < dimension; j++) {
 			printf("%f ", square_array[i][j]);
 		}
 		printf("\n");
 	}
}


/*
 * Prints the values describing the current thread executing along with counters
 * indicating the relaxation technique's progress.
 */
void print_relaxation_thread_data(int tid, int updates_counter, int i, int j) {
	printf("Thread %d executing at [%d,%d] (iteration #%d)\n", tid, i, j, 
		updates_counter);
}


/*
 * Prints the values used to update a value of the square array in a single 
 * iteration.
 */
void print_relaxation_values_data(float old, float l, float r, float u, float d, 
	float new, int prec_count) {
	printf("Replacing %f with %f (l: %f, r: %f, u: %f, d: %f)\n", old, new, l, 
		r, u, d);
	printf("Precision is %d\n\n", prec_count);
}


/*
 * Prints the parameters used at the end of the script for logging purposes.
 */
void print_final_results(int dimension, int num_thr, float precision) {
	printf("Threads used: %d\n", num_thr);
	printf("Array dimension: %d\n", dimension);
	printf("Precision used: %f\n", precision);
}