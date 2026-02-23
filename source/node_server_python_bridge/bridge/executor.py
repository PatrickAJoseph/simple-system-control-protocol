import socket
import json
from processor import process_command

import sys
sys.path.append("../../python")

import initiator

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
        
        result = ""
        type = command["type"]

        if type and type == "set":
            print("Processing set command")
            reg = command["register"]
            param = command["parameter"]
            value = command["value"]
            result = process_command(type, reg, param, value)
        elif type and type == "get":
            reg = command["register"]
            param = command["parameter"]
            result = process_command(type, reg, param)
        elif type and type == "connect":
            result = process_command(type, reg = "", param = "", value = 0)

        print("Response of process_command function: {_response}".format(_response = result))

        try:
            json_payload=json.dumps(result).encode()
            conn.sendall(json_payload)
        except:
            print("Error while formatting to JSON format")

        conn.close()

except KeyboardInterrupt:
    print("\nGracefully shutting down Python socket server...")

finally:
    server.close()
    print("Socket closed.")
