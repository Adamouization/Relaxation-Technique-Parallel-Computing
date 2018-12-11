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
#include "array_helpers.h"
#include "print_helpers.h"
int DEBUG = false;


/*
 * 
 */
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


/*
 * 
 */
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


/*
 * Program entry.
 */
int main(int argc, char** argv) {
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
	
	// Get the number of processes and the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	
	
	// Kill program if less than 2 processes
	if (world_size < 2) {
		fprintf(stderr, "World size must be greater than 2 for %s\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	
	// Root process
	if (world_rank == 0) {
		double *square_array = initialise_square_array(dimension);
		
		if (true) {
			print_square_array(dimension, square_array);
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
		print_square_array(dimension, square_array);
	}
	
	// child process #1 (do rows 1 and 2)
	else if (world_rank == 1){
		int my_dimension;
		MPI_Recv(&my_dimension, 1, MPI_INT, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		double *my_chunk1 = malloc((long unsigned int)my_dimension * sizeof(double));
		MPI_Recv(my_chunk1, my_dimension, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if (DEBUG) {
			print_non_square_array(7, 4, my_chunk1);
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
			print_non_square_array(7, 4, my_chunk1);
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
			print_non_square_array(7, 3, my_chunk1);
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