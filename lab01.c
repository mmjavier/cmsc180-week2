/*
@author: Myko Jefferson M. Javier
@date: January 27, 2026
@section: CMSC 180 - CD3L
@code-desc: Implementing Min-Max Transformation on an nxn matrix in C
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

//Function Prototypes
void mmt(double ** mat, int n,  int m);

//Helper Functions
double min (double **mat, int size, int col);
double max (double **mat, int size, int col);
double generate_random(int max);
void print_matrix(double **matrix, int n);
double ** generate_matrix(int row, int col);
int64_t timestamp_now (void);
double timestamp_to_seconds (int64_t timestamp);

//Main Function
int main(int argc, char **argv){
    //Variables
    int n;
    double **matrix;

    //Check if a file is inputed from the exec call
    if (argc < 2){
        //Ask user for input
        printf("Enter dimension (n): ");
        scanf("%d", &n);

        //Initialize the nxn matrix
        matrix = generate_matrix(n, n);

        //Create a matrix of size N with generate_random values from 1-100
        srand(time(NULL)); //Seed Time

        //Loop for generating Random values for the nxn matrix
        for (int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                matrix[i][j] = generate_random(100);
            }
        }
    }else{
        FILE *fptr = fopen(argv[1], "r");

        //First line is dimension
        fscanf(fptr, "%d", &n);

        //Allocate Matrix of size n
        matrix = generate_matrix(n,n);

        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                fscanf(fptr, "%lf", &matrix[i][j]);
            }
        }
    }

    //Log time start
    int64_t start = timestamp_now ();
    //Compute for the MMT
    mmt(matrix, n, n);
    int64_t end = timestamp_now ();
    printf("Elapsed Time:\n%lf", timestamp_to_seconds(end - start));
}

//Function for generating a generate_random number seeded on current time
double generate_random(int max){
    return (rand() % max) + 1;
}

//Function for printing a matrix
void print_matrix(double **matrix, int n){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            printf(" %0.2g\t",  matrix[i][j]);
        }
        printf("\n");
    }
}

//Function for getting the max from a matrix (to be improved)
double max(double **mat, int size, int col){
    double max = mat[0][col];
    for (int i = 1; i < size; i++){
        max = (mat[i][col] >= max) ? mat[i][col] : max;
    }

    return max;
}

//Function for generating a matrix given dimensions
double ** generate_matrix(int row, int col){
    double **temp = (double**)malloc(sizeof(double*)*row);

    for(int i = 0; i < row; i++){
        temp[i] = (double *)(malloc(sizeof(double) * col));
    }

    return temp;
}
//Function for getting the min from the matrix (to be improved)
double min(double **mat, int size, int col){
    double min = mat[0][col];
    for (int i = 1; i < size; i++){
        min = (mat[i][col] < min) ? mat[i][col] : min;
    }

    return min;
}

//Function for computing the MMT (new mat)
void mmt(double ** matrix, int n, int m){

    double colMax, colMin; 

    for (int i=0; i < n; i++){
        colMax = max(matrix, n, i);
        colMin = min(matrix, n, i);

        for (int j = 0; j < n; j++){
            matrix[j][i] = (matrix[j][i] - colMin) / (colMax - colMin);
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



