#include "Common.h"
#include <stdio.h>
#include <math.h>
extern data_float_t err;
extern int num;

int solve_one_iter(const data_float_t* restrict a_local,
		const data_float_t* restrict b_local,
		const data_float_t* restrict x,
		const int proc_startid,
		const int proc_chunksize,
		data_float_t* restrict x_new){
	//cheet sheet
	//x_i_new = (b_i - sigma(j = 1 to num)(a_ij * x_j) + a_ii * x_i) / a_ii
	//delta = (x_i_new - x_i)/x_i_new
	int unfinish = 0;
	for(int i = 0; i < proc_chunksize; ++i){
		const int curr_row = i + proc_startid;
		//NOTE: HERE WE FORCE TO USE DOUBLE DURING UPDATING
		//AVOID PRECISION PROBLEM
		double x_i_new = (double)b_local[i] + a_local[TO_ONE_DIM(i, curr_row, num)] * x[curr_row];
		for(int j = 0; j < num; ++j){
			x_i_new -= a_local[TO_ONE_DIM(i, j, num)] * x[j];
		}
		x_i_new = x_i_new / a_local[TO_ONE_DIM(i, curr_row, num)];
		double delta = (x_i_new - x[curr_row])/(x_new[curr_row]);
		unfinish |= ((fabs(delta) >= err) ? 1 : 0);
		x_new[curr_row] = (float)x_i_new;
#ifdef DEBUG
		printf("x_%d, old val = "FORMAT_STR", new val = "FORMAT_STR", delta = "FORMAT_STR", unfinish = %d\n",
				curr_row, x[curr_row], x_new[curr_row], delta, unfinish);
#endif
	}
	return unfinish;
}


