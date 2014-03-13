/*
 * PPRCommon.h
 *
 *  Created on: Mar 12, 2014
 *      Author: wenleix
 */

#ifndef PPRCOMMON_H_
#define PPRCOMMON_H_

struct SimpleGraph;


const double DAMP_FACTOR = 0.85;

void JacobiSolver(SimpleGraph *g, double *pref, double *pprValue, int numIter);

#endif /* PPRCOMMON_H_ */
