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
 
 
 83.000000 86.000000 77.000000 15.000000 93.000000 35.000000 86.000000
92.000000 49.000000 21.000000 62.000000 27.000000 90.000000 59.000000
63.000000 26.000000 40.000000 26.000000 72.000000 36.000000 11.000000
68.000000 67.000000 29.000000 82.000000 30.000000 62.000000 23.000000
67.000000 35.000000 29.000000 2.000000 22.000000 58.000000 69.000000
67.000000 93.000000 56.000000 11.000000 42.000000 29.000000 73.000000
21.000000 19.000000 84.000000 37.000000 98.000000 24.000000 15.000000

 
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


double** create_new_array(int dim_x, int dim_y) {
	long unsigned int dimension_x = (long unsigned int) dim_x;
	long unsigned int dimension_y = (long unsigned int) dim_y;
	double **arr;
	int i, j;

	// allocate space for a 1D array of double pointers.
	arr = malloc(dimension_x * sizeof(double*));
	check_double_malloc(arr);

	// allocate space for multiple 1D arrays of doubles
	for (i = 0; i < dimension_y; i++) {
		arr[i] = malloc(dimension_y * sizeof(double));
		check_double_malloc(arr);
	}

	return arr;
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


void print_non_square_array(int x, int y, double** square_array) {
	int i, j;
	for (i = 0; i < y; i++) {
 		for (j = 0; j < x; j++) {
 			printf("%f ", square_array[i][j]);
 		}
 		printf("\n");
 	}
}


double** select_chunk(int dimension, double** square_array, int start_row, int end_row) {
	int difference = end_row - start_row + 1;
	double** chunk_array = create_new_array(difference,dimension);
	int i, j;
	for (i = start_row; i < difference+start_row; i++) {
		for (j = 0; j < dimension; j++) {
			chunk_array[i-start_row][j] = square_array[i][j];
		}
	}
	
	return chunk_array;
}

int main() {
	int DEBUG = 1;
	int dimension = 7;
	double precision = 0.01f;
	int world_size;
	int world_rank;
	
    int rc = MPI_Init(NULL, NULL); // Initialize the MPI environment.
	if (rc != MPI_SUCCESS) {
		printf ("Error starting MPI program\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Get the rank of the process
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	// papa process
	if (world_rank == 0) {
		double **square_array = initialise_square_array(dimension);
		print_array(dimension, square_array);
			printf("\n");
			printf("\n");
		
		
		double** chunk1 = select_chunk(dimension, square_array, 0, 3);
		//print_non_square_array(7,4,chunk1);
		// send data to child process #1
		int i;
		MPI_Send(&dimension, 1, MPI_INT, 1, 99, MPI_COMM_WORLD);
		for (i = 0; i < 4; i++)
		{
			int n_to_pass=(dimension+1)*(i+1)-2;
			MPI_Send(&chunk1[n_to_pass], dimension, MPI_DOUBLE, 1, 99, MPI_COMM_WORLD);
		}
		// send data to child process #2

		
		
		
		double** chunk2 = select_chunk(dimension, square_array, 2, 5);
		//print_non_square_array(7,4,chunk2);
			//printf("\n");
		MPI_Send(&dimension, 1, MPI_INT, 2, 99, MPI_COMM_WORLD);
		for (i = 0; i < 4; i++)
		{
			int n_to_pass=(dimension+1)*(i+2+1)-2;
			MPI_Send(&chunk2[n_to_pass], dimension, MPI_DOUBLE, 2, 99, MPI_COMM_WORLD);

		}
		
		
		
		double** chunk3 = select_chunk(dimension, square_array, 4, 6);
		//print_non_square_array(7,3,chunk3);
			//printf("\n");
		MPI_Send(&dimension, 1, MPI_INT, 3, 99, MPI_COMM_WORLD);
		int j;
		//for (i = 0; i < 3; i++)
		//{
			/*for (j = 0; j < dimension; j++)
			{
				printf("%f ", chunk3[i][j]);
			}
			printf("\n");*/
			
			int n_to_pass=(dimension+1)*(i+1)-2;
			printf("n_to_pass: %d\n",n_to_pass);
			MPI_Send(&chunk3[46], dimension, MPI_DOUBLE, 3, 99, MPI_COMM_WORLD);

		//}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
	}
	
	// child process #1
	else if (world_rank == 1){ // do rows 1 and 2
		int my_dimension;
		
		MPI_Recv(&my_dimension, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		double *my_chunk1 = malloc(my_dimension * sizeof(double));
		
		int i, j;
		for (i = 0; i < 4; i++)
		{
			MPI_Recv(&my_chunk1[0], my_dimension, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			/*for (j = 0; j < my_dimension; j++)
			{
				printf("%f ", my_chunk1[j]);
			}
			printf("\n");*/
		}
		printf("Hello world from processor #%d out of %d processors\n", world_rank, world_size);
		printf("\n");
	}
	
	
	
	
	
	// child process #2
	else if (world_rank == 2){ // do rows 3 and 4
		int my_dimension1;
		MPI_Recv(&my_dimension1, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		double *my_chunk2 = malloc(my_dimension1 * sizeof(double));
		int i, j;
		
			printf("\n");
		for (i = 0; i < 4; i++)
		{
			MPI_Recv(&my_chunk2[0], my_dimension1, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			/*for (j = 0; j < my_dimension1; j++)
			{
				printf("%f ", my_chunk2[j]);
			}
			printf("\n");*/
		}
		printf("Hello world from processor #%d out of %d processors\n", world_rank, world_size);
		printf("\n");
	}
	
	
	
	
	// child process #3
	else if (world_rank == 3){ // do rows 5 only
		int my_dimension2;
		MPI_Recv(&my_dimension2, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		double *my_chunk3 = malloc(my_dimension2 * sizeof(double));
		int i, j;
		
			printf("\n");
		for (i = 0; i < 3; i++)
		{
			MPI_Recv(&my_chunk3, my_dimension2, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (j = 0; j < my_dimension2; j++)
			{
				printf("%f ", my_chunk3[j]);
			}
			printf("\n");
			break;
		}
		printf("Hello world from processor #%d out of %d processors\n", world_rank, world_size);
		printf("\n");
	}
		
		
		
		
		
		
		
		
		
	
    MPI_Finalize(); // Clean up the MPI environment.
	//return 0;
}