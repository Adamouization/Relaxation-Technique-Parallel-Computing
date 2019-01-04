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
 * example: "mpirun -quiet -np 4 ./distributed_relaxation -d 15 -p 0.1 -debug 1"
 * 
 * Author: Adam Jaamour
 * Date: 07-Jan-2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>
#include "array_helpers.h"
#include "print_helpers.h"

#define send_tag 1001
#define recv_tag 1002

int DEBUG;
struct sub_arr_rows {
	int start;
	int end;
};


int main(int argc, char** argv) {

	// Variables
	int dimension, num_elements_to_send, num_elements_to_recv, num_sub_arr_rows, 
		num_extra_rows, extra_rows_to_send, extra_rows_to_recv, iteration_counter,
		row_length, finished_children;
	int id, f, i, j;
	int precision_counter = 0;
	int is_above_precision = 1;
	int continue_relaxation = 1;
	double precision, difference;
	struct sub_arr_rows rows;
	double *square_array;
	double *old_values_array;
	double *sub_arr;
	int *flags_array;
	int root_process, world_size, world_rank, rc;
	double start_MPI, end_MPI, elapsed_time;
	MPI_Status status;
	MPI_Request request;
	
	// Default values (if no command line arguments are correctly passed)
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
	
	// Start recording time
	start_MPI = MPI_Wtime();
	
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
	
	// Executed by root process only
	if (world_rank == root_process) {
		// initialise array values
		square_array = initialise_square_array(dimension);
		old_values_array = initialise_square_array(dimension);

		// print initial parameters for log information
		print_parameters(dimension, world_size, precision);

		// array of flags used to keep track of which child process was told to finish
		flags_array = malloc((long unsigned int)(world_size-1) * sizeof(int));
		for (f = 0; f < world_size - 1; f++) {
			flags_array[f] = 0;
		}

		if (DEBUG >= 1) {
			print_square_array(dimension, square_array); 
			printf("\n");
		}
		
		while(is_above_precision) {

			// Calculate the number of extra rows to assign to individual processes
			num_extra_rows = (dimension - 2) - (world_size - 1);

			// Manage each child process
			for (id = 1; id < world_size; id++) {

				// determine the number of extra rows (if any) to send to each child process
				extra_rows_to_send = 0;
				if (num_extra_rows > 0) {
					extra_rows_to_send = 1;
					num_extra_rows--;
				}
				MPI_Send(&extra_rows_to_send, 1, MPI_INT, id, send_tag, MPI_COMM_WORLD);
				
				// determine which rows each child process gets
				rows = get_sub_array_rows(dimension, world_size - 1, id, extra_rows_to_send);
				num_sub_arr_rows = rows.end - rows.start + 1;
				if (DEBUG >= 2) printf("\nID: %d start row is %d and end row is %d\n", id, rows.start, rows.end);
				
				// send chunk of square array to children processes
				sub_arr = select_chunk(dimension, square_array, rows.start, rows.end);
				num_elements_to_send = dimension * num_sub_arr_rows;
				MPI_Isend(&num_elements_to_send, 1, MPI_INT, id, send_tag, MPI_COMM_WORLD, &request);
				MPI_Isend(sub_arr, num_elements_to_send, MPI_DOUBLE, id, send_tag, MPI_COMM_WORLD, &request);

				// receive updated chunks from children processes
				MPI_Recv(sub_arr, num_elements_to_send, MPI_DOUBLE, id, recv_tag, MPI_COMM_WORLD, &status);
				
				// place updated sub array back in square array
				old_values_array = stitch_array(old_values_array, sub_arr, rows.start, rows.end, dimension);

				// check if we went within precision
				MPI_Wait(&request, &status);
				for (i = 1; i < dimension - 1; i++) {
		 			for (j = 1; j < dimension - 1; j++) {
			 			difference = (double)fabs(old_values_array[i * dimension + j] - square_array[i * dimension + j]);
						if (difference < precision) { // check if difference is smaller than precision for a single value
							precision_counter++;
						} else {
							precision_counter = 0;
						}

						// if difference is smaller than precision for all values, thne we are within precision
						if (precision_counter >= ((dimension - 2) * (dimension - 2))) {
							flags_array[id-1] = 1;
						}
			 			if (DEBUG >= 3) printf("old value %f - new value %f = %f\n", square_array[i * dimension + j], old_values_array[i * dimension + j], difference);
		 			}
		 		}
			 	if (DEBUG >= 3) printf("From root process: is above precision: %d\n", is_above_precision);
		 		
				// check that all children finished 
		 		finished_children = 0;
		 		is_above_precision = 1;
		 		for (f = 0; f < world_size - 1; f++) {
					if (flags_array[f] == 1) {
						finished_children++;
					}
					if (DEBUG >= 3) printf("flags array for process id %d (index %d) = %d\n", id, f, flags_array[f]);
				}
				if (finished_children == world_size - 1) {
					is_above_precision = 0;
				}

				// tell children if need to continue or stop relaxation
				MPI_Isend(&is_above_precision, 1, MPI_INT, id, send_tag, MPI_COMM_WORLD, &request);

				// update values
				double *temp_array_values = square_array;
				square_array = old_values_array;
				old_values_array = temp_array_values;
			}
		}

		// finish and print final array
		printf("Relaxation successfully completed using %d processes (1 root process and %d children)\n", world_size, world_size-1);
		if (DEBUG >= 1) {
			print_square_array(dimension, square_array);
		}		
		
		// calculate parallel tasks runtime only
		end_MPI = MPI_Wtime();
		elapsed_time = end_MPI - start_MPI;
		printf("Total time = %f seconds\n\n", elapsed_time);
	}
	
	// Executed by all children processes
	else {
		while (continue_relaxation) {

			// get the number of extra rows assigned to this process
			MPI_Recv(&extra_rows_to_recv, 1, MPI_INT, root_process, send_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			if (DEBUG >= 2) printf("ID: %d extra_rows_to_recv %d\n\n", world_rank, extra_rows_to_recv);
			
			// get the number of elements in portion of array to receive
			rows = get_sub_array_rows(dimension, world_size - 1, world_rank, extra_rows_to_recv);
			num_sub_arr_rows = rows.end - rows.start + 1;
			MPI_Recv(&num_elements_to_recv, 1, MPI_INT, root_process, send_tag, MPI_COMM_WORLD, &status);
			
			// receive portion of array to perform relaxation on
			sub_arr = malloc((long unsigned int)num_elements_to_recv * sizeof(double));
			MPI_Recv(sub_arr, num_elements_to_recv, MPI_DOUBLE, root_process, send_tag, MPI_COMM_WORLD, &status);
					
			if (DEBUG >= 3) {
				print_non_square_array(dimension, num_sub_arr_rows, sub_arr);
				printf("Hello world from processor #%d out of %d child processes\n\n", world_rank, world_size - 1);
			}
			
			is_above_precision = 1;
			iteration_counter = 0;
			difference = 0.0;
			row_length = num_elements_to_recv / num_sub_arr_rows;
			
			// perform relaxation
			while(is_above_precision) {
				precision_counter = 0;
				for (i = 1; i < num_sub_arr_rows - 1; i++) {
					for (j = 1; j < row_length - 1; j++) {
						// value to replace
						double old_value = sub_arr[i * row_length + j]; 

						// get 4 surrounding values needed to average
						double v_left = sub_arr[i * row_length + j-1];
						double v_right = sub_arr[i * row_length + j+1];
						double v_up = sub_arr[(i-1) * row_length + j];
						double v_down = sub_arr[(i+1) * row_length + j];

						// perform the calculation
						double new_value = (v_left + v_right + v_up + v_down) / 4;
						
						// replace the old value with the new one
						sub_arr[i * row_length + j] = new_value;
						
						// check if difference is smaller than precision
						difference = (double)fabs(old_value - new_value);
						if (difference < precision) {
							precision_counter++;
						} else { // reset precision counter if diff smaller than prec
							precision_counter = 0;
						}

						// if difference smaller than precision for all values to relax, stop
						if (precision_counter >= ((num_sub_arr_rows - 2) * (row_length - 2))) {
							is_above_precision = 0;
						}
						
						// print current iteration data
						if (DEBUG >= 3 && iteration_counter == -1) {
							printf("\n");
							print_non_square_array(row_length, num_sub_arr_rows, sub_arr);
							printf("\n");
							print_relaxation_values_data(old_value, v_left, v_right, v_up, v_down, new_value, iteration_counter);
						}
					}
					if (!(is_above_precision)) { // check at end of current array iteration
						break;
					}
				}
				iteration_counter++;
			}
			
			// send chunk back to root process
			MPI_Send(sub_arr, num_elements_to_recv, MPI_DOUBLE, root_process, recv_tag, MPI_COMM_WORLD);

			// wait for root process to tell us to continue relaxation or not
			MPI_Recv(&continue_relaxation, 1, MPI_INT, root_process, send_tag, MPI_COMM_WORLD, &status);
			if (DEBUG >= 3) printf("From child process %d - continue_relaxation: %d\n", world_rank, continue_relaxation);

		}
	}
	
	// exit program ungracefully (use "mpirun -quiet")
	exit(0);
	
	// Clean up the MPI environment.
    MPI_Finalize();
    return 0;
}