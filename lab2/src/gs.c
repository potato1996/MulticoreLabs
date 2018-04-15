#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>
#include "gssol.h"
#include "Common.h"

/***** Globals ******/
data_float_t *a; /* The coefficients */
data_float_t *x;  /* The unknowns */
data_float_t *b;  /* The constants */
data_float_t err; /* The absolute relative error */
int num = 0;  /* number of unknowns */

/****** Function declarations */
void check_matrix(); /* Check whether the matrix will converge */
void get_input();  /* Read input from file */

int solve_gs(const int comm_sz,
		const int my_rank); /* solve the problem, return the number of iters */

//distribute input across machines
void distribute_input(data_float_t** a_local, 
		data_float_t** b_local,
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
	data_float_t sum = 0;
	data_float_t aii = 0;
  
	for(i = 0; i < num; i++)
	{
		sum = 0;
		aii = fabs(a[TO_ONE_DIM(i,i,num)]);
    
		for(j = 0; j < num; j++)
			if( j != i)
				sum += fabs(a[TO_ONE_DIM(i,j,num)]);
       
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

void distribute_input(data_float_t** a_local, 
		data_float_t** b_local,
		int** proc_startid,
		int** proc_chunksize,
		const int comm_sz,
		const int my_rank){
	
	//1. let all process know the number of unknows
	MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//2. calculate chunksizes
	//cheat sheet
	//If we have n item for k buckets:
	//(n - k * [n/k]) of buckets have ([n/k] + 1) item
	//k - (n - k * [n/k]) of buckets have [n/k] item
	*proc_startid = (int *) malloc(num * sizeof(int));
	*proc_chunksize = (int *) malloc(num * sizeof(int));
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
	*a_local = (data_float_t *) malloc((*proc_chunksize)[my_rank] * num * sizeof(data_float_t));
	*b_local = (data_float_t *) malloc((*proc_chunksize)[my_rank] * sizeof(data_float_t));

	//4. distribute a_local
	int a_chunksize[64];
	int a_startid[64];
	for(int i = 0; i < comm_sz; ++i){
		a_chunksize[i] = (*proc_chunksize)[i] * num;
		a_startid[i] = (*proc_startid)[i] * num;
	}
	MPI_Scatterv(a, a_chunksize, a_startid, MPI_DATA_FLOAT_T,
		   	*a_local, a_chunksize[my_rank], MPI_DATA_FLOAT_T, 0, MPI_COMM_WORLD);

	//5. distribute b_local
	MPI_Scatterv(b, *proc_chunksize, *proc_startid, MPI_DATA_FLOAT_T,
			*b_local, (*proc_chunksize)[my_rank], MPI_DATA_FLOAT_T, 0, MPI_COMM_WORLD);

	//6. distribute x
	if(my_rank != 0){
		x = (data_float_t*) malloc(num * sizeof(data_float_t));
	}
	MPI_Bcast(x, num, MPI_DATA_FLOAT_T, 0, MPI_COMM_WORLD);

	//7. let all procs know err
	MPI_Bcast(&err, 1, MPI_DATA_FLOAT_T, 0, MPI_COMM_WORLD);
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
inline int getNextInt(const char* buf, int pos, float* num){
	while(buf[pos] == ' ')pos++;
	char intbuf[11];
	int intLen = 0;
	while(buf[pos] != ' ' && buf[pos] != '\n'){
		intbuf[intLen] = buf[pos];
		pos++;
		intLen++;
	}
	intbuf[intLen] = '\0';
	*num =  atof(intbuf);
	return pos;
}
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
	fscanf(fp,FORMAT_STR" ",&err);

	/* Now, time to allocate the matrices and vectors */
	a = (data_float_t *) malloc(num * num * sizeof(data_float_t));
	if(!a)
	{
		printf("Cannot allocate a!\n");
		exit(1);
	}		
 
	x = (data_float_t *) malloc(num * sizeof(data_float_t));
	if( !x)
	{
		printf("Cannot allocate x!\n");
		exit(1);
	}


	b = (data_float_t *) malloc(num * sizeof(data_float_t));
	if( !b)
	{
		printf("Cannot allocate b!\n");
		exit(1);
	}

	/* Now .. Filling the blanks */ 

	/* The initial values of Xs */
	//for(i = 0; i < num; i++)
	//	fscanf(fp,FORMAT_STR" ", &x[i]);
 
	//for(i = 0; i < num; i++)
	//{
	//	for(j = 0; j < num; j++)
	//		fscanf(fp,FORMAT_STR" ",&a[TO_ONE_DIM(i,j,num)]);
   
		/* reading the b element */
	//	fscanf(fp,FORMAT_STR" ",&b[i]);
	//}
	//
	
	/* Read file line by line, much faster! */
	char* lineBuf = (char*) malloc(11 * num); 
	
	/* The inital values of Xs */
	fgets(lineBuf, 11 * num, fp);
	int linepos = 0;
	for(i = 0; i < num; i++){
		linepos = getNextInt(lineBuf, linepos, &x[i]);
	}
	
	for(i = 0; i < num; i++){
		fgets(lineBuf, 11 * num, fp);
		linepos = 0;
		for(j = 0; j < num; j++){
			linepos = getNextInt(lineBuf, linepos, &a[TO_ONE_DIM(i, j, num)]);
		}
		
		/* reading the b element */
		linepos = getNextInt(lineBuf, linepos, &b[i]);
	}
 
	free(lineBuf);
	fclose(fp); 
}

int solve_gs(const int comm_sz, const int my_rank){
	//local data for each proc
	data_float_t* a_local;
	data_float_t* b_local;
	int* proc_startid;
	int* proc_chunksize;

#ifdef ENABLE_TIMING
	double startTime, finishTime;
	if(my_rank == 0){
		startTime = MPI_Wtime();
	}
#endif

	//distribute data to each proc
	distribute_input(&a_local,
			&b_local,
			&proc_startid,
			&proc_chunksize,
			comm_sz,
			my_rank);

#ifdef ENABLE_TIMING
	if(my_rank == 0){
		finishTime = MPI_Wtime();
		printf("Input Distribution Time = %lf seconds\n", finishTime - startTime);
		startTime = MPI_Wtime();
	}
#endif


	data_float_t* x_new = (data_float_t*) malloc(num * sizeof(data_float_t));
	int iter = 0;
	int unfinish = 1;
#ifdef DEBUG
	for(; iter < 12; iter++){
#else
	for(; unfinish; iter++){
#endif
		//solve one iter in each proc
		int has_next_round = solve_one_iter(a_local,
				b_local,
				x,
				proc_startid[my_rank],
				proc_chunksize[my_rank],
				x_new);

		//check if we are finish?
		unfinish = 0;
		MPI_Allreduce(&has_next_round,
				&unfinish,
				1,
				MPI_INT,
				MPI_BOR,
				MPI_COMM_WORLD);

		//collect new_x
		MPI_Allgatherv(&x_new[proc_startid[my_rank]],
				proc_chunksize[my_rank],
				MPI_DATA_FLOAT_T,
				x,
				proc_chunksize,
				proc_startid,
				MPI_DATA_FLOAT_T,
				MPI_COMM_WORLD);
#ifdef DEBUG
		if(my_rank == 0){
			printf("### Iteration %d\n", iter);
			printf("Has Next Round %d\n", has_next_round);
			for(int i = 0; i<num; ++i){
				printf(FORMAT_STR"\n", x[i]);
			}
		}
#endif
	}
	free(a_local);
	free(b_local);
	free(proc_startid);
	free(proc_chunksize);

#ifdef ENABLE_TIMING
	if(my_rank == 0){
		finishTime = MPI_Wtime();
		printf("Computing Time = %lf seconds\n", finishTime - startTime);
	}
#endif

	return iter;
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

#ifdef ENABLE_TIMING
	double startTime, finishTime;
#endif

	//Process 0 read input
	if(my_rank == 0){

#ifdef ENABLE_TIMING
		startTime = MPI_Wtime();
#endif

		if( argc != 2){
			printf("Usage: ./gsref filename\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
  
		/* Read the input file and fill the global data structure above */ 
		get_input(argv[1]);

		if( num < comm_sz){
			printf("# of unknowns < number of processes, ABORT\n");
			MPI_Abort(MPI_COMM_WORLD, 2);
		}
 
		/* Check for convergence condition */
		/* This function will exit the program if the coffeicient will never converge to 
		* * the needed absolute error. 
		* * This is not expected to happen for this programming assignment.
		* */
		//check_matrix();

#ifdef ENABLE_TIMING
		finishTime = MPI_Wtime();
		printf("Reading Input Time = %lf seconds\n", finishTime - startTime);
#endif

	}

	int iter = solve_gs(comm_sz, my_rank);

	if(my_rank == 0){
 
		char output[100] ="";
 
		/* Writing results to file */
		FILE * fp;
		sprintf(output,"%d.sol",num);
		fp = fopen(output,"w");
		if(!fp){
			printf("Cannot create the file %s\n", output);
			exit(1);
		}
    
		for(int i = 0; i < num; i++)
			fprintf(fp, FORMAT_STR"\n",x[i]);
 
		printf("total number of iterations: %d\n", iter);
 
		fclose(fp);
	}

	MPI_Finalize();
 
	exit(0);
}
