import serial

ser = serial.Serial("COM7", baudrate=9600, timeout=0.5)
# ser_bytes = ser.readline()
# print(ser_bytes)

ser.flushInput()

while True:
    try:
        ser_bytes = ser.readline().decode("utf-8").rstrip()
        # decoded_bytes = float(ser_bytes[0 : len(ser_bytes) - 2].decode("utf-8"))
        print(ser_bytes)
    except:
        print("Keyboard Interrupt")
        break
