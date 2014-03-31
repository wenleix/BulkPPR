//  Preprocess the TwitterData Set
//  Relabel the dataset and convert it into Wenlei Graph format

//  Wenlei Graph Format:
//  [NumVertex] [NumEdge]
//  [OutDeg] [DstID1] [DstID2]...
//  [OutDeg] [DstID1] [DstID2]...

#include <cstdio>
#include <cstring>
#include <vector>

using namespace std;

const int MaxOriginalID = 80000000;
const int MaxNumVertex = 20000000;   //  20M vertices atm.
int mapping[MaxOriginalID];
vector<int> outEdge[MaxNumVertex];

int newIDCnt = 0;

int relabel(int u) {
    int *newID = &mapping[u];
    if (*newID == -1) 
        *newID = newIDCnt++;
    
    return *newID;
}

int main(int argv, char** argc) {
    memset(mapping, 0xFF, sizeof(mapping));

    if (argv != 3) {
        fprintf(stderr, "Usage: %s [JureData] [OutData]\n", argc[0]);
        return 1;
    }
    
    FILE *inFile = fopen(argc[1], "r");
    FILE *outFile = fopen(argc[2], "w");

    char buf[1000];
    int u, v;
    int edgeCnt = 0;

    while (fscanf(inFile, "%d%d", &v, &u) != EOF) {
        sscanf(buf, "%d%d", &u, &v);
        int uu = relabel(u);
        int vv = relabel(v);
        if (uu == vv) 
            continue;
        outEdge[uu].push_back(vv);
        edgeCnt++;
    }

    //  Print the output file
    fprintf(outFile, "%d %d\n", newIDCnt, edgeCnt);
    for (int i = 0; i < newIDCnt; i++) {
        fprintf(outFile, "%lu", outEdge[i].size());
        for (int j = 0; j < outEdge[i].size(); j++)
            fprintf(outFile, " %d", outEdge[i][j]);
        fprintf(outFile, "\n");
    }

    fclose(inFile);
    fclose(outFile);

    fprintf(stdout, "INFO: Vertex: %d, Edge: %d\n", newIDCnt, edgeCnt);

    return 0;
}

