from socket import socket

PACKET_SIZE = 64
server = socket()
server.bind(("127.0.0.1", 5000))
server.listen(1)
while True:
	client, caddr = server.accept()
	print(caddr, " connected")
	r = client.recv(PACKET_SIZE)
	while r:
		print(r.decode(), end="")
		r = client.recv(PACKET_SIZE)

