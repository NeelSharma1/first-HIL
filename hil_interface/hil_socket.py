# hil_socket.py
# Simple TCP server to mock cFS ↔ Trick transport

import socket
import struct

HOST = "localhost"
PORT = 5005

def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print("HIL Interface Listening...")

        conn, addr = s.accept()
        with conn:
            print(f"Connected by {addr}")
            while True:
                data = conn.recv(8)
                if not data:
                    break

                # Unpack double
                position = struct.unpack("d", data)[0]
                print("Received Position:", position)

                # Send back simple command
                command = -position
                conn.sendall(struct.pack("d", command))

if __name__ == "__main__":
    start_server()
