#   Low-Rank approximation for PPR values
#   TODO(wxie): Looks like some functions can be refactored with ppr_svd.py

import sys
from sys import argv
import numpy as np
from scipy import linalg
import random

def ReadBlocks(metisFile):
    lines = metisFile.readlines()
    vtxBlockID = [int(l) for l in lines]
    numBlock = max(vtxBlockID) + 1
    n = len(vtxBlockID)
    print 'Num Blocks: %d, Num Vtx: %d' % (numBlock, n)

    blocks = [[] for _ in range(numBlock)]
    for i in range(len(vtxBlockID)):
        bid = vtxBlockID[i]
        blocks[bid].append(i)

    return (n, blocks)


#   Filenames are in the format ppr_%d.txt
def ReadPPRs(dirname, vidList):
    pprs = []
    for vid in vidList:
        fname = '%s/ppr_%d.txt' % (dirname, vid)
        pprFile = open(fname, 'r')
        lines = pprFile.readlines()
        vals = [float(l) for l in lines]
        nonzero = len(filter(lambda x: x != 0, vals))
        if nonzero < 100:		#	Skip those uninteresting guys...
            continue

        pprvector = np.array(vals)
        pprFile.close()

        pprs.append(pprvector)

    return pprs


#   Return a set
def GetExtendedBlock(vidList, graphFile, expand):
    if expand == -1:
        return set()

    #   Read in the graph

    firstLine = graphFile.readline()
    parsed = firstLine.split(' ')
    numVtx = int(parsed[0])
    gr = []       # the graph edge list
    restLines = graphFile.readlines()
    for i in range(len(restLines)):
        outIdStr = restLines[i].split(' ')[1:]
        outId = [int(s) for s in outIdStr]
        gr.append(outId)

    assert len(gr) == numVtx

    extendedSet = set(vidList)
    oldHop = set(vidList)
    for i in range(expand):
        newHop = set()
        for vid in oldHop:
            newHop |= set(gr[vid])      #   Add the outgoing vertices

        extendedSet |= newHop
        oldHop = newHop


    return extendedSet


def ZeroifyBlock(pprs, blockList):
    npprs = []
    for i in range(len(pprs)):
        nppr = np.zeros(len(pprs[i]))
        for idx in blockList:
            nppr[idx] = pprs[i][idx]
        npprs.append(nppr)
    return npprs


def EvaluatePrec(approx, truth, K):
    assert len(approx) == len(truth)
    n = len(approx)

    appList = [(i, approx[i]) for i in range(n)]
    trueList = [(i, truth[i]) for i in range(n)]

    # 	Random shuffle for potential tie
    random.shuffle(appList)
    random.shuffle(trueList)	

    appList.sort(key = lambda tup: tup[1], reverse = True)
    trueList.sort(key = lambda tup: tup[1], reverse = True)

    appTop = set([tup[0] for tup in appList[:K]])
    trueTop = set([tup[0] for tup in trueList[:K]])

#    print appTop
#    print trueTop
    cor = len(appTop.intersection(trueTop))
    return float(cor) / K
    


if len(argv) != 6:
    print 'Usage: %s [Graph-File] [METIS-File] [BlockID] [Exppand] [PPR-Dir]' % (argv[0])
    sys.exit(1)

graphFile = open(argv[1], 'r')
metisFile = open(argv[2], 'r')
blockID = int(argv[3])
expand = int(argv[4])
pprDir = argv[5]

(numVtx, blocks) = ReadBlocks(metisFile)
eblock = GetExtendedBlock(blocks[blockID], graphFile, expand)
print 'Extended Block Size: %d' % (len(eblock))

pprs = ReadPPRs(pprDir, blocks[blockID])

print 'Num vectors: %d' % (len(pprs))
sys.stdout.flush()

#   Generate the filter
mask = []
for i in range(numVtx):
    if i not in eblock:
        mask.append(i)

mat = np.vstack(ZeroifyBlock(pprs, mask))
print mat.shape
(mm, nn) = mat.shape

(U, s, Vh) = linalg.svd(mat, full_matrices = False)
print U.shape, s.shape, Vh.shape

print 'Evaluate the approximation!'
approx = np.zeros(mat.shape)
for idx in eblock:
    for i in range(mm):
        approx[i, idx] = pprs[i][idx]

sumprec = 0.0
for i in range(mm):
    sumprec += EvaluatePrec(approx[i, :], pprs[i], 100)

fullmat = np.vstack(pprs)
print 'Frob\t%d\t%.10f' % (0, np.linalg.norm(fullmat - approx, 'fro'))
print 'Prec\t%d\t%.3f' % (0, sumprec / mm)
sys.stdout.flush()
for i in range(len(s)):
    sumprec = 0.0
    for j in range(mm):
        row = s[i] * U[j, i] * Vh[i, :]
        approx[j] += row
        sumprec += EvaluatePrec(approx[j, :], pprs[j], 100)
#    comp = s[i] * U[:, i] * Vh[i, :]
#    approx += comp
    print 'Frob\t%d\t%.10f' % (i + 1, np.linalg.norm(fullmat - approx, 'fro'))
    print 'Prec\t%d\t%.3f' % (i + 1, sumprec / mm)
    sys.stdout.flush()

graphFile.close()
metisFile.close()

