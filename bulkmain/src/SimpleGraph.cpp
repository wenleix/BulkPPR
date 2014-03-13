/*
 * SimpleGraph.cpp
 *
 *  Created on: Dec 16, 2013
 *      Author: wenleix
 */

#include <cstddef>
#include <sys/time.h>

#include "SimpleGraph.h"

//  Construct from the binary format file
//  Format:
//  [# Vertex] [# Edges]
//  [# OutDegree] [DstID1] [DstID2]...
SimpleGraph* SimpleGraph::readBinaryFile(FILE* binFile) {
    timeval start, end;
    gettimeofday(&start, NULL);

    SimpleGraph *g = new SimpleGraph();
	int numRead;

    numRead = fread(&g->numVertex, sizeof(int), 1, binFile);
    assert(numRead == 1);
    numRead = fread(&g->numEdge, sizeof(int), 1, binFile);
    assert(numRead == 1);

    fprintf(stdout, "INFO: # Vertex: %d, # Edge: %d\n", g->numVertex, g->numEdge);

    g->vertexList = new SimpleVertex[g->numVertex];
	for (int i = 0; i < g->numVertex; i++)
		g->vertexList[i].inDeg = 0;
    g->edgeList = new int[g->numEdge];

    int currentOffset = 0;
    for (int i = 0; i < g->numVertex; i++) {
        int outDeg;
        numRead = fread(&outDeg, sizeof(int), 1, binFile);
        assert(numRead == 1);
        assert(currentOffset + outDeg <= g->numEdge);

        int *edgeStart = g->edgeList + currentOffset;
        numRead = fread(edgeStart, sizeof(int), outDeg, binFile);
        assert(numRead == outDeg);

        //  Check self-loop and update inDeg
        for (int j = 0; j < outDeg; j++) {
			int dstID = edgeStart[j];
            assert(i != dstID);
            g->vertexList[dstID].inDeg++;
		}

        g->vertexList[i].vertexID = i;
        g->vertexList[i].outDeg = outDeg;
        g->vertexList[i].outVtx = g->edgeList + currentOffset;

        currentOffset += outDeg;
    }

    assert(currentOffset == g->numEdge);

    gettimeofday(&end, NULL);

    double elapse_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    fprintf(stdout, "INFO: Graph load done, time elapsed: %.2f sec.\n", elapse_ms / 1000.0);
	fflush(stdout);

	return g;
}


SimpleGraph* SimpleGraph::readTextFile(FILE* txtFile) {
    timeval start, end;
    gettimeofday(&start, NULL);

    SimpleGraph *g = new SimpleGraph();
	int numRead;
    numRead = fscanf(txtFile, "%d%d", &g->numVertex, &g->numEdge);
    assert(numRead == 2);

    fprintf(stdout, "INFO: # Vertex: %d, # Edge: %d\n", g->numVertex, g->numEdge);

    g->vertexList = new SimpleVertex[g->numVertex];
	for (int i = 0; i < g->numVertex; i++)
		g->vertexList[i].inDeg = 0;
	g->edgeList = new int[g->numEdge];

    int currentOffset = 0;
    for (int i = 0; i < g->numVertex; i++) {
        int outDeg;
        numRead = fscanf(txtFile, "%d", &outDeg);
        assert(numRead == 1);
        assert(currentOffset + outDeg <= g->numEdge);

        int *edgeStart = g->edgeList + currentOffset;
        for (int j = 0; j < outDeg; j++) {
        	numRead = fscanf(txtFile, "%d", &edgeStart[j]);
            assert(numRead == 1);
        }

        //  Check self-loop and update inDeg
        for (int j = 0; j < outDeg; j++) {
			int dstID = edgeStart[j];
            assert(i != dstID);
            g->vertexList[dstID].inDeg++;
		}

        g->vertexList[i].vertexID = i;
        g->vertexList[i].outDeg = outDeg;
        g->vertexList[i].outVtx = g->edgeList + currentOffset;

        currentOffset += outDeg;
    }

    assert(currentOffset == g->numEdge);

    gettimeofday(&end, NULL);

    double elapse_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    fprintf(stdout, "INFO: Graph load done, time elapsed: %.2f sec.\n", elapse_ms / 1000.0);
	fflush(stdout);
}


