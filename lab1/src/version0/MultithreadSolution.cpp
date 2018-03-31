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
		int& minDis,
		int* minAnsSeq) {
		//leaf case
		if (depth == numCities) {
			if (currDis < minDis) {
				minDis = currDis;
				for (int i = 0; i < numCities; ++i) {
					minAnsSeq[i] = currAnsSeq[i];
				}
			}
			return;
		}
		//DFS search
		for (int i = 1; i < numCities; ++i) {
			//already visite city i before, skip it
			if (testBit(mask, i))continue;

			//get the distance from current city to city i
			const int disi = dis[toOneDim(currCity, i, numCities)];

			//over the minimum result so far, skip it
			if (currDis + disi >= minDis)continue;

			//add the next city into tracking array
			currAnsSeq[depth] = i;

			DFS(dis, numCities,
				currDis + disi,
				i,
				currAnsSeq,
				setBit(mask, i),
				depth + 1,
				minDis,
				minAnsSeq);
		}
	}
}//anonymous namespace
void MultithreadSolution::tsm(const int* dis, 
	const int numCities, 
	const int numThreads)
{
	if(numCities > 2){
		omp_set_num_threads(numThreads);

		//shared variables
		int minDis_s = INT_MAX;
		int minAnsSeq_s[MAXCITIES];

	#pragma omp parallel
		{
			//private variables
			int currAnsSeq[MAXCITIES];

			//must start from 0 !
			currAnsSeq[0] = 0;

			int minAnsSeq[MAXCITIES];
			int minDis_ns = INT_MAX;

			//make workload distributed more evenly:
			//manually unroll the first two loops!
		#pragma omp for schedule(static)
			for (int i = 0; i < (numCities-1) * (numCities-1); ++i) {
				
				//the city id for first two cities
				int firstId = i/(numCities-1) + 1;
				int secondId = i%(numCities-1) + 1;
				if(firstId == secondId)continue;
			
				//the distance between first two cities	
				const int disi = dis[firstId] + 
					dis[toOneDim(firstId, secondId, numCities)];

				if (disi >= minDis_ns)continue;
				currAnsSeq[1] = firstId;
				currAnsSeq[2] = secondId;
				DFS(dis, numCities,
					disi,
					secondId,
					currAnsSeq,
					setBit(setBit(1, firstId),secondId),
					3,
					minDis_ns,
				minAnsSeq);
			}
		//gather up the result
		#pragma omp critical
			if(minDis_ns < minDis_s){
				minDis_s = minDis_ns;
				for(int i = 0;i<numCities;++i){
					minAnsSeq_s[i] = minAnsSeq[i];
				}
			}
		}

		//print the final result

#ifdef DEBUG
		int checkCount = 0;
#endif

		printf("Best path:");
		for (int j = 0; j < numCities; ++j) {
			printf(" %d", minAnsSeq_s[j]);

#ifdef DEBUG
			if(j > 0){
				checkCount += dis[toOneDim(minAnsSeq_s[j-1],minAnsSeq_s[j],numCities)];
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
	}
	else if(numCities == 1){
		printf("Best Path: 0\nDistance: 0\n");
	}
	else if(numCities == 2){
		printf("Best Path: 0 1\nDistance: %d\n", dis[1]);
	}
	else{
		printf("Number of Cities must be positive!\n");
	}
}
