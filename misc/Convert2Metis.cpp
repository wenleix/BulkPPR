//  Convert to the METIS format :)

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

	FILE* graphFile = fopen(argv[1], "r");
	FILE* metisFile = fopen(argv[2], "w");

	int numVtx, numEdge;
    int numRead = fscanf(graphFile, "%d%d", &numVtx, &numEdge);
    assert(numRead == 2);
	unordered_set<int>* neighborVertex = new unordered_set<int>[numVtx];
    for (int i = 0; i < numVtx; i++) {
        int outDeg = 0;
        numRead = fscanf(graphFile, "%d", &outDeg);
        assert(numRead == 1);
        for (int j = 0; j < outDeg; j++) {
            int u1 = i;
            int u2;
            numRead = fscanf(graphFile, "%d", &u2);
            assert(numRead == 1);
            neighborVertex[u1].insert(u2);
            neighborVertex[u2].insert(u1);
        }
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
	fclose(graphFile);
	fclose(metisFile);
}



