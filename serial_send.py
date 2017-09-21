#!/usr/bin/env python3

import serial
import time
import struct

def stop():
    while True:
        time.sleep(1)

if __name__ == "__main__":

    uart = serial.Serial("/dev/ttyUSB2", 115200)
    count = 0

    while True:
        try:
            pkt = struct.pack("Ibbbbbbb", count, 0, 1, 2, 3, 4, 5, 6)
            uart.write(pkt)
            print("Sent {} packets".format(count))

            resp = uart.read(33)
            for i in range(0, 33, 11):
                rcount, b0, b1, b2, b3, b4, b5, b6 = struct.unpack("Ibbbbbbb", resp[i:i+11])
                if rcount != count:
                    print("Count incorrect at index {}, was {}, should be {}".format(i/11, rcount, count))
                    stop()
                if b0 != 0:
                    print("Byte 0 incorrect at index {}, was {}, should be 0".format(i/11, b0))
                    stop()
                if b1 != 1:
                    print("Byte 1 incorrect at index {}, was {}, should be 1".format(i/11, b1))
                    stop()
                if b2 != 2:
                    print("Byte 2 incorrect at index {}, was {}, should be 2".format(i/11, b2))
                    stop()
                if b3 != 3:
                    print("Byte 3 incorrect at index {}, was {}, should be 3".format(i/11, b3))
                    stop()
                if b4 != 4:
                    print("Byte 4 incorrect at index {}, was {}, should be 4".format(i/11, b4))
                    stop()
                if b5 != 5:
                    print("Byte 5 incorrect at index {}, was {}, should be 5".format(i/11, b5))
                    stop()
                if b6 != 6:
                    print("Byte 6 incorrect at index {}, was {}, should be 6".format(i/11, b6))
                    stop()

            count += 1

        except KeyboardInterrupt:
            break

    uart.close()

