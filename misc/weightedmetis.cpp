//  Convert to the METIS format, with edge weights now!! :)

#include <cstdio>
#include <cstdlib>
#include <unordered_set>

using namespace std;

const int FACTOR = 1000;

int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [GraphFile] [MetisFile]\n", argv[0]);
		return 1;
	}

	FILE* graphFile = fopen(argv[1], "r");
	FILE* metisFile = fopen(argv[2], "w");

	int numVtx, numEdge;
    fscanf(graphFile, "%d%d", &numVtx, &numEdge);
	unordered_set<int>* neighborVertex = new unordered_set<int>[numVtx];
    for (int i = 0; i < numVtx; i++) {
        int outDeg = 0;
        fscanf(graphFile, "%d", &outDeg);
        for (int j = 0; j < outDeg; j++) {
            int u1 = i;
            int u2;
            fscanf(graphFile, "%d", &u2);
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

	fprintf(metisFile, "%d %d 001\n", numVtx, countEdges / 2);
	for (int i = 0; i < numVtx; i++) {
		for (unordered_set<int>::iterator it = neighborVertex[i].begin();
				it != neighborVertex[i].end(); it++) {
			//	Metis doesn't require the vertex list to be sorted
            double estWeight = 1.0 / neighborVertex[i].size() + 1.0 / neighborVertex[*it].size();
            int intWeight = (int)(estWeight * FACTOR) + 1;
			fprintf(metisFile, "%d %d ", (*it) + 1, intWeight);
		}
		fprintf(metisFile, "\n");
	}

	delete [] neighborVertex;
	fclose(graphFile);
	fclose(metisFile);
}



