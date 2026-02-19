/*
@author: Myko Jefferson M. Javier
@date: January 27, 2026
@section: CMSC 180 - CD3L
@code-desc: Implementing Min-Max Transformation on an nxn matrix in C using Threads
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

//Thread data structure
typedef struct {
    double *matrix;     // Pointer to the full matrix
    int n;              // Matrix dimension (n x n)
    int start_col;      // Starting column for this thread
    int num_cols;       // Number of columns to process (n/t)
} args;

//Function Prototypes
void mmt(double *mat, int n, int start_col, int num_cols);
void *thread_mmt(void *arg);

//Helper Functions
double min (double *mat, int n, int col);
double max (double *mat, int n, int col);
double generate_random(int max);
void print_matrix(double *matrix, int n);
double *generate_matrix(int row, int col);
int64_t timestamp_now (void);
double timestamp_to_seconds (int64_t timestamp);
void transform_matrix(double *mat, int n);

//Main Function
int main(int argc, char **argv){
    //Variables
    int n, t;
    double *matrix;

    //Check if a file is inputed from the exec call
    if (argc < 2){
        //Ask user for input
        printf("Enter dimension (n): ");
        scanf("%d", &n);
        printf("Enter thread count (t): ");
        scanf("%d", &t);

        //Initialize the nxn matrix
        matrix = generate_matrix(n, n);

        //Create a matrix of size N with generate_random values from 1-100
        srand(time(NULL)); //Seed Time

        //Loop for generating Random values for the nxn matrix
        for (int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                matrix[(size_t)i * n + j] = generate_random(100);
            }
        }
    }else{
        FILE *fptr = fopen(argv[1], "r");

        //First line is dimension, second is thread count
        fscanf(fptr, "%d", &n);
        fscanf(fptr, "%d", &t);

        //Allocate Matrix of size n
        matrix = generate_matrix(n,n);

        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                fscanf(fptr, "%lf", &matrix[(size_t)i * n + j]);
            }
        }
        
        fclose(fptr);
    }

    //Create thread data structures
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * t);
    args *arguments = (args *)malloc(sizeof(args) * t);
    
    if (threads == NULL || arguments == NULL) {
        printf("Error: Failed to allocate memory for thread structures\n");
        free(matrix);
        return 0;
    }
    
    //Calculate columns per thread (divide matrix column-wise into t submatrices)
    int cols_per_thread = n / t;
    int remaining_cols = n % t;
    
    //Log time start
    int64_t start = timestamp_now();
    
    //Create t threads, each processes n/t columns of the full matrix
    int current_col = 0;
    for (int i = 0; i < t; i++){
        int thread_cols = cols_per_thread + (i < remaining_cols ? 1 : 0);
        
        arguments[i].matrix = matrix;
        arguments[i].n = n;
        arguments[i].start_col = current_col;
        arguments[i].num_cols = thread_cols;  // n/t columns
        
        pthread_create(&threads[i], NULL, thread_mmt, &arguments[i]);
        
        current_col += thread_cols;
    }
    
    //Wait for all threads to complete
    for (int i = 0; i < t; i++){
        pthread_join(threads[i], NULL);
    }
    
    int64_t end = timestamp_now();
    printf("Elapsed Time:\n%lf\n", timestamp_to_seconds(end - start));

    print_matrix(matrix, n);
    //Cleanup
    free(threads);
    free(arguments);
    free(matrix);
    
    return 0;
}

//Function for transposing the matrix
void transpose_matrix(double *matrix, int n){
    double temp;
    for (int i = 0; i < n; i++){
        for (int j = i + 1; j < n; j++){
            
            //Calculate 1d indices
            int index1 = i * n + j;
            int index2 = j * n + i;

            //Swap
            temp = matrix[index1];
            matrix[index1] = matrix[index2];
            matrix[index2] = temp;
        }
    }
}

//Thread function - processes n/t columns of the matrix
void *thread_mmt(void *arg){
    args *data = (args *)arg;
    
    // Call mmt on this thread's column range: mmt for columns [start_col, start_col + num_cols)
    mmt(data->matrix, data->n, data->start_col, data->num_cols);
    
    return NULL;
}

//Function for generating a generate_random number seeded on current time
double generate_random(int max){
    return (rand() % max) + 1;
}

//Function for printing a matrix
void print_matrix(double *matrix, int n){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            printf(" %0.2g\t",  matrix[(size_t)i * n + j]);
        }
        printf("\n");
    }
}

//Function for getting the max from a column
double max(double *mat, int n, int col){
    double max = mat[col];
    for (int i = 1; i < n; i++){
        double val = mat[(size_t)i * n + col];
        max = (val >= max) ? val : max;
    }

    return max;
}

//Function for generating a matrix given dimensions
double *generate_matrix(int row, int col){
    int total_size = row * col * sizeof(double);
    double *temp = (double*)malloc(total_size);
    
    if (temp == NULL) {
        printf("Error: Failed to allocate memory for matrix");
        return NULL;
    }
    
    return temp;
}

//Function for getting the min from a column
double min(double *mat, int n, int col){
    double min = mat[col];
    for (int i = 1; i < n; i++){
        double val = mat[i * n + col];
        min = (val < min) ? val : min;
    }
    return min;
}

//Function for computing the MMT on specific columns of the full matrix
// mat: pointer to full matrix, n: matrix dimension, start_col: first column, num_cols: number of columns
void mmt(double *mat, int n, int start_col, int num_cols){

    double colMax, colMin; 

    // Process each column in the range [start_col, start_col + num_cols)
    for (int i = start_col; i < start_col + num_cols; i++){
        colMax = max(mat, n, i);
        colMin = min(mat, n, i);

        // Apply min-max normalization to this column
        for (int j = 0; j < n; j++){
            int idx = j * n + i;
            mat[idx] = (mat[idx] - colMin) / (colMax - colMin);
        }
    }

}


//Helper functions for elapsed time sourced from stackoverflow (for higher resolution)
 int64_t timestamp_now (void)
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (int64_t) tv.tv_sec * CLOCKS_PER_SEC + tv.tv_usec;
}

double timestamp_to_seconds (int64_t timestamp)
{
    return timestamp / (double) CLOCKS_PER_SEC;
}



