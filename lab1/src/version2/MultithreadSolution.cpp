#include "MultithreadSolution.h"
#include "Common.h"
#include<cstdlib>
#include<cstdio>
#include<climits>
#include<omp.h>
namespace {
	void DFS(const int* dis,
		const int numCities,
		const int currDis,
		const int currCity,
		int* currAnsSeq,
		const int mask,
		const int depth,
		int& minDis_s,
		int& minDis,
		int* minAnsSeq) {
		if (depth == numCities) {
			bool setSeq = false;
#pragma omp critical
			{
				if (currDis < minDis_s) {
					minDis_s = currDis;
					setSeq = true;
				}
			}
			if (setSeq) {
				minDis = currDis;
				for (int i = 0; i < numCities; ++i) {
					minAnsSeq[i] = currAnsSeq[i];
				}
			}
			return;
		}
		for (int i = 0; i < numCities; ++i) {
			if (testBit(mask, i))continue;
			const int disi = dis[toOneDim(currCity, i, numCities)];

			int globalMinDis;
#pragma omp atomic read
			globalMinDis = minDis_s;

			if (currDis + disi >= globalMinDis)continue;
			currAnsSeq[depth] = i;
			DFS(dis, numCities,
				currDis + disi,
				i,
				currAnsSeq,
				setBit(mask, i),
				depth + 1,
				minDis_s,
				minDis,
				minAnsSeq);
		}
	}
}//anonymous namespace
void MultithreadSolution::tsm(const int* dis, 
	const int numCities, 
	const int numThreads)
{
	if(numCities > 1){
		omp_set_num_threads(numThreads);

		//shared variables
		int minDis_s = INT_MAX;
		int *minDis_ns = new int[numThreads];
		int *minAnsSeqs = new int[numThreads * numCities];

	#pragma omp parallel
		{
			//private variables
			int tid = omp_get_thread_num();
			int currAnsSeq[MAXCITIES];
			int *minAnsSeq = minAnsSeqs + tid * numCities;

			minDis_ns[tid] = INT_MAX;

			//make workload distributed more evenly:
			//manually unroll the first two loops!
		#pragma omp for schedule(static)
			for (int i = 0; i < numCities * numCities; ++i) {
				int firstId = i/numCities;
				int secondId = i%numCities;
				if(firstId == secondId)continue;
				
				const int disi = dis[toOneDim(firstId, secondId, numCities)];

				int globalMinDis;
			#pragma omp atomic read
				globalMinDis = minDis_s;

				if (disi >= globalMinDis)continue;
				currAnsSeq[0] = firstId;
				currAnsSeq[1] = secondId;
				DFS(dis, numCities,
					disi,
					secondId,
					currAnsSeq,
					setBit(setBit(0, firstId),secondId),
					2,
					minDis_s,
					minDis_ns[tid],
				minAnsSeq);
			}	
		}
		//get the result
		for (int i = 0; i < numThreads; ++i) {
			if (minDis_ns[i] == minDis_s) {
				int* minAnsSeq = minAnsSeqs + i * numCities;

#ifdef DEBUG
				int checkCount = 0;
#endif

				printf("Best path:");
				for (int j = 0; j < numCities; ++j) {
					printf(" %d", minAnsSeq[j]);

#ifdef DEBUG
					if(j > 0){
						checkCount += dis[toOneDim(minAnsSeq[j-1],minAnsSeq[j],numCities)];
					}
#endif
				}
				printf("\nDistance: %d\n", minDis_s);

#ifdef DEBUG
				if(checkCount == minDis_s){
					printf("Ans Check Pass\n");
				}
				else{
					printf("Ans Check FAILED\n");
				}
#endif

				break;
			}	
		}

		//clean up
		delete minDis_ns;
		delete minAnsSeqs;
	}
	else{
		printf("Best Path: 0\nDistance: 0\n");
	}
}
