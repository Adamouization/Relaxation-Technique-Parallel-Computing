/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Relaxation technique for solving differential equations in a distributed 
 * memory architecture using MPI (Message Passing Interface)
 *
 * Local usage: 
 * 1) "mpicc -Wall -Wextra -Wconversion main.c -o distributed_relaxation -lm"
 * 2) "mpirun -np <number_of_processes> ./distributed_relaxation"
 * 
 * Author: Adam Jaamour
 * Date: 30-Nov-2018
 */

/*
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
#include <math.h>
#include <stdbool.h>
#include <mpi.h>
int DEBUG = false;


void check_double_malloc(double* square_array) {
	if (square_array == NULL) {
		fprintf(stderr, "Failed to allocate space for the array.\n");
		exit(EXIT_FAILURE);
	}
}


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
			//sq_array[i][j] = (double)(i*dim+j*j+1);
		}
	}

	return sq_array;
}


double* create_new_array(int dim_x, int dim_y) {
	long unsigned int dimension_x = (long unsigned int) dim_x;
	long unsigned int dimension_y = (long unsigned int) dim_y;
	double *arr;
 
	// allocate space for a 1D array of double pointers.
	arr = malloc(dimension_x * dimension_y * sizeof(double));
	check_double_malloc(arr);
	
	return arr;
}


void print_array(int dimension, double* square_array) {
	int i, j;
	for (i = 0; i < dimension; i++) {
 		for (j = 0; j < dimension; j++) {
 			printf("%f ", square_array[i*dimension+j]);
 		}
 		printf("\n");
 	}
}


void print_non_square_array(int y, int x, double* square_array) {
	int i, j;
	for (i = 0; i < y; i++) {
 		for (j = 0; j < x; j++) {
 			printf("%f ", square_array[i*x+j]);
 		}
 		printf("\n");
 	}
}


int get_range_from_p_num_start(int dimension, int number_of_processes, int process_rank) {
	float div = (float)dimension / (float)number_of_processes;
	int chunk_height = (int)ceil(div);
	if (DEBUG) {
		printf("div: %f \n",div);
		printf("dimension: %d \n",dimension);
		printf("number_of_processes: %d \n",number_of_processes);
		printf("chunk_height: %d \n",chunk_height);
	}
	int start_height = (process_rank-1)*chunk_height;	
	return start_height;
}


int get_range_from_p_num_end(int dimension, int number_of_processes, int process_rank) {
	float div= (float)dimension / (float)number_of_processes;
	int chunk_height = (int)ceil(div);
	if (DEBUG) {
		printf("div: %f \n",div);
		printf("dimension: %d \n",dimension);
		printf("number_of_processes: %d \n",number_of_processes);
		printf("chunk_height: %d \n",chunk_height);
	}
	int start_height = (process_rank-1)*chunk_height;
	int end_height = start_height+chunk_height+1;
	
	if(process_rank == number_of_processes) {
		return dimension+1;
	} else {
		return end_height;
	}
}


double* stitch_array(double* sq_array, double* chunk_array, int start, int end, int dimension){
	int i, j;
	for (i = start+1; i < end; i++) {
		//printf("i: %d \n",i);
		for (j = 1; j < dimension - 1; j++) {
			sq_array[i*dimension+j] = chunk_array[(i-start)*dimension+j];
			//printf("%d ",j);
		}
		//printf("\n");
	}
	return sq_array;
}


double* select_chunk(int dimension, double* square_array, int start_row, int end_row) {
	int difference = end_row - start_row + 1;
	double* chunk_array = create_new_array(difference,dimension);
	int i, j;
	for (i = start_row; i < difference+start_row; i++) {
		for (j = 0; j < dimension; j++) {
			chunk_array[(i-start_row)*(dimension)+j] = square_array[i*(dimension)+j];
			//printf("%f ", chunk_array[(i-start_row)*(dimension)+j]);
		}
 		//printf("\n");
	}
	return chunk_array;
}


int main() {
	int dimension = 7;
	int entire_dimension;
	//double precision = 0.01f;
	int start, end;
	int world_size, world_rank, rc;
	
	// Initialize the MPI environment.
    rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS) {
		printf ("Error starting MPI program\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Get the rank of the process
	
	// Root process
	if (world_rank == 0) {
		double *square_array = initialise_square_array(dimension);
		
		if (true) {
			print_array(dimension, square_array);
			printf("\n");
		}
		
		// send data to child process #1
		double* chunk1 = select_chunk(dimension, square_array, 0, 3);
		entire_dimension=dimension*4;
		MPI_Send(&entire_dimension, 1, MPI_INT, 1, 99, MPI_COMM_WORLD);
		MPI_Send(chunk1, entire_dimension, MPI_DOUBLE, 1, 99, MPI_COMM_WORLD);
		
		// send data to child process #2
		chunk1 = select_chunk(dimension, square_array, 2, 5);
		entire_dimension=dimension*4;
		MPI_Send(&entire_dimension, 1, MPI_INT, 2, 99, MPI_COMM_WORLD);
		MPI_Send(chunk1, entire_dimension, MPI_DOUBLE, 2, 99, MPI_COMM_WORLD);

		// send data to child process #3
		chunk1 = select_chunk(dimension, square_array, 4, 6);
		entire_dimension=dimension*3;
		MPI_Send(&entire_dimension, 1, MPI_INT, 3, 99, MPI_COMM_WORLD);
		MPI_Send(chunk1, entire_dimension, MPI_DOUBLE, 3, 99, MPI_COMM_WORLD);
		
		// receive updated chunks from process #1 and place them back in square array
		MPI_Recv(chunk1, dimension*4, MPI_DOUBLE, 1, 66, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		start=get_range_from_p_num_start(dimension-2, world_size-1, 1);
		end=get_range_from_p_num_end(dimension-2, world_size-1, 1);
		square_array = stitch_array(square_array, chunk1, start, end, dimension);
		
		// receive updated chunks from process #2 and place them back in square array
		MPI_Recv(chunk1, dimension*4, MPI_DOUBLE, 2, 66, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		start=get_range_from_p_num_start(dimension-2, world_size-1, 2);
		end=get_range_from_p_num_end(dimension-2, world_size-1, 2);
		square_array = stitch_array(square_array, chunk1, start, end, dimension);
		
		// receive updated chunks from process #3 and place them back in square array
		MPI_Recv(chunk1, dimension*3, MPI_DOUBLE, 3, 66, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		start=get_range_from_p_num_start(dimension-2, world_size-1, 3);
		end=get_range_from_p_num_end(dimension-2, world_size-1, 3);
		square_array = stitch_array(square_array, chunk1, start, end, dimension);
		
		// finish and print final array
		print_array(dimension, square_array);
	}
	
	// child process #1 (do rows 1 and 2)
	else if (world_rank == 1){
		int my_dimension;
		MPI_Recv(&my_dimension, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		double *my_chunk1 = malloc((long unsigned int)my_dimension * sizeof(double));
		MPI_Recv(my_chunk1, my_dimension, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if (DEBUG) {
			print_non_square_array(4,7,my_chunk1);
			printf("Hello world from processor #%d out of %d processors\n", world_rank, world_size);
		}
		
		// todo - perform relaxation on chunk here (instead of increment by 10)
		int i,j;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < my_dimension/4; j++) {
				my_chunk1[i*(my_dimension/4)+j] = my_chunk1[i*(my_dimension/4)+j] + 10;
			}
		}
		// send chunk back to root process
		MPI_Send(my_chunk1, my_dimension, MPI_DOUBLE, 0, 66, MPI_COMM_WORLD);
	}
	
	// child process #2 do rows 3 and 4
	else if (world_rank == 2){
		int my_dimension;
		MPI_Recv(&my_dimension, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		double *my_chunk1 = malloc((long unsigned int)my_dimension * sizeof(double));
		MPI_Recv(my_chunk1, my_dimension, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if (DEBUG) {
			print_non_square_array(4,7,my_chunk1);
			printf("Hello world from processor #%d out of %d processors\n", world_rank, world_size);
		}
		
		// todo - perform relaxation on chunk here (instead of increment by 10)
		int i,j;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < my_dimension/4; j++) {
				my_chunk1[i*(my_dimension/4)+j] = my_chunk1[i*(my_dimension/4)+j] + 10;
			}
		}
		// send chunk back to root process
		MPI_Send(my_chunk1, my_dimension, MPI_DOUBLE, 0, 66, MPI_COMM_WORLD);
	}
	
	// child process #3 do row 5 only
	else if (world_rank == 3){
		int my_dimension;
		MPI_Recv(&my_dimension, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		double *my_chunk1 = malloc((long unsigned int)my_dimension * sizeof(double));
		MPI_Recv(my_chunk1, my_dimension, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if (DEBUG) {
			print_non_square_array(3,7,my_chunk1);
			printf("Hello world from processor #%d out of %d processors\n", world_rank, world_size);
		}
		
		// todo - perform relaxation on chunk here (instead of increment by 10)
		int i,j;
		for (i = 0; i < 3; i++) {
			for (j = 0; j < my_dimension/3; j++) {
				my_chunk1[i*(my_dimension/3)+j] = my_chunk1[i*(my_dimension/3)+j] + 10;
			}
		}
		// send chunk back to root process
		MPI_Send(my_chunk1, my_dimension, MPI_DOUBLE, 0, 66, MPI_COMM_WORLD);
	}
	
	// Clean up the MPI environment.
    MPI_Finalize();
}