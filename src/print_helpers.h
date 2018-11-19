/**
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * 
 * Header file for functions used to print data on the command line.
 * 
 * Author: aj645
 * Date: 19-Nov-2018
 */
 

void print_parameters(int dimension, 
					  int num_thr, 
					  double precision, 
					  double** square_array);


void print_array(int dimension, 
				 double** square_array);


void print_relaxation_thread_data(int tid, 
								  int updates_counter,
								  int pos_i,
								  int pos_j);


void print_relaxation_values_data(double old, 
						   		  double l, 
						   		  double r, 
						   		  double u, 
						   		  double d, 
						   		  double new,
						   		  int prec_count);


void print_final_results(int dimension, 
						 int num_thr, 
						 double precision);
