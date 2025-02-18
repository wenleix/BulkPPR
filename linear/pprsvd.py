#   Take in the PPR vectors for a block of vertices, and see their SVD result.

import sys
from sys import argv
import numpy as np
from scipy import linalg

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
def ReadPPRs(dirname, vidList, mask):
    pprs = []
    for vid in vidList:
        fname = '%s/ppr_%d.txt' % (dirname, vid)
        pprFile = open(fname, 'r')
        lines = pprFile.readlines()
        pprvector = np.array([float(l) for l in lines])
        pprFile.close()

        pprs.append(pprvector[mask])

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

#   Generate the filter
mask = []
for i in range(numVtx):
    if i not in eblock:
        mask.append(i)

pprs = ReadPPRs(pprDir, blocks[blockID], mask)

print 'Num vectors: %d' % (len(pprs))
mat = np.vstack(pprs)
print mat.shape

s = linalg.svd(mat, compute_uv = False)
print s.shape

for i in range(len(s)):
    print s[i]

graphFile.close()
metisFile.close()

