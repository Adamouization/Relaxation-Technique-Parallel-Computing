/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Header file for functions used to print data on the command line.
 * 
 * Author: Adam Jaamour
 * Date: 07-Jan-2019
 */

 
 void print_parameters(int dimension, int num_processes, double precision);
 
 
 void print_square_array(int dimension, double* sq_array);
 
 
 void print_non_square_array(int dim_x, int dim_y, double* arr);
 
 
 void print_relaxation_values_data(double old, double l, double r, double u, double d, double new, int count);