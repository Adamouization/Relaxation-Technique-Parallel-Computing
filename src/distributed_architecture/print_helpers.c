/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Source file for functions used to print data on the command line.
 * 
 * Author: aj645
 * Date: 30-Nov-2018
 */
 
#include <stdio.h>
#include "print_helpers.h"


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
 * Prints an non-square array of dimensions X and Y to the command line.
 */
void print_non_square_array(int dim_x, int dim_y, double* arr) {
	int i, j;
	for (i = 0; i < dim_y; i++) {
 		for (j = 0; j < dim_x; j++) {
 			printf("%f ", arr[i * dim_x + j]);
 		}
 		printf("\n");
 	}
}