#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define TO_ONE_DIM(x, y, n) ((x) * (n) + (y))

/***** Globals ******/
float *a; /* The coefficients */
float *x;  /* The unknowns */
float *b;  /* The constants */
float err; /* The absolute relative error */
int num = 0;  /* number of unknowns */

/****** Function declarations */
void check_matrix(); /* Check whether the matrix will converge */
void get_input();  /* Read input from file */

//distribute input across machines
void distribute_input(float** a_local, 
		float** x_local,
		float** b_local,
		int** proc_startid,
		int** proc_chunksize,
		const int comm_sz,
		const int my_rank);
/********************************/



/* Function definitions: functions are ordered alphabetically ****/
/*****************************************************************/

/* 
   Conditions for convergence (diagonal dominance):
   1. diagonal element >= sum of all other elements of the row
   2. At least one diagonal element > sum of all other elements of the row
 */
void check_matrix()
{
	int bigger = 0; /* Set to 1 if at least one diag element > sum  */
	int i, j;
	float sum = 0;
	float aii = 0;
  
	for(i = 0; i < num; i++)
	{
		sum = 0;
		aii = fabs(a[TO_ONE_DIM(i,i,num)]);
    
		for(j = 0; j < num; j++)
			if( j != i)
				sum += fabs(a[TO_ONE_DIM(i,j,n)]);
       
		if( aii < sum)
		{
			printf("The matrix will not converge.\n");
			exit(1);
		}
    
		if(aii > sum)
			bigger++;
    
	}
  
	if( !bigger )
	{
		printf("The matrix will not converge\n");
		exit(1);
	}
}

void distribute_input(float** a_local, 
		float** x_local,
		float** b_local,
		int** proc_startid,
		int** proc_chunksize,
		const int comm_sz,
		const int my_rank){
	
	//1. let all process know the number of unknows
	MPI_Bcase(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//2. calculate chunksizes
	//cheat sheet
	//If we have n item for k buckets:
	//(n - k * [n/k]) of buckets have ([n/k] + 1) item
	//k - (n - k * [n/k]) of buckets have [n/k] item
	*proc_startid = (int *) malloc(num, sizeof(int));
	*proc_chunksize = (int *) malloc(num, sizeof(int));
	int chunkSize = floor(1.0 * num / comm_sz);
	int numLarge = num - comm_sz * chunkSize;
	for(int i = 0; i < numLarge; ++i){
		(*proc_startid)[i] = (chunkSize + 1) * i;
		(*proc_chunksize)[i] = chunkSize + 1;
	}
	for(int i = numLarge; i < num; ++i){
		(*proc_startid)[i] = chunkSize * i + numLarge;
		(*proc_chunksize)[i] = chunkSize;
	}

	//3. create local inputs
	*a_local = (float *) malloc((*proc_chunksize)[my_rank] * num, sizeof(float));
	*b_local = (float *) malloc((*proc_chunksize)[my_rank], sizeof(float));
	*x_local = (float *) malloc(num, sizeof(float));

	//4. distribute a
	int a_chunksize[64];
	int a_startid[64];
	for(int i = 0; i < comm_sz; ++i){
		a_chunksize[i] = (*proc_chunksize)[i] * num;
		a_startid[i] = (*proc_startid)[i] * num;
	}
	MPI_Scatterv(a, a_chunksize, a_startid, MPI_FLOAT,
		   	*a_local, a_chunksize[my_rank], MPI_FLOAT, 0, MPI_COMM_WORLD);

	//5. distribute b
	MPI_Scatterv(b, *proc_chunksize, *proc_startid, MPI_FLOAT,
			*b_local, (*proc_chunksize)[my_rank], MPI_FLOAT, 0, MPI_COMM_WORLD);

	//6. distribute x
	MPI_Scatterv(x, *proc_chunksize, *proc_startid, MPI_FLOAT,
			*b_local, (*proc_chunksize)[my_rank], MPI_FLOAT, 0, MPI_COMM_WORLD);


}

/******************************************************/
/* Read input from file */
/* After this function returns:
 * a[][] will be filled with coefficients and you can access them using a[i][j] for element (i,j)
 * x[] will contain the initial values of x
 * b[] will contain the constants (i.e. the right-hand-side of the equations
 * num will have number of variables
 * err will have the absolute error that you need to reach
 */
void get_input(char filename[])
{
	FILE * fp;
	int i,j;  
 
	fp = fopen(filename, "r");
	if(!fp)
	{
		printf("Cannot open file %s\n", filename);
		exit(1);
	}

	fscanf(fp,"%d ",&num);
	fscanf(fp,"%f ",&err);

	/* Now, time to allocate the matrices and vectors */
	a = (float *) malloc(num * num * sizeof(float));
	if(!a)
	{
		printf("Cannot allocate a!\n");
		exit(1);
	}		
 
	x = (float *) malloc(num * sizeof(float));
	if( !x)
	{
		printf("Cannot allocate x!\n");
		exit(1);
	}


	b = (float *) malloc(num * sizeof(float));
	if( !b)
	{
		printf("Cannot allocate b!\n");
		exit(1);
	}

	/* Now .. Filling the blanks */ 

	/* The initial values of Xs */
	for(i = 0; i < num; i++)
		fscanf(fp,"%f ", &x[i]);
 
	for(i = 0; i < num; i++)
	{
		for(j = 0; j < num; j++)
			fscanf(fp,"%f ",&a[TO_ONE_DIM(x,y,n)]);
   
		/* reading the b element */
		fscanf(fp,"%f ",&b[i]);
	}
 
	fclose(fp); 
}


/************************************************************/


int main(int argc, char *argv[])
{
	int comm_sz;
	int my_rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	
	//int i;
	//int nit = 0; /* number of iterations */
	//char output[100] ="";
	
	//Process 0 read input
	if(my_rank == 0){
		if( argc != 2){
			printf("Usage: ./gsref filename\n");
			exit(1);
		}
  
		/* Read the input file and fill the global data structure above */ 
		get_input(argv[1]);
 
		/* Check for convergence condition */
		/* This function will exit the program if the coffeicient will never converge to 
		* * the needed absolute error. 
		* * This is not expected to happen for this programming assignment.
		* */
		check_matrix();
	}
 
 
	/* Writing results to file */
	FILE * fp;
	sprintf(output,"%d.sol",num);
	fp = fopen(output,"w");
	if(!fp){
		printf("Cannot create the file %s\n", output);
		exit(1);
	}
    
	for( i = 0; i < num; i++)
		fprintf(fp,"%f\n",x[i]);
 
	printf("total number of iterations: %d\n", nit);
 
	fclose(fp);
 
	exit(0);
}
