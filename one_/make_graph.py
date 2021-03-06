import re,os,sys, numpy as np
from matplotlib import pyplot as plt



def mkX(n):
    #x = map(lambda x: x*200, range(len(rx0)/2))
    #x += ( map(lambda x: x*300*4, range(len(rx0)/2,len(rx0))) )
    #x = map(lambda x: x*30, range(n/2))
    #x += ( map(lambda x: x*30*4, range(n/2,n)) )
    #x = range(n)
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

def partA():
    rtsdir = 'a_rtscts/'
    nortsdir = 'a_no_rtscts/'
    plt.subplot(221)
    # Plot throughputs first
    rx0 = getAttrib('1', 'rxPackets' , map(lambda x: rtsdir + x, os.listdir(rtsdir)) )
    rx1 = getAttrib('1', 'rxPackets' , map(lambda x: nortsdir + x, os.listdir(nortsdir)) )

    x = mkX(len(rx0))

    plt.plot(x,rx0, label="RTS/CTS")
    plt.plot(x,rx1, '--', label="No RTS/CTS")

    plt.title("Throughput")
    plt.xlabel("CBR")
    plt.ylabel("Packets Recvd")
    plt.legend(loc=4)

    # Now plot delays
    plt.subplot(222)
    y0 = getAttrib('1', 'delaySum' , map(lambda x: rtsdir + x, os.listdir(rtsdir)) )
    y1 = getAttrib('1', 'delaySum' , map(lambda x: nortsdir + x, os.listdir(nortsdir)) )
    tx0 = getAttrib('1', 'txPackets' , map(lambda x: rtsdir + x, os.listdir(rtsdir)) )
    tx1 = getAttrib('1', 'txPackets' , map(lambda x: nortsdir + x, os.listdir(nortsdir)) )
    # compute average
    y0 = map(lambda ii: float(y0[ii]) / tx0[ii], range(len(y0)))
    y1 = map(lambda ii: float(y1[ii]) / tx1[ii], range(len(y1)))

    plt.plot(x,y0, label="RTS/CTS")
    plt.plot(x,y1, '--',label="No RTS/CTS")

    plt.title("Delay")
    plt.xlabel("CBR")
    plt.ylabel("Delay Sum")
    plt.legend(loc=4)


def partB():
    rtsdir = 'b_rtscts/'
    nortsdir = 'b_no_rtscts/'
    plt.subplot(223)
    # Plot throughputs first
    rts0 = getAttrib('1', 'rxPackets' , map(lambda x: rtsdir + x, os.listdir(rtsdir)) )
    norts0 = getAttrib('1', 'rxPackets' , map(lambda x: nortsdir + x, os.listdir(nortsdir)) )
    rts1 = getAttrib('2', 'rxPackets' , map(lambda x: rtsdir + x, os.listdir(rtsdir)) )
    norts1 = getAttrib('2', 'rxPackets' , map(lambda x: nortsdir + x, os.listdir(nortsdir)) )

    x = mkX(len(rts0))

    plt.plot(x,rts0, label="RTS/CTS 1")
    plt.plot(x,norts0, label="No RTS/CTS 1", )
    plt.plot(x,rts1, '--',label="RTS/CTS 2")
    plt.plot(x,norts1, '--',label="No RTS/CTS 2", )

    plt.title("Throughput")
    plt.xlabel("CBR")
    plt.ylabel("Packets Recvd")
    plt.legend(loc=4)

    # Now plot delays
    plt.subplot(224)
    rts0 = getAttrib('1', 'delaySum' , map(lambda x: rtsdir + x, os.listdir(rtsdir)) )
    norts0 = getAttrib('1', 'delaySum' , map(lambda x: nortsdir + x, os.listdir(nortsdir)) )
    rts1 = getAttrib('2', 'delaySum' , map(lambda x: rtsdir + x, os.listdir(rtsdir)) )
    norts1 = getAttrib('2', 'delaySum' , map(lambda x: nortsdir + x, os.listdir(nortsdir)) )
    rts_tx0 = getAttrib('1', 'txPackets' , map(lambda x: rtsdir + x, os.listdir(rtsdir)) )
    norts_tx0 = getAttrib('1', 'txPackets' , map(lambda x: nortsdir + x, os.listdir(nortsdir)) )
    rts_tx1 = getAttrib('2', 'txPackets' , map(lambda x: rtsdir + x, os.listdir(rtsdir)) )
    norts_tx1 = getAttrib('2', 'txPackets' , map(lambda x: nortsdir + x, os.listdir(nortsdir)) )
    # compute average
    rts0 = map(lambda ii: float(rts0[ii]) / rts_tx0[ii], range(len(rts0)))
    norts0 = map(lambda ii: float(norts0[ii]) / norts_tx0[ii], range(len(norts0)))
    rts1 = map(lambda ii: float(rts1[ii]) / rts_tx1[ii], range(len(rts1)))
    norts1 = map(lambda ii: float(norts1[ii]) / norts_tx1[ii], range(len(norts1)))

    plt.plot(x,rts0, label="RTS/CTS 1")
    plt.plot(x,norts0, label="No RTS/CTS 1")
    plt.plot(x,rts1, '--',label="RTS/CTS 2")
    plt.plot(x,norts1, '--',label="No RTS/CTS 2")

    plt.title("Delay")
    plt.xlabel("CBR")
    plt.ylabel("Delay Sum")
    plt.legend(loc=4)
    plt.gcf().set_size_inches(20,11)
    plt.savefig('ONE.png', dpi=100)


partA()
partB()
