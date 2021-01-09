import socket
import time
s1 = socket.socket(
    socket.AF_INET, socket.SOCK_STREAM)

port = 8006
s1.connect(("127.0.0.1", port))
s1.send(b'GET / HTTP/1.1\r\nHost: localhost:8000\r\nUser-Agent: Go-http-client/1.1\r\n\r\n')
response = s1.recv(port)
s1.send(b'POST / HTTP/1.1\r\nHost: localhost:8000\r\nUser-Agent: Go-http-client/1.1\r\n\r\n')
response = s1.recv(port)
print(response)
