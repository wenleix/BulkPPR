//  A simple graph structure to be used 

//  I keep the structure to be simple so it would be easier to reason about the performance,
//	as the Sparse PPR problem are usually solved by one-thread using some careful code.

//  I am not going to handling more than 2 billion edges at this moment...

#ifndef SIMPLE_GRAPH_H
#define SIMPLE_GRAPH_H

#include <cassert>
#include <cstdio>

//  Just the graph structure

struct SimpleVertex {
    int vertexID;   //  This is redundant, but I guess it is fine to be a bit redudant on vertex data :P

	int inDeg;
    int outDeg;
    int* outVtx;  	//  The outgoing vertices list (stored in ID)
};


struct SimpleGraph {
    int numVertex;
    int numEdge;

    SimpleVertex* vertexList;
    int* edgeList;      //  We can have an array of edges for each vertex...

    inline SimpleVertex* getVertex(int vertexID) {
        return vertexList + vertexID;
    }


    //  Construct from the binary format file
    //  Format: 
    //  [# Vertex] [# Edges]
    //  [# OutDegree] [DstID1] [DstID2]...
    static SimpleGraph* readBinaryFile(FILE* binFile);

    //	Same, but text format
    static SimpleGraph* readTextFile(FILE* txtFile);


    ~SimpleGraph() {
        delete [] vertexList;
        delete [] edgeList;
    }

private:
    SimpleGraph() {
    	numVertex = -1;
    	numEdge = -1;
    	vertexList = NULL;
    	edgeList = NULL;
    }
};

void removeNeighbor(SimpleGraph* g, int root, double *rank);

#endif

