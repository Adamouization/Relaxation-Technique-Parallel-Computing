/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Source file for functions used to print data on the command line.
 * 
 * Author: aj645
 * Date: 07-Jan-2019
 */
 
#include <stdio.h>
#include "print_helpers.h"


/*
 * Prints the initial data values used to initiate the program.
 */
void print_parameters(int dimension, int num_processes, double precision) {
	printf("\nArray dimension: %d\n", dimension);
	printf("World size: %d\n", num_processes);
	printf("Precision: %f\n", precision);
	printf("\n");
}


/*
 * Prints a square array to the command line.
 */
void print_square_array(int dim, double* sq_array) {
	int i, j;
	for (i = 0; i < dim; i++) {
 		for (j = 0; j < dim; j++) {
 			printf("%f ", sq_array[i * dim + j]);
 		}
 		printf("\n");
 	}
}


/*
 * Prints the values used to update a value of the square array in a single 
 * iteration.
 */
void print_relaxation_values_data(double old, double l, double r, double u, 
	double d, double new) {
	printf("Replacing %f with %f (l: %f, r: %f, u: %f, d: %f)\n", old, new, l, 
		r, u, d);
}