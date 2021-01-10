import socket
import time
s1 = socket.socket(
    socket.AF_INET, socket.SOCK_STREAM)

port = 80
s1.connect(("127.0.0.1", port))
body = (b'POST /post_body HTTP/1.1\r\nHost: localhost:8033\r\nUser-Agent: Go-http-client/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: test/file\r\nAccept-Encoding: gzip\r\n\r\n0\r\n\r\n')
body = body + body
s1.send(body)
response = s1.recv(port)
print(response)
