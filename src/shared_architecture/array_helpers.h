/**
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * 
 * Header file for functions used to manipulate arrays.
 * 
 * Author: Adam Jaamour
 * Date: 19-Nov-2018
 */

double** initialise_square_array(int dim);


pthread_mutex_t** initialise_mutex_array(int dim);

 
void check_double_malloc(double** square_array);


void check_mutex_malloc(pthread_mutex_t** mutex_array);