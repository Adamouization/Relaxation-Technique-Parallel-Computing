/**
 * CM30225 Parallel Computing
 * Assessment Coursework 2
 * 
 * Header file for functions used to manipulate arrays.
 * 
 * Author: Adam Jaamour
 * Date: 07-Jan-2019
 */
 
 
 void check_double_malloc(double* square_array);
 
 
 double* initialise_square_array(int dim);
 
 
 double* create_new_array(int num_elements);
  
 
 double* stitch_array(double* sq_array, double* chunk_array, int start, int end, int dimension);