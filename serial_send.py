#!/usr/bin/env python3

import serial
import time
import struct

if __name__ == "__main__":

    uart = serial.Serial("/dev/ttyUSB2", 115200)
    count = 0

    while True:
        try:
            pkt = struct.pack("Ibbbbbbb", count, 0, 1, 2, 3, 4, 5, 6)
            uart.write(pkt)
            count += 1
            print("Sent {} packets".format(count))
            time.sleep(0.01)
        except KeyboardInterrupt:
            break

    uart.close()

