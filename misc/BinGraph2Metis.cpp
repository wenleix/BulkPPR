//  Convert the binary version of graph to the METIS format :)
//  Since for large graphs I am using binary format :)

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <unordered_set>

using namespace std;

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [GraphFile] [MetisFile]\n", argv[0]);
		return 1;
	}

	FILE* binFile = fopen(argv[1], "rb");
	FILE* metisFile = fopen(argv[2], "w");

    int numRead;
	int numVtx, numEdge;
    numRead = fread(&numVtx, sizeof(int), 1, binFile);
    assert(numRead == 1);
    numRead = fread(&numEdge, sizeof(int), 1, binFile);
    assert(numRead == 1);

	unordered_set<int>* neighborVertex = new unordered_set<int>[numVtx];
    for (int i = 0; i < numVtx; i++) {
        int outDeg = 0;
        numRead = fread(&outDeg, sizeof(int), 1, binFile);
        assert(numRead == 1);

        int *outId = new int[outDeg];
        numRead = fread(outId, sizeof(int), outDeg, binFile);
        for (int j = 0; j < outDeg; j++) {
            int u1 = i;
            int u2 = outId[j];
            neighborVertex[u1].insert(u2);
            neighborVertex[u2].insert(u1);
        }

        delete [] outId;
    }
	fprintf(stderr, "Read complete!\n");

	//	Count # edges
	int countEdges = 0;
	for (int i = 0; i < numVtx; i++)
		countEdges += neighborVertex[i].size();

	//	Write out
    fprintf(stdout, "Double Check: Original # Edges: %d, count Edges: %d. \n", 
            countEdges, numEdge);

	fprintf(metisFile, "%d %d\n", numVtx, countEdges / 2);
	for (int i = 0; i < numVtx; i++) {
		for (unordered_set<int>::iterator it = neighborVertex[i].begin();
				it != neighborVertex[i].end(); it++) {
			//	Metis doesn't require the vertex list to be sorted
			fprintf(metisFile, "%d ", (*it) + 1);
		}
		fprintf(metisFile, "\n");
	}

	delete [] neighborVertex;
	fclose(binFile);
	fclose(metisFile);
}



