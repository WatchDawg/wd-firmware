import csv
import serial
import struct
import time

ser = serial.Serial('/dev/ttyUSB0', 9600, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)
#ser = serial.Serial('COM9', 9600, timeout=0, parity=serial.PARITY_NONE, rtscts=1)

with open('coords.txt', newline='') as csvfile:
    readCSV = csv.reader(csvfile, delimiter=',')

    ser.reset_input_buffer()
    ser.reset_output_buffer()

    h = 0x53
    ser.write(struct.pack('>B', h))
    #time.sleep(.1)
    #ser.flush()
##    ser.write((h).to_bytes(1, byteorder="big", signed=True))
##    print((180000).to_bytes(8, byteorder="big", signed=True))


    for row in readCSV:
        ack = ""
        print('WAITING FOR ACK')
        while "A" not in ack:
            ack += str(ser.read())
            time.sleep(.1)
        print(ack)
        lat = int(float(row[0])*(10**7))
        long = int(float(row[1])*(10**7))

        ser.write(struct.pack('>i', lat))
        ack = ""
        print('WAITING FOR ACK')
        while "A" not in ack:
            ack += str(ser.read())
        print(ack)
        ser.write(struct.pack('>i', long))
##        ser.write((long).to_bytes(4, byteorder="big", signed=True))
    csvfile.close()

    h = 0x45
    ser.write((h).to_bytes(1, byteorder="big", signed=True))

ser.close()
