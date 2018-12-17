/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Relaxation technique for solving differential equations in a distributed 
 * memory architecture using MPI (Message Passing Interface)
 *
 * Local usage: 
 * 1) "mpicc -Wall -Wextra -Wconversion main.c -o distributed_relaxation -lm"
 * 2) "mpirun -np <num_processes> ./distributed_relaxation -d <dimension> -p <precision> -debug <debug mode>"
 * example: mpirun -np 4 ./distributed_relaxation -d 15 -p 0.01 -debug 1
 * 
 * Author: Adam Jaamour
 * Date: 07-Jan-2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include "array_helpers.h"
#include "print_helpers.h"

#define send_tag 1
#define recv_tag 2

int DEBUG;
struct sub_arr_rows {
	int start;
	int end;
};


int main(int argc, char** argv) {
	int dimension, num_elements_to_send, num_elements_to_recv, num_sub_arr_rows, 
		num_extra_rows, extra_rows_to_send, extra_rows_to_recv;
	int root_process, world_size, world_rank, rc;
	double* sub_arr; // chunk of main square array with specific rows only
	struct sub_arr_rows rows;
	double precision;
	
	// Default values
	DEBUG = 1;
	dimension = 100;
	precision = 0.01f;
	
	// Read and Parse command line input
	int arg;
	for (arg = 1; arg < argc; arg++) {
		// parse square array dimension
		if (strcmp(argv[arg], "-d") == 0) {
			if (arg + 1 <= argc - 1) { // ensure there are more arguments
				if (atoi(argv[arg+1]) > 0) {
					arg++;
					dimension = atoi(argv[arg]);
				} else {
					fprintf(stderr, "WARNING: Invalid argument for -d. Must be a positive integer. Using dimension = %d as default value.\n", dimension);
				}
			}
		} 
		// parse square array values' floating precision
		else if (strcmp(argv[arg], "-p") == 0) {
			if (arg + 1 <= argc - 1) {
				if (atof(argv[arg+1]) > 0.0) {
					arg++;
					precision = atof(argv[arg]);
				} else {
					fprintf(stderr, "WARNING: Invalid argument for -p. Must be a positive float. Using precision = %f as default value.\n", precision);
				}
			}
		}
		// parse debug code value
		else if (strcmp(argv[arg], "-debug") == 0) {
			if (arg + 1 <= argc - 1) { /* Make sure we have more arguments */
				if (atoi(argv[arg+1]) >= 0) {
					arg++;
					DEBUG = atoi(argv[arg]);
				} else {
					fprintf(stderr, "WARNING: Invalid argument for -debug. Must be a positive integer. Using debug mode = %d as default value.\n", DEBUG);
				}
			}
		}
	}
	
	// Initialize the MPI environment.
    rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS) {
		printf ("Error starting MPI program\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	
	// Get the number of processes and the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	root_process = 0;
	
	// Kill program if less than 2 processes
	if (world_size < 2) {
		fprintf(stderr, "World size must be greater than 2 for %s\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	
	// Don't use more processes than there are rows to process in the array
	if (world_size > dimension - 2) {
		world_size = dimension - 2;
	}
	
	// Calculate the number of extra rows to assign to individual processes
	num_extra_rows = (dimension - 2) - (world_size - 1);
	
	// Executed by root process only
	if (world_rank == root_process) {		
		print_parameters(dimension, world_size, precision);
		double *square_array = initialise_square_array(dimension);
		
		if (DEBUG >= 1) {
			print_square_array(dimension, square_array); 
			printf("\n");
		}
		
		int id;
		for (id = 1; id < world_size; id++) {
			
			// determine which rows each process gets
			extra_rows_to_send = 0;
			if (num_extra_rows > 0) {
				extra_rows_to_send = 1;
				num_extra_rows--;
			}
			MPI_Send(&extra_rows_to_send, 1, MPI_INT, id, send_tag, MPI_COMM_WORLD);
			rows = get_sub_array_rows(dimension, world_size - 1, id, extra_rows_to_send);
			num_sub_arr_rows = rows.end - rows.start + 1;
			
			if (DEBUG >= 2) printf("\nID: %d start row is %d and end row is %d\n", id, rows.start, rows.end);
			
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
		printf("Relaxation completed\n");
		if (DEBUG >= 1) {
			print_square_array(dimension, square_array);
		}
	}
	
	// Executed by all children processes
	else {
		// get the number of extra rows assigned to this process
		MPI_Recv(&extra_rows_to_recv, 1, MPI_INT, root_process, send_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if (DEBUG >= 2) printf("ID: %d extra_rows_to_recv %d\n\n", world_rank, extra_rows_to_recv);
		
		// get the number of elements in portion of array to receive
		rows = get_sub_array_rows(dimension, world_size - 1, world_rank, extra_rows_to_recv);
		num_sub_arr_rows = rows.end - rows.start + 1;
		MPI_Recv(&num_elements_to_recv, 1, MPI_INT, root_process, send_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// receive portion of array to perform relaxation on
		sub_arr = malloc((long unsigned int)num_elements_to_recv * sizeof(double));
		MPI_Recv(sub_arr, num_elements_to_recv, MPI_DOUBLE, root_process, send_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		if (DEBUG >= 3) {
			print_non_square_array(dimension, num_sub_arr_rows, sub_arr);
			printf("Hello world from processor #%d out of %d processors\n\n", world_rank, world_size);
		}
		
		// todo - perform relaxation on chunk here (instead of increment by 10)
		int i, j;
		for (i = 0; i < num_sub_arr_rows; i++) {
			for (j = 0; j < num_elements_to_recv/num_sub_arr_rows; j++) {
				sub_arr[i* (num_elements_to_recv/num_sub_arr_rows) + j] = 9.9999990;
			}
		}
		
		// send chunk back to root process
		MPI_Send(sub_arr, num_elements_to_recv, MPI_DOUBLE, root_process, recv_tag, MPI_COMM_WORLD);
	}
	
	// Clean up the MPI environment.
    MPI_Finalize();
}