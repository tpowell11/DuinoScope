#This program writes and reads to a gpio serial connection between the pi and the arduino
#currently it just has comms and no stellarium integration, but that will come soon
import serial
ser = serial.Serial(‘/dev/ttyAMA0’, 9600, timeout=1)
ser.open()

ser.write(“testing”)
try:
while 1:
response = ser.readline()
print response
except KeyboardInterrupt:
ser.close()