#include"SerialSolution.h"
#include"MultithreadSolution.h"
#include"Common.h"
#include<cstdio>
#include<cstdlib>


int main(int argc, char** argv) {
	int numCities = atoi(argv[1]);

#ifdef USE_MULTI_THREAD 
	int numThreads = atoi(argv[2]);
	char* filePath = argv[3];
#else
	char* filePath = argv[2];
#endif

	FILE* fptr = fopen(filePath, "r");
	if (fptr == NULL)
	{
		printf("Cannot open file \n");
		exit(0);
	}
	int *dis = new int[numCities* numCities];
	for (int i = 0; i < numCities * numCities; ++i) {
		fscanf(fptr,"%d", &dis[i]);
	}

#ifndef USE_MULTI_THREAD
	SerialSolution::tsm(dis, numCities);
#else
	MultithreadSolution::tsm(dis, numCities, numThreads);
#endif
	delete dis;
	return 0;
}
