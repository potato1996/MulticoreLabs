#define TO_ONE_DIM(x, y, n) ((x) * (n) + (y))
extern float err;
extern int num;

int solve_one_iter(const float* restrict a_local,
		const float* restrict b_local,
		const float* restrict x,
		const int proc_startid,
		const int proc_chunksize,
		float* restrict x_new){
	//cheet sheet
	//x_i_new = (b_i - sigma(j = 1 to num)(a_ij * x_j) + a_ii * x_i) / a_ii
	//delta = (x_i_new - x_i)/x_i_new
	int unfinish = 0;
	for(int i = 0; i < proc_chunksize; ++i){
		const int curr_row = i + proc_startid;
		float x_i_new = b_local[i];
		for(int j = 0; j < num; ++j){
			x_i_new -= a_local[TO_ONE_DIM(i, j, num)] * x[j];
		}
		x_i_new += a_local[TO_ONE_DIM(i, curr_row, num)] * x[curr_row];
		x_new[curr_row] = x_i_new / a_local[TO_ONE_DIM(i, curr_row, num)];
		float delta = (x_new[curr_row] - x[curr_row])/(x_new[curr_row]);
		unfinish |= ((delta >= err) ? 1 : 0);
	}
	return unfinish;
}


