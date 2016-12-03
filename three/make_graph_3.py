import re,os,sys, numpy as np
from matplotlib import pyplot as plt



def mkX(n):
    #x = map(lambda x: x*200, range(len(rx0)/2))
    #x += ( map(lambda x: x*300*4, range(len(rx0)/2,len(rx0))) )
    """
    x = []
    acc = 0
    for i in range(n):
        if i < n/2:
            acc += 50
        else:
            acc += 50*2
        x.append(acc)
    """
    x = map(lambda x: x*100, range(n))
    return x

def getAttrib( flowID , attrib , fs ):
    ys = []
    fs = sorted(fs, key=lambda s:int(filter( str.isdigit, s)))
    for fname in fs:
        if "xml" in fname:
            for line in open(fname).readlines():
                if attrib in line and 'flowId="' + flowID + '"' in line:
                    try:
                        t = re.match(".*" + attrib + "=\"(\d+)\".*", line).groups()[0]
                    except:
                        t = re.match(".*" + attrib + "=\"\+(\d+)\.(\d*)..\".*", line).groups()[0]
                    ys.append(float(t))
    return ys

from collections import defaultdict

def findAllCBR12():
    fs = os.listdir('three')
    return set(map(lambda x: x.split('_')[0], fs))

def plotWithCBR12(cbr12):
    cbr12 = str(cbr12)
    fs = filter( lambda x: x.split('_')[0] == cbr12, os.listdir('three') )
    three0 = getAttrib('1', 'rxPackets' , map(lambda x: 'three/' + x, fs) )
    three1 = getAttrib('2', 'rxPackets' , map(lambda x: 'three/' + x, fs) )
    three2 = getAttrib('3', 'rxPackets' , map(lambda x: 'three/' + x, fs) )

    plt.subplot(121)
    x = map(lambda x: x*100, range(len(three0)))
    plt.plot(x,three0, label="Three 1")
    x = map(lambda x: x*100, range(len(three1)))
    plt.plot(x,three1, label="Three 2")
    x = map(lambda x: x*100, range(len(three2)))
    plt.plot(x,three2, '--', label="Three Hacked")

    plt.title("Throughput (CBR 1/2 = %s)"%cbr12)
    plt.xlabel("CBR 3")
    plt.ylabel("Packets Recvd")
    plt.legend(loc=4)

    # Now plot delays
    plt.subplot(122)

    three0 = getAttrib('1', 'delaySum' , map(lambda x: 'three/' + x, fs) )
    thrtx0 = getAttrib('1', 'txPackets' , map(lambda x: 'three/' + x, fs) )
    three1 = getAttrib('2', 'delaySum' , map(lambda x: 'three/' + x, fs) )
    three1 = getAttrib('2', 'delaySum' , map(lambda x: 'three/' + x, fs) )
    thrtx2 = getAttrib('3', 'txPackets' , map(lambda x: 'three/' + x, fs) )
    thrtx2 = getAttrib('3', 'txPackets' , map(lambda x: 'three/' + x, fs) )
    three0 = map(lambda ii: float(three0[ii]), range(len(three0)))
    three1 = map(lambda ii: float(three1[ii]), range(len(three1)))
    three2 = map(lambda ii: float(three2[ii]), range(len(three2)))

    x = map(lambda x: x*100, range(len(three0)))
    plt.plot(x,three0, label="Three 1")
    x = map(lambda x: x*100, range(len(three1)))
    plt.plot(x,three1, label="Three 2")
    x = map(lambda x: x*100, range(len(three2)))
    plt.plot(x,three2, '--', label="Three Hacked")

    plt.title("Delay (CBR 1/2 = %s)"%cbr12)
    plt.xlabel("CBR 3")
    plt.ylabel("Delay Sum")
    plt.legend(loc=4)
    plt.gcf().set_size_inches(20,11)
    plt.savefig('three/%s.png'%cbr12, dpi=100)

for cbr12 in findAllCBR12():
    plotWithCBR12(cbr12)
    plt.clf()
