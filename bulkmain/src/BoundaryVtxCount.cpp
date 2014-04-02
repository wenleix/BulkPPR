//	A misc tool to count the number of the boundary vertex

#include <cstdio>
#include <cstring>
#include <cassert>
#include <vector>
#include <cstdlib>

#include "SimpleGraph.h"

using namespace std;

//	TODO(wxie): Refactor and put in a common header??
int ReadMetisFile(FILE *metisFile, int *blockID, int numVertex) {
	int numVertexRead = 0;
	while (fscanf(metisFile, "%d", &blockID[numVertexRead]) != EOF)
		numVertexRead++;

	if (numVertexRead != numVertex)
		fprintf(stderr, "Error, vertex read: %d \n", numVertexRead);
	assert(numVertexRead == numVertex);
	int maxBlockID = -1;
	for (int i = 0; i < numVertex; i++)
		if (blockID[i] > maxBlockID)
			maxBlockID = blockID[i];

	return maxBlockID + 1;
}

int GetNumBoundaryVtx(SimpleGraph *g, const vector<int>& block) {
	bool *inBlock = new bool[g->numVertex];
	memset(inBlock, 0, sizeof(bool) * g->numVertex);
	for (int vid : block)
		inBlock[vid] = true;

	int numBoundary = 0;
	for (int vid : block) {
		bool touchOutside = false;
		SimpleVertex *vtx = g->getVertex(vid);
		for (int j = 0; j < vtx->outDeg; j++) {
			int dstId = vtx->outVtx[j];
			if (!inBlock[dstId]) {
				touchOutside = true;
				break;
			}
		}

		if (touchOutside)
			numBoundary++;
	}

	delete [] inBlock;

	return numBoundary;
}



int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "%s [graph.txt] [metis-partition.txt] [partitionID]\n", argv[0]);
		return 1;
	}

	bool txtFile = true;
	if (strcmp(argv[1] + strlen(argv[1]) - 4, ".txt") == 0) {
		txtFile = true;
		fprintf(stdout, "INFO: txt file!\n");
	}
	else if (strcmp(argv[1] + strlen(argv[1]) - 4, ".bin") == 0) {
		txtFile = false;
		fprintf(stdout, "INFO: bin file!\n");
	}
	else {
		assert(false);
	}

	FILE *gfile = txtFile ? fopen(argv[1], "r") : fopen(argv[1], "rb");
	FILE *metisFile = fopen(argv[2], "r");
	int partitionId = atoi(argv[3]);	//	The partition we would like to evaluate

	SimpleGraph *g = txtFile ? SimpleGraph::readTextFile(gfile) : SimpleGraph::readBinaryFile(gfile);
	int *blockID = new int[g->numVertex];
	int numBlock = ReadMetisFile(metisFile, blockID, g->numVertex);

	fprintf(stdout, "INFO: # Blocks = %d.\n", numBlock);
	vector<int> *blocks = new vector<int>[numBlock];
	//	Build ID list for each block
	for (int i = 0; i < g->numVertex; i++)
		blocks[blockID[i]].push_back(i);

	int numBoundaryVtx = GetNumBoundaryVtx(g, blocks[partitionId]);

	fprintf(stdout, "INFO: # Boundary Vertices for Block %d: %d \n", partitionId, numBoundaryVtx);

	return 0;
}



