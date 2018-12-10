/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Relaxation technique for solving differential equations in a distributed memory 
 * architecture using MPI (Message Passing Interface)
 *
 * Local usage: 
 * 1) "mpicc main.c -o distributed_relaxation"
 * 2) "mpirun -np <number_of_processes> ./distributed_relaxation"
 * 
 * Author: Adam Jaamour
 * Date: 30-Nov-2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


void check_double_malloc(double** square_array) {
	if (square_array == NULL) {
		fprintf(stderr, "Failed to allocate space for the array.\n");
		exit(EXIT_FAILURE);
	}
}


 * Initialises a square array by creating an initial array of pointers, each 
 * looking at a 1D arrays of doubles.
 */
double** initialise_square_array(int dim) {
	long unsigned int dimension = (long unsigned int) dim;
	double **sq_array;
	int i, j;

	// allocate space for a 1D array of double pointers.
	sq_array = malloc(dimension * sizeof(double*));
	check_double_malloc(sq_array);

	// allocate space for multiple 1D arrays of doubles
	for (i = 0; i < dim; i++) {
		sq_array[i] = malloc(dimension * sizeof(double));
		check_double_malloc(sq_array);
	}

	// populate the array with random doubles
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			sq_array[i][j] = (double)(rand() % 100);
			//sq_array[i][j] = (double)(i*dim+j*j+1);
		}
	}

	return sq_array;
}


void print_array(int dimension, double** square_array) {
	int i, j;
	for (i = 0; i < dimension; i++) {
 		for (j = 0; j < dimension; j++) {
 			printf("%f ", square_array[i][j]);
 		}
 		printf("\n");
 	}
}


int main() {
	bool DEBUG = false;
	int dimension = 5;
	double precision = 0.01f;
	int world_size;
	int world_rank;
	
    MPI_Init(NULL, NULL); // Initialize the MPI environment.
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Get the rank of the process
	
	// papa process
	if (world_rank == 0) {
		double **square_array = initialise_square_array(dimension);
		print_array(dimension, square_array);
		printf("suck my rubber duck\n");
	}
	
    // Print off a hello world message
    printf("Hello world from processor #%d out of %d processors\n", world_rank, world_size);

    MPI_Finalize(); // Clean up the MPI environment.
}