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
    x = map(lambda x: x*150, range(n))
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

def part():
    threedir = 'three/'
    three_h_dir = 'three_hacked/'
    three0 = getAttrib('1', 'rxPackets' , map(lambda x: threedir + x, os.listdir(threedir)) )
    three1 = getAttrib('2', 'rxPackets' , map(lambda x: threedir + x, os.listdir(threedir)) )
    three_h0 = getAttrib('1', 'rxPackets' , map(lambda x: three_h_dir + x, os.listdir(three_h_dir)) )
    three_h1 = getAttrib('2', 'rxPackets' , map(lambda x: three_h_dir + x, os.listdir(three_h_dir)) )

    x = mkX(len(three0))

    plt.subplot(121)
    plt.plot(x,three0, label="Three 1")
    plt.plot(x,three1, label="Three 2")

    plt.plot(x,three_h0, '--', label="Three Hacked 1")
    plt.plot(x,three_h1, '--', label="Three Hacked 2")

    plt.title("Throughput")
    plt.xlabel("CBR")
    plt.ylabel("Packets Recvd")
    plt.legend(loc=4)

    # Now plot delays
    plt.subplot(122)
    three0 = getAttrib('1', 'delaySum' , map(lambda x: threedir + x, os.listdir(threedir)) )
    three_tx0 = getAttrib('1', 'txPackets' , map(lambda x: threedir + x, os.listdir(threedir)) )
    three1 = getAttrib('2', 'delaySum' , map(lambda x: threedir + x, os.listdir(threedir)) )
    three_tx1 = getAttrib('2', 'txPackets' , map(lambda x: threedir + x, os.listdir(threedir)) )

    three_h0 = getAttrib('1', 'delaySum' , map(lambda x: three_h_dir + x, os.listdir(three_h_dir)) )
    three_h_tx0 = getAttrib('1', 'txPackets' , map(lambda x: three_h_dir + x, os.listdir(three_h_dir)) )
    three_h1 = getAttrib('2', 'delaySum' , map(lambda x: three_h_dir + x, os.listdir(three_h_dir)) )
    three_h_tx1 = getAttrib('2', 'txPackets' , map(lambda x: three_h_dir + x, os.listdir(three_h_dir)) )
     # compute average
    #three0 = map(lambda ii: float(three0[ii]) / three_tx0[ii], range(len(three0)))
    #three1 = map(lambda ii: float(three1[ii]) / three_tx1[ii], range(len(three1)))
    #three_h0 = map(lambda ii: float(three_h0[ii]) / three_h_tx0[ii], range(len(three_h0)))
    #three_h1 = map(lambda ii: float(three_h1[ii]) / three_h_tx1[ii], range(len(three_h1)))
    three0 = map(lambda ii: float(three0[ii]), range(len(three0)))
    three1 = map(lambda ii: float(three1[ii]), range(len(three1)))
    three_h0 = map(lambda ii: float(three_h0[ii]), range(len(three_h0)))
    three_h1 = map(lambda ii: float(three_h1[ii]), range(len(three_h1)))

    plt.plot(x,three0, label="Three 1")
    plt.plot(x,three1, label="Three 2")
    plt.plot(x,three_h0, '--', label="Three Hacked 1")
    plt.plot(x,three_h1, '--', label="Three Hacked 2")

    plt.title("Delay")
    plt.xlabel("CBR")
    plt.ylabel("Delay Sum")
    plt.legend(loc=4)

part()
plt.show()
