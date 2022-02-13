import socket
import os

UDP_IP = "192.168.1.177"
UDP_PORT = 8888

while not("Received = 4" in os.popen(f"ping {UDP_IP}").read()):
    pass

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind(('',UDP_PORT))
sock.connect((UDP_IP,UDP_PORT))
sock.settimeout(5)
print("Socket connected")

while True:
    try:
        sock.sendto("message from python".encode('utf-8'),(UDP_IP,UDP_PORT))
        data, addr = sock.recvfrom(500) # buffer size is 1024 bytes
        print("received message: %s" % data)
    except socket.error:
        print("Connection timed out")