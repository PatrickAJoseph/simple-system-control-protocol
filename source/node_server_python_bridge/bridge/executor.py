import socket
import json
from processor import process_command


HOST = "127.0.0.1"
PORT = 5000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind((HOST, PORT))
server.listen()

server.settimeout(1.0)  # seconds

print("Python executor running... Press Ctrl+C to stop.")

try:
    while True:
        try:
            conn, addr = server.accept()
        except socket.timeout:
            continue   # loop again, allows Ctrl+C to be processed

        data = conn.recv(2048)
        if not data:
            conn.close()
            continue

        command = json.loads(data.decode())
        print("Got command: ", command, " for execution")
        result = process_command(command)

        conn.sendall(json.dumps(result).encode())
        conn.close()

except KeyboardInterrupt:
    print("\nGracefully shutting down Python socket server...")

finally:
    server.close()
    print("Socket closed.")
