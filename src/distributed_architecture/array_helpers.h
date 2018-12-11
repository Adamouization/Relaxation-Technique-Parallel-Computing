/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Header file for functions used to manipulate arrays.
 * 
 * Author: Adam Jaamour
 * Date: 30-Nov-2018
 */
 
 
 void check_double_malloc(double* square_array);
 
 
 double* initialise_square_array(int dim);
 
 
 double* create_new_array(int dim_x, int dim_y);
 
 
 double* stitch_array(double* sq_array, double* chunk_array, int start, int end, int dimension);
 
 
 double* select_chunk(int dimension, double* square_array, int start_row, int end_row);