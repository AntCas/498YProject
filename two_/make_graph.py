import re,os,sys, numpy as np
from matplotlib import pyplot as plt



def mkX(n):
    #x = map(lambda x: x*200, range(len(rx0)/2))
    #x += ( map(lambda x: x*300*4, range(len(rx0)/2,len(rx0))) )
    #x = map(lambda x: x*30, range(n/2))
    #x += ( map(lambda x: x*30*4, range(n/2,n)) )
    #x = range(n)
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
    rtsdir = 'rtscts/'
    nortsdir = 'no_rtscts/'
    plt.subplot(121)
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
    plt.plot(x,map(lambda xy: xy[0]+xy[1], zip(norts0,norts1)), label="Average NO RTS")
    plt.plot(x,map(lambda xy: xy[0]+xy[1], zip(rts0,rts1)), label="Average RTS")

    plt.title("Throughput")
    plt.xlabel("CBR")
    plt.ylabel("Packets Recvd")
    plt.legend(loc=4)

    # Now plot delays
    plt.subplot(122)
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
    #for i in range(len(y0)):
    #    print("""  %d: 
    #                  (rx0: %f)
    #                  (rx1: %f)
    #                  (y0: %f) (tx0: %f)
    #                  (y1: %f) (tx1: %f)"""%(i,rx0[i],rx1[i],y0[i],tx0[i],y1[i],tx1[i]))

    plt.plot(x,rts0, label="RTS/CTS 1")
    plt.plot(x,norts0, label="No RTS/CTS 1")
    plt.plot(x,rts1, '--',label="RTS/CTS 2")
    plt.plot(x,norts1, '--',label="No RTS/CTS 2")

    plt.title("Delay")
    plt.xlabel("CBR")
    plt.ylabel("Delay Sum")
    plt.legend(loc=4)
    plt.gcf().set_size_inches(20,11)
    plt.savefig('TWO.png', dpi=100)

part()
