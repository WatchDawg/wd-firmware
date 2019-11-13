import csv
import serial
import serial.tools.list_ports
import struct
import time

ports = list(serial.tools.list_ports.comports())
for p in ports:
    if 'USB Serial Port' in p.description:
        print('Found FTDI USB-Serial Bridge')
        print(p)
        break;

#ser = serial.Serial('COM12', 9600, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)
ser = serial.Serial(p.device, 9600, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)

with open('coords.txt', newline='') as csvfile:
    readCSV = csv.reader(csvfile, delimiter=',')

    ser.reset_input_buffer()
    ser.reset_output_buffer()

    h = 0xFE
    ser.write(struct.pack('>B', h))
    time.sleep(.1)


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
        
    csvfile.close()

    h = 0xFF
    ser.write(struct.pack('>B', h))

ser.close()
