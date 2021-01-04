import socket

s1 = socket.socket(
    socket.AF_INET, socket.SOCK_STREAM)

port = 8055
s1.connect(("127.0.0.1", port))
s1.send(b'POST /directory/youpi.bla HTTP/1.1\r\nHost: localhost:8049\r\nUser-Agent: Go-http-client/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: test/file\r\nAccept-Encoding: gzip\r\n\r\n5\r\nabcde\r\n0\r\n\r\n')
response = s1.recv(port)
print(response)

s2 = socket.socket(
    socket.AF_INET, socket.SOCK_STREAM)
s2.connect(("127.0.0.1", port))
s2.send(b'POST /post_body HTTP/1.1\r\nHost: localhost:8049\r\nUser-Agent: Go-http-client/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: test/file\r\nAccept-Encoding: gzip\r\n\r\n0\r\n\r\n')
response = s2.recv(port)
print(response)