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
#include <stdlib.h>
#include "print_helpers.h"


/*
 * Prints the initial data values used to initiate the program.
 */
void print_initial_data(int dimension, int num_thr, double precision, 
	double** square_array) {
	printf("------------------------ Initial data:\n");
	printf("Array dimension: %d\n", dimension);
	printf("Number of threads: %d\n", num_thr);
	printf("Precision: %f\n", precision);
	printf("Square array:\n");
	print_array(dimension, square_array);
}


/*
 * Prints an array to the command line.
 */
void print_array(int dimension, double** square_array) {
	int i, j;
	for (i = 0; i < dimension; i++) {
 		for (j = 0; j < dimension; j++) {
 			printf("%f ", square_array[i][j]);
 		}
 		printf("\n");
 	}
}


/*
 * Prints the values used for each iteration of the relaxation technique.
 */
void print_relaxation_data(double old, double l, double r, double u, double d, 
	double new, int precision) {
	printf("Value to replace: %f\n", old);
	printf("l: %f, r: %f, u: %f, d: %f\n", l, r, u, d);
	printf("New value = %f\n", new);
	printf("precision_counter: %d\n", precision);
	printf("------\n");
}