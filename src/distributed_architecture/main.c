/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Relaxation technique for solving differential equations in a distributed 
 * memory architecture using MPI (Message Passing Interface)
 *
 * Local usage: 
 * 1) "mpicc -Wall -Wextra -Wconversion main.c -o distributed_relaxation"
 * 2) "mpirun -np <num_processes> ./distributed_relaxation -d <dimension> -p <precision> -debug <debug mode>"
 * example: "mpirun -np 6 ./distributed_relaxation -d 15 -p 0.1 -debug 2"
 * 
 * Author: Adam Jaamour
 * Date: 07-Jan-2019
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <mpi.h>
#include "array_helpers.h"
#include "print_helpers.h"

#define SEND_TAG 1001
#define RECV_TAG 1002

int DEBUG;
struct sub_arr_rows {
	// structure used for children processes to keep track of which part of the array they have
    int start;
	int end;
	int num_elements;
};


int main(int argc, char *argv[]) {
	
	// Variables
	int dimension, num_elements_to_send, num_elements_to_receive, 
		num_sub_arr_elements, average_height, extra_rows, initial_end_row, 
		extra_rows_counter, height, num_children_processes, iteration_count, 
		withinPrecision, num_sub_arr_rows, prev_child_id, next_child_id;
	int id, rc, world_rank, root_process_id, world_size, start_row, end_row;
	int i, j;
	double *square_array;
	double *sub_arr;
	double *temp_arr;
	double *temp_sub_arr;
	double *new_sub_arr;
	double precision, difference;
	double v_left, v_right, v_up, v_down, new_value;
	bool first_iteration;
	MPI_Status status;
	MPI_Request request;
	double start_MPI, end_MPI, elapsed_time;

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
				if (atoi(argv[arg + 1]) > 0) {
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
				if (atof(argv[arg + 1]) > 0.0) {
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
				if (atoi(argv[arg + 1]) >= 0) {
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

	// Start recording time after initialisation overhead and command line parsing is finished
	start_MPI = MPI_Wtime();

	// Get the number of processes and the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	root_process_id = 0;

    // Kill program if less than 2 processes
	if (world_size < 2) {
		fprintf(stderr, "World size must be greater than 2 for %s\n", argv[0]);
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	
	// Don't use more processes than there are num_sub_arr_rows to process in the array
	if (world_size > dimension - 2) {
		world_size = dimension - 2;
	}
	
	// Calculating variables with constant values
	num_children_processes = world_size - 1;
	average_height = (int)floor((dimension - 2) / num_children_processes) + 2;
	extra_rows = (dimension - 2) % num_children_processes;
	initial_end_row = average_height - 1; // Set end row of master thread, decrement for index (row 1 -> index 0)
	

	// Executed by root process only
    if (world_rank == root_process_id) {
		
		first_iteration = true;
		
		// initialise array values
		square_array = initialise_square_array(dimension);
		
		// print initial parameters for log information
		print_parameters(dimension, world_size, precision);
		if (DEBUG >= 0) {
			print_square_array(dimension, square_array); 
			printf("\n");
		}

		iteration_count = 0;
		withinPrecision = 0;

		// array used to keep track of the start row, end row and total number 
		// of elements for each of the children's sub array
		struct sub_arr_rows rows_arr[num_children_processes]; 

		while (!withinPrecision) {
			
			withinPrecision = 1;
			iteration_count++;
			
			// send a portion of the array to each child process only once
			if (first_iteration) {
				
				extra_rows_counter = extra_rows;
				end_row = initial_end_row;

				// determine portions of the original square array to send to children processes and send them
				for (id = 1; id < world_size; id++) {
					
					// determine the number of extra num_sub_arr_rows (if any) to send to each child process
					height = average_height;
					if (extra_rows_counter > 0) {
						height++;
						extra_rows_counter--;
					}
					
					// assign previous process end row as first editable row and pass row before for relaxation
					start_row = end_row - 1;
					// first child process starts with 0
					if (id == 1) start_row = 0;
					// assign end row using the row index (minus 1)
					end_row = start_row + (height - 1);
					
					if (DEBUG >= 2) printf("\nProcess ID %d: start row = %d - end row = %d\n", id, start_row, end_row);
					
					// tell children processes how many elements from the original array they will receive in a non-blocking send
					num_elements_to_send = dimension * height;
					MPI_Isend(&num_elements_to_send, 1, MPI_INT, id, SEND_TAG, MPI_COMM_WORLD, &request);
					
					// Store this information in our struct for later use when we receive the array back
					rows_arr[id].start = start_row;
					rows_arr[id].end = end_row;
					rows_arr[id].num_elements = num_elements_to_send;
										
					// send the sub array to the children processes in a non-blocking send
					MPI_Isend(&square_array[rows_arr[id].start * dimension], num_elements_to_send, MPI_DOUBLE, id, SEND_TAG, MPI_COMM_WORLD, &request);
				}
				first_iteration = false;
			} 

			// wait for children processes to have received their sub arrays
			MPI_Wait(&request, &status);

			// check with each process to see if they went out of precision on this iteration
			int returnedWP = 0;
			for (id = 1; id < world_size; id++) {
				MPI_Recv(&returnedWP, 1, MPI_INT, id, RECV_TAG, MPI_COMM_WORLD, &status);
				if (!returnedWP && withinPrecision) {
					withinPrecision = 0;
				}
			}

			// tell children processes to continue or to stop relaxing their portion of the array
			for (id = 1; id < world_size; id++) {
				MPI_Isend(&withinPrecision, 1, MPI_INT, id, SEND_TAG, MPI_COMM_WORLD, &request);
			}
			MPI_Wait(&request, &status);
		}

		// Relaxation is finished for all children processes
		// Start stitching the sub arrays back into the final relaxed square array
		for (id = 1; id < world_size; id++) {
			// get back the start row, end row and number of elements to receive that 
			// were previously calculated and stored in the array
			start_row = rows_arr[id].start;
			end_row = rows_arr[id].end;
			num_elements_to_receive = rows_arr[id].num_elements;

			// temporarily store the received values from the children processes 
			// in temp_arr before merging with the main square array
			temp_arr = create_new_array(num_elements_to_receive);

			// receive the relaxed sub array and store it into a temporary array before merging
			MPI_Recv(temp_arr, num_elements_to_receive, MPI_DOUBLE, id, RECV_TAG, MPI_COMM_WORLD, &status);

			// calculate number of rows received by the child process
			num_sub_arr_rows = end_row + 1 - start_row;

			// stitch back the received sub array to the main square array
			square_array = stitch_array(square_array, temp_arr, start_row, end_row, dimension);
			
			// free up allocated space used for the temporary array
			free(temp_arr);
		}
				
		// calculate elapsed time for parallel tasks only
		end_MPI = MPI_Wtime();
		elapsed_time = end_MPI - start_MPI;
		printf("Relaxation successfully completed in %d iterations using %d processes (1 root process and %d children)\n", iteration_count, world_size, num_children_processes);
		printf("Total time = %f seconds\n\n", elapsed_time);

		// free up allocated space used for sub arrays and wait for all children process to gracefully finish
		free(square_array);
		MPI_Wait(&request, &status);
	} 
	
	// Executed by all children processes
	else {

		// initialise children processes variables
		withinPrecision = 0;
		first_iteration = true;
		num_sub_arr_elements = 0;

		// get the number of elements from the main array to receive
		MPI_Recv(&num_sub_arr_elements, 1, MPI_INT, root_process_id, SEND_TAG, MPI_COMM_WORLD, &status);
		
		// create 2 arrays to receive the current values and store the new values while relaxing
		sub_arr = create_new_array(num_sub_arr_elements);
		new_sub_arr = create_new_array(num_sub_arr_elements);
		
		// determine the number of rows the sub array will have
		num_sub_arr_rows = num_sub_arr_elements / dimension;

		while (!withinPrecision) {
			
			// first iteration where the entire sub array is received from the root process
			if (first_iteration) {
				
				// receive the entire portion of th array that will need to be relaxed in this process
		        MPI_Recv(sub_arr, num_sub_arr_elements, MPI_DOUBLE, root_process_id, SEND_TAG, MPI_COMM_WORLD, &status);
		        
				// copy the values in the new values array
		        for (i = 0; i < num_sub_arr_rows; i++) {
					for (j = 0; j < dimension; j++) {
						new_sub_arr[i * dimension + j] = sub_arr[i * dimension + j];
					}
				}
				first_iteration = false;
			} 
			
			// future iterations do no send/receive the entire portion of tha array to relax again as it causes unnecessary communication overhead 
			// from now on, send and receive the first and last rows of the sub array this process is working on
			else {
				prev_child_id = world_rank - 1;
				next_child_id = world_rank + 1;
				
				// send first row to the previous child process (unless if this is the first child process)
				if (world_rank != 1) {
					MPI_Isend(&sub_arr[dimension], dimension, MPI_DOUBLE, prev_child_id, SEND_TAG, MPI_COMM_WORLD, &request);
				}
				
				//  send last row to the next child process (unless if this is the last child process)
				if (world_rank != num_children_processes) {
					MPI_Isend(&sub_arr[(num_sub_arr_rows - 2) * dimension], dimension, MPI_DOUBLE, next_child_id, SEND_TAG, MPI_COMM_WORLD, &request);
				}

				// receive the first row from the previous child process (unless this is the first child process)
				if (world_rank != 1) {
					MPI_Recv(&sub_arr[0], dimension, MPI_DOUBLE, prev_child_id, SEND_TAG, MPI_COMM_WORLD, &status);
				}

				// receive the last row from the next child process (unless this is the last child process)
				if (world_rank != num_children_processes) {
					MPI_Recv(&sub_arr[(num_sub_arr_rows-1)*dimension], dimension, MPI_DOUBLE, next_child_id, SEND_TAG, MPI_COMM_WORLD, &status);
				}
			}

			// perform relaxation on assigned portion of the array
			withinPrecision = 1;
			for (i = 1; i < num_sub_arr_rows - 1; i++) {
				for (j = 1; j < dimension - 1; j++) {
					// get 4 surrounding values needed to average
					v_left = sub_arr[i * dimension + (j-1)];
					v_right = sub_arr[i * dimension + (j+1)];
					v_up = sub_arr[(i-1) * dimension + j];
					v_down = sub_arr[(i+1) * dimension + j];
					
					// perform the calculation
					new_value = (v_left + v_right + v_up + v_down) / 4.0;
					
					// replace the old value with the new one
					new_sub_arr[i * dimension + j] = new_value;
					
					// check if process went within the precision needed to stop relaxation
					difference = (double)fabs(sub_arr[i * dimension + j] - new_value);
					if (difference > precision && withinPrecision == 1) {
						withinPrecision = 0; // need to iterate again
					}
				}
			}

			// tell root process if this process relaxed its portion of the array within the precision
			MPI_Send(&withinPrecision, 1, MPI_INT, root_process_id, RECV_TAG, MPI_COMM_WORLD);

			// wait for root process to tell this process to stop or continue relaxaing sub array
			MPI_Recv(&withinPrecision, 1, MPI_INT, root_process_id, SEND_TAG, MPI_COMM_WORLD, &status);

			// update arrays for next iteration
			temp_sub_arr = sub_arr;
			sub_arr = new_sub_arr;
			new_sub_arr = temp_sub_arr;
		}
		
		// child process is finished and sends back its relaxed portion of the 
		// array to the root process that will stitch it back with the final array
		MPI_Send(sub_arr, num_sub_arr_elements, MPI_DOUBLE, root_process_id, RECV_TAG, MPI_COMM_WORLD);
		
		// free up allocated space used for sub arrays
		free(sub_arr);
		free(new_sub_arr);
	}
	
	// Clean up the MPI environment.
	MPI_Finalize();
	return 0;
}