/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Source file for functions used to manipulate arrays.
 * 
 * Author: Adam Jaamour
 * Date: 30-Nov-2018
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "array_helpers.h"


/*
 * Initialises a square array by creating an 1D array of doubles while keeping 
 * track of rows.
 * Populates it with random doubles ranging from 1.0 to 100.0.
 */
double* initialise_square_array(int dim) {
	long unsigned int dimension = (long unsigned int) dim;
	double *sq_array;
	int i, j;

	// allocate space for a 1D array of double pointers.
	sq_array = malloc(dimension * dimension * sizeof(double));
	check_double_malloc(sq_array);

	// populate the array with random doubles
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			sq_array[i*dim+j] = (double)(rand() % 100);
		}
	}

	return sq_array;
}


/*
 * Initialises (doesn't populate) an non-square array of dimensions X times Y by 
 * creating an 1D array of doubles while keeping track of rows.
 */
double* create_new_array(int dim_x, int dim_y) {
	long unsigned int dimension_x = (long unsigned int) dim_x;
	long unsigned int dimension_y = (long unsigned int) dim_y;
	double *array;
 
	// allocate space for a 1D array of double pointers.
	array = malloc(dimension_x * dimension_y * sizeof(double));
	check_double_malloc(array);
	
	return array;
}


/*
 * Returns a portion of a square array using the start row and the end row to
 * select from.
 */
double* select_chunk(int dim, double* sq_array, int start_row, int end_row) {
	int diff = end_row - start_row + 1;
	double* chunk_array = create_new_array(diff,dim);
	int i, j;
	
	for (i = start_row; i < start_row + diff; i++) {
		for (j = 0; j < dim; j++) {
			chunk_array[(i-start_row) * dim + j] = sq_array[i* dim + j];
		}
	}
	
	return chunk_array;
}


/*
 * Updates the appropriate values in the square array's rows given the updates 
 * chunk.
 */
double* stitch_array(double* sq_array, double* chunk_array, int start_row, int end_row, int dim){
	int i, j;
	
	for (i = start_row + 1; i < end_row; i++) {
		for (j = 1; j < dim - 1; j++) {
			sq_array[i*dim+j] = chunk_array[(i-start_row) * dim + j];
		}
	}
	
	return sq_array;
}


/*
 * Check that malloc correctly allocated the amount of space required for the 
 * square array of doubles.
 * If not, exit the program with a failure.
 */
void check_double_malloc(double* square_array) {
	if (square_array == NULL) {
		fprintf(stderr, "Failed to allocate space for the array.\n");
		exit(EXIT_FAILURE);
	}
}