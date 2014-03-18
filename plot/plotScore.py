#   Plot the PageRank Score
#   Adapted fro text files :)

import sys
from sys import stdout
import matplotlib.pyplot as plt
import numpy as np
from math import exp

if len(sys.argv) != 2:
    stdout.write('Usage: %s [RankFile]\n' % (sys.argv[0]))
    sys.exit(1)

f = open(sys.argv[1], 'r')
lines = f.readlines()
rr = [float(l) for l in lines]
rr.sort()
rr = rr[::-1]   #   Reverse


#stdout.write('INFO: Non-zero members: %d\n' % (nonzero))
stdout.write('INFO: Sum of all the score: %.4lf\n' % (sum(rr)))
stdout.write('INFO: Sum of all the score^2: %.4lf\n' % (sum([x * x for x in rr])))

mylen = min(1000000, len(rr))
rr = rr[:mylen]
stdout.write('INFO: Sum of score for the top 1 million vertices: %.4lf\n' % (sum(rr)))
stdout.write('INFO: Sum of score for the top 100 vertices: %.4le, value for 100th score: %.4le\n' % (sum(rr[:100]), rr[99]))
stdout.write('INFO: Sum of score^2 for the top 100 vertices: %.4lf.\n' % (sum([x * x for x in rr[:100]])))

stdout.write('INFO: Sum of score for the top 500 vertices: %.4lf, value for 500th score: %.4le\n' % (sum(rr[:500]), rr[499]))

stdout.write('INFO: Sum of score for the top 1000 vertices: %.4lf, value for 1000th score: %.4le\n' % (sum(rr[:1000]), rr[999]))

idx = []
# Those indices are only using for fit; to get away about the bias at the beginning...
cidx = 100
while True:
    if rr[cidx] == 0:
        break
    idx.append(cidx)
    cidx = int(cidx * 1.5)
    if cidx > len(rr):
        break

logrr = np.log([rr[i - 1] for i in idx])
logcc = np.log(idx)
zz  = np.polyfit(logcc, logrr, 1)
print zz


#   1 million points are good enough
step = (len(rr) + 999999) / 1000000
cc = range(1, len(rr) + 1, step)
rr = rr[::step]
fitrr = np.power(cc, zz[0]) * exp(zz[1])

plt.loglog(cc, rr, '+')
plt.loglog(cc, fitrr, '-')

plt.xlabel('ith ranked vertex')
plt.ylabel('PageRank Value')


f.close()

plt.show()
