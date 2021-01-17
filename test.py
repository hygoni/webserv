import socket
import time

port = 8080
body = (b'GET / HTTP/1.1\r\nHost: localhost:8033\r\n\r\n')


s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s1.connect(("localhost", port))
body = 'POST /directory/youpi.bla HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Go-http-client/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: test/file\r\nAccept-Encoding: gzip\r\n\r\n'
size = 10000
body += (hex(size)[2:] + '\r\n' + 'A' * size + '\r\n0\r\n\r\n')

# n_socket, count
# 3, 4 -> select failed
# 3, 5 -> substr exception
# what...??? wtf!

n_socket = 20
count = 5

sockets = []
for i in range(n_socket):
  print('creating ...' + str(i))
  sock = socket.socket(
      socket.AF_INET, socket.SOCK_STREAM)
  sockets.append(sock)
  sock.connect(('localhost', port))

for i in range(count):
  print('sending ...' + str(i))
  for j in range(n_socket):
    time.sleep(0.5)
    sock = sockets[j]
    print('sending... ' + body)
    sock.send(body)
    response = sock.recv(port)
    print('recv... ' + response)

'''
port = 8080
s1.connect(("127.0.0.1", port))
body = (b'POST /post_body HTTP/1.1\r\nHost: localhost:8033\r\nUser-Agent: Go-http-client/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: test/file\r\nAccept-Encoding: gzip\r\n\r\n0\r\n\r\n')
body = body + body
s1.send(body)
response = s1.recv(port)
print(response)
response = s1.recv(port)
print(response)
'''
