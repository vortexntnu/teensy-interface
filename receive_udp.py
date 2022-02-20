import socket
import os

UDP_IP = "10.0.0.2"
UDP_PORT = 8888

REMOTE_IP = "129.241.187.177"

LOCALHOST_IP = "127.0.0.1"

''' while not("Received = 4" in os.popen(f"ping {REMOTE_IP}").read()):
    pass '''

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP,UDP_PORT))
print("Socket connected")

while True:
    data, addr = sock.recvfrom(500) # buffer size is 1024 bytes
    print("received message: %s" % data)
    