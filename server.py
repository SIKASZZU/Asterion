# UDP
import socket

IP = "0.0.0.0"  # Listen on all interfaces
PORT = 9999
BUFFER_SIZE = 1024

server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind((IP, PORT))

clients = set()

print(f"Server started on {IP}:{PORT}")

while True:
    try:
        data, addr = server_socket.recvfrom(BUFFER_SIZE)
        print(f"Received from {addr}: {data.decode()}")

        # Remember client
        if addr not in clients:
            clients.add(addr)

        # Echo back to all clients
        for client in clients:
            if client != addr:
                server_socket.sendto(data, client)

    except KeyboardInterrupt:
        print("Server shutting down.")
        break
