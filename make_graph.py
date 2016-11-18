import re,sys, numpy as np
from matplotlib import pyplot as plt


data = []
xs,ys = [],[]

fs = sys.argv[1:]
print(fs)

for fname in fs:
    if "xml" in fname:
        for line in open(fname).readlines():
            if "rxPackets" in line:
                t = re.match(".*rxPackets=\"(\d+)\".*", line).groups()[0]
                ys.append(t)

#plt.scatter(data)
#plt.plot(xs,ys, '-')
print(ys)
plt.plot(ys)
plt.show()

