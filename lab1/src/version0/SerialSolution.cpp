#include"SerialSolution.h"
#include"Common.h"
#include<cstdlib>
#include<cstdio>
#include<climits>
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
		if (depth == numCities) {
			if (currDis < minDis) {
				minDis = currDis;
				for (int i = 0; i < numCities; ++i) {
					minAnsSeq[i] = currAnsSeq[i];
				}
			}
			return;
		}
		for (int i = 1; i < numCities; ++i) {
			if (testBit(mask, i))continue;
			const int disi = dis[toOneDim(currCity, i, numCities)];
			if (currDis + disi >= minDis)continue;
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
void SerialSolution::
tsm(const int * dis, const int numCities)
{
	int minDis = INT_MAX;
	int currAnsSeq[MAXCITIES];
	currAnsSeq[0] = 0; //must start from city 0
	int minAnsSeq[MAXCITIES];
	for (int i = 1; i < numCities; ++i) {
		currAnsSeq[1] = i;
		DFS(dis, numCities,
			dis[toOneDim(0,i,numCities)], i, currAnsSeq,
			setBit(1, i), 2, minDis, minAnsSeq);
	}
	if(numCities > 1){
		printf("Best path:");
		for (int j = 0; j < numCities; ++j) {
			printf(" %d", minAnsSeq[j]);
		}
		printf("\nDistance: %d\n", minDis);
	}
	else{
		printf("Best Path: 0\nDistance: 0\n");
	}
	return;
}
