#include"Common.h"
int solve_one_iter(const data_float_t* restrict a_local,
        const data_float_t* restrict b_local,
        const data_float_t* restrict x,
        const int proc_startid,
        const int proc_chunksize,
        data_float_t* restrict x_new);
