import socket

TEENSY_IP = "129.241.187.177"
UDP_PORT = 8888
MESSAGE = b"1"

print("UDP target IP: %s" % TEENSY_IP)
print("UDP target port: %s" % UDP_PORT)
print("message: %s" % MESSAGE)

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE, (TEENSY_IP, UDP_PORT))