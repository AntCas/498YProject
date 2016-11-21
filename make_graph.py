import re,os,sys, numpy as np
from matplotlib import pyplot as plt




def getAttrib( attrib , fs ):
    ys = []
    for fname in fs:
        if "xml" in fname:
            for line in open(fname).readlines():
                if attrib in line:
                    try:
                        t = re.match(".*" + attrib + "=\"(\d+)\".*", line).groups()[0]
                    except:
                        t = re.match(".*" + attrib + "=\"\+(\d+)\.(\d*)..\".*", line).groups()[0]
                    ys.append(float(t))
    return ys

def main():

    plt.subplot(121)
    # Plot throughputs first
    rx0 = getAttrib( 'rxPackets' , map(lambda x: 'rtscts/' + x, os.listdir('./rtscts')) )
    rx1 = getAttrib( 'rxPackets' , map(lambda x: 'no_rtscts/' + x, os.listdir('./no_rtscts')) )
    x = map(lambda x: x*800, range(len(rx0)))
    plt.plot(x,rx0, label="RTS/CTS")
    plt.plot(x,rx1, label="No RTS/CTS")

    plt.title("Throughput")
    plt.xlabel("CBR")
    plt.ylabel("Packets Recvd")
    plt.legend(loc=4)

    # Now plot delays
    plt.subplot(122)
    y0 = getAttrib( 'delaySum' , map(lambda x: 'rtscts/' + x, os.listdir('./rtscts')) )
    y1 = getAttrib( 'delaySum' , map(lambda x: 'no_rtscts/' + x, os.listdir('./no_rtscts')) )
    tx0 = getAttrib( 'txPackets' , map(lambda x: 'rtscts/' + x, os.listdir('./rtscts')) )
    tx1 = getAttrib( 'txPackets' , map(lambda x: 'no_rtscts/' + x, os.listdir('./no_rtscts')) )
    # compute average
    y0 = map(lambda ii: float(y0[ii]) / tx0[ii], range(len(y0)))
    y1 = map(lambda ii: float(y1[ii]) / tx1[ii], range(len(y1)))
    for i in range(len(y0)):
        print("""  %d: 
                      (rx0: %f)
                      (rx1: %f)
                      (y0: %f) (tx0: %f)
                      (y1: %f) (tx1: %f)"""%(i,rx0[i],rx1[i],y0[i],tx0[i],y1[i],tx1[i]))

    plt.plot(x,y0, label="RTS/CTS")
    plt.plot(x,y1, label="No RTS/CTS")

    plt.title("Delay")
    plt.xlabel("CBR")
    plt.ylabel("Delay Sum")
    plt.legend(loc=4)

    plt.show()

main()
