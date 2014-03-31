/*
 * PPRCommon.cpp
 *
 *  Created on: Mar 12, 2014
 *      Author: wenleix
 */

#include <cstring>
#include <cmath>

#include <sys/time.h>

#include "PPRCommon.h"
#include "SimpleGraph.h"

//	The standard Jacobi solver, used as baseline.
void JacobiSolver(SimpleGraph *g, double *pref, double *pprValue, int numIter) {
	double *preValue = new double[g->numVertex];
	double *curValue = new double[g->numVertex];

	memset(preValue, 0, sizeof(double) * g->numVertex);

    timeval start, end;
    gettimeofday(&start, NULL);

    for (int iIter = 0; iIter < numIter; iIter++) {
    	memcpy(curValue, pref, sizeof(double) * g->numVertex);

        for (int i = 0; i < g->numVertex; i++) {
            if (preValue[i] == 0)
                continue;

            SimpleVertex *vtx = g->getVertex(i);
            double mycontribute = preValue[i] / vtx->outDeg * DAMP_FACTOR;

            for (int j = 0; j < vtx->outDeg; j++) {
                int dstID = vtx->outVtx[j];
                curValue[dstID] += mycontribute;
            }
        }

		double* temp = curValue;
		curValue = preValue;
		preValue = temp;
    }

    //	Calculate the L1 change in the last iteration.
    double L1Change = 0.0;
    for (int i = 0; i < g->numVertex; i++)
    	L1Change += fabs(curValue[i] - preValue[i]);

    fprintf(stderr, "DEBUG: L1Change in Last Iteration = %.4le\n", L1Change);

    //  Copy the result out
	memcpy(pprValue, preValue, sizeof(double) * g->numVertex);

    int nonZeroCount = 0;
    for (int i = 0; i < g->numVertex; i++)
        if (pprValue[i] != 0.0) nonZeroCount++;
    fprintf(stdout, "DEBUG: # non zero entries / All vertex: %d / %d\n", nonZeroCount, g->numVertex);

    gettimeofday(&end, NULL);

    double elapse_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    fprintf(stdout, "INFO: Jacobi Solver Running Time: %.2f sec.\n", elapse_ms / 1000.0);


    delete [] preValue;
    delete [] curValue;
}



