/** MACRO FUNCTIONS **/
#define TO_ONE_DIM(x, y, n) ((x) * (n) + (y))


/** SWITCHES **/
//#define DEBUG
#define ENABLE_TIMING
#define USE_32BIT_FLOAT


/** FLOTING TYPE SWITCH **/

/** 32-bit float **/
#ifdef USE_32BIT_FLOAT

#define data_float_t float
#define MPI_DATA_FLOAT_T MPI_FLOAT
#define FORMAT_STR "%f"

/** 64-bit float **/
#else 

#define data_float_t double
#define MPI_DATA_FLOAT_T MPI_DOUBLE
#define FORMAT_STR "%lf"

#endif 
