//	A misc tool to report the size of average expanded block.
//	Put it here because I am lazy and would like to use SimpleGraph :P

#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <unordered_set>

#include "SimpleGraph.h"

using namespace std;

const int NumExpand = 4;

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

int CountInnerEdge(SimpleGraph *g, vector<int>& eblock, bool *inBlock) {
	int numEdges = 0;
	for (int vid : eblock) {
		SimpleVertex *vtx = g->getVertex(vid);
		for (int j = 0; j < vtx->outDeg; j++) {
			int dstId = vtx->outVtx[j];
			if (inBlock[dstId])
				numEdges++;
		}
	}

	return numEdges;
}

void BlockExpand(SimpleGraph *g, const vector<int>& block, int numVtx[], int numEdge[], 
		         int outDegCut, int nexpand) {
//	fprintf(stderr, "DEBUG: Start Block Expansion!\n");

	vector<int> expandedBlock(block.begin(), block.end());
	bool *inBlock = new bool[g->numVertex];
	memset(inBlock, 0, sizeof(bool) * g->numVertex);
	for (int vid : expandedBlock)
		inBlock[vid] = true;

	//	Start with the original block
	numVtx[0] = expandedBlock.size();
	numEdge[0] = CountInnerEdge(g, expandedBlock, inBlock);

	for (int expand = 1; expand <= nexpand; expand++) {
		//	Expand out
		vector<int> newBoundary;
		for (int vid : expandedBlock) {
			SimpleVertex *vtx = g->getVertex(vid);
			if (vtx->outDeg > outDegCut) continue;
			for (int j = 0; j < vtx->outDeg; j++) {
				int dstId = vtx->outVtx[j];
				if (!inBlock[dstId]) {	//	New boundary vertex
					newBoundary.push_back(dstId);
					inBlock[dstId] = true;
				}
			}
		}
		expandedBlock.insert(expandedBlock.end(), newBoundary.begin(), newBoundary.end());

		numVtx[expand] = expandedBlock.size();
		numEdge[expand] = CountInnerEdge(g, expandedBlock, inBlock);
	}

	delete [] inBlock;
	fprintf(stderr, "DEBUG: End Block Expansion! ::  ");
	for (int i = 0; i <= nexpand; i++)
		fprintf(stderr, " (%d, %d)", numVtx[i], numEdge[i]);
	fprintf(stderr, "\n");
}

int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "%s [graph.txt] [metis-partition.txt] [OutDegCut]\n", argv[0]);
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
	int outDegCut = atoi(argv[3]);

	SimpleGraph *g = txtFile ? SimpleGraph::readTextFile(gfile) : SimpleGraph::readBinaryFile(gfile);
	int *blockID = new int[g->numVertex];
	int numBlock = ReadMetisFile(metisFile, blockID, g->numVertex);

	fprintf(stdout, "INFO: # Blocks = %d.\n", numBlock);
	vector<int> *blocks = new vector<int>[numBlock];
	//	Build ID list for each block
	for (int i = 0; i < g->numVertex; i++)
		blocks[blockID[i]].push_back(i);

	//	Randomly choose 100 blocks
	int *blockIdx = new int[numBlock];
	for (int i = 0; i < numBlock; i++)
		blockIdx[i] = i;
	srand(128827);
	random_shuffle(blockIdx, blockIdx + numBlock);
	int processedBlocks = 0;
	long long sumVtx[NumExpand + 1] = {0}, sumEdge[NumExpand + 1] = {0};
	fprintf(stderr, "DEBUG: Start to count size of blocks!\n");
	for (int i = 0; i < numBlock; i++) {
		int blkId = blockIdx[i];
		fprintf(stderr, "DEBUG: BlockID: %d\n", blkId);
		if (blocks[blkId].size() == 0)
			continue;

		int numVtx[NumExpand + 1], numEdge[NumExpand + 1];
		BlockExpand(g, blocks[blkId], numVtx, numEdge, outDegCut, NumExpand);

		for (int j = 0; j <= NumExpand; j++) {
			sumVtx[j] += numVtx[j];
			sumEdge[j] += numEdge[j];
		}

		processedBlocks++;
		if (processedBlocks == 100)
			break;
	}

	fprintf(stdout, "INFO: %d blocks processed! \n", processedBlocks);
	for (int i = 0; i <= NumExpand; i++)
		fprintf(stdout, "INFO: Block Expansion %d, Avg Vtx %.2lf, Avg Edge %.2lf.\n",
				i, (double)sumVtx[i] / processedBlocks, (double)sumEdge[i] / processedBlocks);

	delete [] blockID;
	delete [] blocks;
	fclose(gfile);
	fclose(metisFile);

	return 0;
}


