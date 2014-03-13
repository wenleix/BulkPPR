/*
 * BlockPPR.cpp
 *
 *  Created on: Mar 12, 2014
 *      Author: wenleix
 */

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>

#include "PPRCommon.h"
#include "SimpleGraph.h"

using namespace std;

vector< vector<int> >* ReadMETISPartitions(FILE* metisFile, int numVertex) {
//	fprintf(stderr, "here!!\n");

	vector< vector<int> > *blocks = new vector< vector<int> >();
	for (int i = 0; i < numVertex; i++) {
		int blockID = -1;
		int numRead = fscanf(metisFile, "%d", &blockID);
		assert(numRead == 1);
//		fprintf(stderr, "blockID: %d\n", blockID);

		if (blockID >= blocks->size()) {
			blocks->resize(blockID + 1);
		}

		(*blocks)[blockID].push_back(i);
	}

	fprintf(stdout, "INFO: # Blocks: %lu .\n", blocks->size());
	return blocks;
}

void printPPRValue(FILE *pprFile, double *pprValue, int n) {
	for (int i = 0; i < n; i++)
		fprintf(pprFile, "%.10lf\n", pprValue[i]);
}

//	Compute the groundtruth PPR vectors for a block of vertices.
int main(int argc, char** argv) {
	if (argc != 6) {
		fprintf(stderr, "Usage: %s [GraphFile] [METIS-File] [BlockID] [NumIter] [OutputDir]\n", argv[0]);
		return 1;
	}

	FILE *graphFile = fopen(argv[1], "r");
	FILE *metisFile = fopen(argv[2], "r");
	int blockID = atoi(argv[3]);
	int numIter = atoi(argv[4]);
	char* outDir = argv[5];

	SimpleGraph* g = SimpleGraph::readTextFile(graphFile);
	vector< vector<int> > *blocks = ReadMETISPartitions(metisFile, g->numVertex);

	double *pref = new double[g->numVertex];
	double *pprValue = new double[g->numVertex];
	memset(pref, 0, sizeof(double) * g->numVertex);
	//	Process the block
	for (int i = 0; i < (*blocks)[blockID].size(); i++) {
		int vtxID = (*blocks)[blockID][i];
		fprintf(stderr, "DEBUG: Start for vertex %d .\n", vtxID);
		pref[vtxID] = 1.0 - DAMP_FACTOR;

		JacobiSolver(g, pref, pprValue, numIter);

		pref[vtxID] = 0.0;

		//	Print out
		char fnameBuf[1000];
		sprintf(fnameBuf, "%s/ppr_%d.txt", outDir, vtxID);
		FILE *pprFile = fopen(fnameBuf, "w");
		printPPRValue(pprFile, pprValue, g->numVertex);
		fclose(pprFile);
	}


	delete blocks;
	delete [] pref;
	delete [] pprValue;

	fclose(graphFile);
	fclose(metisFile);

	return 0;
}

