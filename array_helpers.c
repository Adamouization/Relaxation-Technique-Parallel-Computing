/**
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * 
 * Source file for functions used to manipulate arrays.
 * 
 * Author: aj645
 * Date: 19-Nov-2018
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "array_helpers.h"


/*
 * Initialises a square array by creating an initial array of pointers, each 
 * looking at 1D arrays of doubles.
 */
float** initialise_square_array(int dim) {
	long unsigned int dimension = (long unsigned int) dim;
	float **sq_array;
	int i, j;

	// allocate space for a 1D array of float pointers.
	sq_array = malloc(dimension * sizeof(float*));
	check_malloc(sq_array);

	// allocate space for multiple 1D arrays of doubles
	for (i = 0; i < dim; i++) {
		sq_array[i] = malloc(dimension * sizeof(float));
		check_malloc(sq_array);
	}

	// populate the array with random doubles
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			//sq_array[i][j] = (i*dim) + (j*j*j*j) + 10;
			sq_array[i][j] = (float)(rand() % 100);
		}
	}
	return sq_array;
}


/*
 * 
 */
pthread_mutex_t** initialise_mutex_array(int dim) {
	long unsigned int dimension = (long unsigned int) dim;
	pthread_mutex_t** mtx_array;
	int i, j;

	// allocate space for a 1D array of float mutexes.
	mtx_array = malloc(dimension * sizeof(pthread_mutex_t*));
	check_mutex_malloc(mtx_array);

	// allocate space for multiple 1D arrays of doubles
	for (i = 0; i < dim; i++) {
		mtx_array[i] = malloc(dimension * sizeof(pthread_mutex_t));
		check_mutex_malloc(mtx_array);
	}

	// populate the array with initialised mutexes
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			pthread_mutex_init(&mtx_array[i][j], NULL);
		}
	}

	return mtx_array;
}


/*
 * Check that malloc correctly allocated the amount of space required for the 
 * square array of doubles.
 * If not, exit the program with a failure.
 */
void check_malloc(float** square_array) {
	if (square_array == NULL) {
		fprintf(stderr, "Failed to allocate space for the array.\n");
		exit(EXIT_FAILURE);
	}
}


/**
 *
 */
void check_mutex_malloc(pthread_mutex_t** mutex_array) {
	if (mutex_array == NULL) {
		fprintf(stderr, "Failed to allocate space for the mutex array.\n");
		exit(EXIT_FAILURE);
	}
}