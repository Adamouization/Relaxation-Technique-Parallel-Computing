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
 * Date: 07-Jan-2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <mpi.h>
#include "array_helpers.h"
#include "print_helpers.h"

#define send_tag 1
#define recv_tag 2

int DEBUG = false;
struct sub_arr_rows {
	int start;
	int end;
};


int main(int argc, char** argv) {
	int dimension, p_dimension, root_process, num_elements_to_send, 
		num_elements_to_recv, num_sub_arr_rows, world_size, world_rank, rc;
	double* sub_arr; // chunk of main square array with specific rows only
	struct sub_arr_rows rows;
	//double precision = 0.01f;
	
	// initialise variables
	dimension = 7;
	root_process = 0;
	
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
	if (world_rank == root_process) {
		double *square_array = initialise_square_array(dimension);
		
		print_square_array(dimension, square_array); 
		printf("\n");
		
		int id;
		for (id = 1; id < world_size; id++) {
			rows = get_sub_array_rows(dimension - 2, world_size - 1, id);
			num_sub_arr_rows = rows.end - rows.start + 1;
			
			if (DEBUG) printf("\n start row is %d and end row is %d\n", rows.start, rows.end);
			
			// send portion of square array to children processes
			sub_arr = select_chunk(dimension, square_array, rows.start, rows.end);
			num_elements_to_send = dimension * num_sub_arr_rows;
			MPI_Send(&num_elements_to_send, 1, MPI_INT, id, send_tag, MPI_COMM_WORLD);
			MPI_Send(sub_arr, num_elements_to_send, MPI_DOUBLE, id, send_tag, MPI_COMM_WORLD);
			
			// receive updated chunks from children processes
			MPI_Recv(sub_arr, num_elements_to_send, MPI_DOUBLE, id, recv_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			// place updated sub array back in square array
			square_array = stitch_array(square_array, sub_arr, rows.start, rows.end, dimension);
		}
		
		// finish and print final array
		print_square_array(dimension, square_array);
	}
	
	// Children processes
	else {
		// get the number of elements in portion of array to receive
		rows = get_sub_array_rows(dimension - 2, world_size - 1, world_rank);
		num_sub_arr_rows = rows.end - rows.start + 1;
		MPI_Recv(&num_elements_to_recv, 1, MPI_INT, root_process, send_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// receive portion of array to perform relaxation on
		sub_arr = malloc((long unsigned int)num_elements_to_recv * sizeof(double));
		MPI_Recv(sub_arr, num_elements_to_recv, MPI_DOUBLE, root_process, send_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if (DEBUG) {
			print_non_square_array(dimension, num_sub_arr_rows, sub_arr);
			printf("Hello world from processor #%d out of %d processors\n", world_rank, world_size);
		}
		
		// todo - perform relaxation on chunk here (instead of increment by 10)
		int i, j;
		for (i = 0; i < num_sub_arr_rows; i++) {
			for (j = 0; j < num_elements_to_recv/num_sub_arr_rows; j++) {
				sub_arr[i* (num_elements_to_recv/num_sub_arr_rows) + j] = sub_arr[i * (num_elements_to_recv/num_sub_arr_rows) + j] + 10;
			}
		}
		
		// send chunk back to root process
		MPI_Send(sub_arr, num_elements_to_recv, MPI_DOUBLE, root_process, recv_tag, MPI_COMM_WORLD);
	}
	
	// Clean up the MPI environment.
    MPI_Finalize();
}