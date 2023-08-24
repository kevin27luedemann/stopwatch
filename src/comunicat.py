#! /usr/bin/python
import numpy as np
import serial as se
import time as ti
import datetime as dt
import sys

def main():
    ser = se.Serial('/dev/ttyUSB0', 9600)
    last = ""
    weather = ""
    while True:
        #now = dt.datetime.now().strftime("%H%M1AA%S1")
        now = dt.datetime.now().strftime("%H%M1")
        if now != last:
            for n in now:
                ser.write(n)
            last = now
        ti.sleep(1)
        ser.write('d')
        taster = ser.read()
        ser.write('w')
        weather = ser.readline()[:-1]
        sys.stdout.write("\r{}, {}, {}".format(now,taster, weather))
        sys.stdout.flush()
        #print now, ser.read()

if __name__ == "__main__":
    main()
