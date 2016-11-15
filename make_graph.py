import re,sys, numpy as np
from matplotlib import pyplot as plt


data = []
xs,ys = [],[]

for line in sys.stdin.readlines():
    if "^" in line:
        seqn,t = re.match("(\d+)\^\+(\d+\.\d+)ms",line).groups()
        #data.append( (seqn,t) )
        xs.append(seqn)
        ys.append(t)

#plt.scatter(data)
plt.plot(xs,ys, '-')
plt.show()

