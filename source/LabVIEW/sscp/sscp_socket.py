import parser
import initiator
import socket
import sys
import time
import psutil

sscpInitiator: initiator

def find_process_using_port(port):
    """
    Returns (pid, process_name) using the given TCP port
    or None if the port is free.
    """
    for conn in psutil.net_connections(kind="tcp"):
        if conn.laddr and conn.laddr.port == port:
            if conn.pid is not None:
                try:
                    proc = psutil.Process(conn.pid)
                    return conn.pid, proc.name()
                except psutil.NoSuchProcess:
                    pass
    return None

def kill_process(pid):
    try:
        proc = psutil.Process(pid)
        proc.terminate()  # graceful stop
        proc.wait(timeout=3)
        print(f"Process {pid} terminated successfully.")
    except psutil.TimeoutExpired:
        print("Graceful terminate failed. Forcing kill...")
        proc.kill()
        print(f"Process {pid} killed.")
    except Exception as e:
        print(f"Error terminating process: {e}")

# Supported commands:
# CONNECT, SET_PARAMETER, GET_PARAMETER, READ_REGISTER, WRITE_REGISTER, DISCONNECT

configFileName = sys.argv[1]

try:
    interfacePort = int(sys.argv[2])
except:
    interfacePort = 3300

try:
    interfaceType = sys.argv[3]
except:
    interfaceType = "SERIAL"

connectionActive = False

with open(configFileName, "r") as configFile:

    try:
        pid = find_process_using_port(interfacePort)
        print(f"PID of previous process using the port {interfacePort}: {pid}")
        kill_process(pid[0])
    except:
        pass

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server_socket.bind(('127.0.0.1', interfacePort))
    server_socket.listen(1)

    print(f"Listening for a connection at 127.0.0.1/{interfacePort}")

    client_socket, client_address = server_socket.accept()

    client_socket.sendall("ACK\r\n".encode('utf-8'))

    print(f"Connection accepted at {client_address}/{client_socket}")

    connectionActive = True
    connectionInterfaceType: initiator.InterfaceType


    while connectionActive == True:
            request = client_socket.recv(1024).decode('utf-8')

            print("Received command:")
            print(request)

            # Connection requests.

            if "CONNECT" in request:

                print("Command received: CONNECT")

                if interfaceType == "SERIAL":

                    print("Interface type: SERIAL")

                    connectionInterfaceType = initiator.InterfaceType.SERIAL

                if interfaceType == "SOCKET":

                    print("Interface type: SOCKET")

                    connectionInterfaceType = initiator.InterfaceType.SOCKET

                if interfaceType == "BLE":

                    print("Interface type: BLE")

                    connectionInterfaceType = initiator.InterfaceType.BLE

                print("Initializing connection ...")

                client_socket.sendall("CONN_INIT\r\n".encode('utf-8'))

                try:
                    sscpInitiator = initiator.initiator(configFileName, connectionInterfaceType)
                    client_socket.sendall("CONN_DONE\r\n".encode('utf-8'))
                    print("Connection initialized !")
                except:
                    client_socket.sendall("CONN_ERR".encode('utf-8'))
                    print("Connection initialization error.")
    
            # Set parameter command.

            if "SET_PARAMETER," in request:

                sections = [item.strip() for item in request.split(",") if item.strip() != ""]

                parameterName = sections[1]
                parameterValue = sections[2]

                print(f"Setting value of parameter {parameterName} to {parameterValue}")

                if( sscpInitiator.set_parameter_value(parameterName, parameterValue) ):
                    print("SET_PARAMETER request done")
                    client_socket.sendall("ACK\r\n".encode('utf-8'))
                else:
                    print("Error on SET_PARAMETER request")
                    client_socket.sendall("ERR\r\n".encode('utf-8'))

            # Set parameter in device command.

            if "SET_PARAMETER_IN_DEVICE" in request:

                sections = [item.strip() for item in request.split(",") if item.strip() != ""]

                parameterName = sections[1]
                parameterValue = sections[2]

                print(f"Setting value of parameter {parameterName} to {parameterValue} in device")

                if( sscpInitiator.set_parameter_value_in_device(parameterName, parameterValue) ):
                    print("SET_PARAMETER_IN_DEVICE request done")
                    client_socket.sendall("ACK\r\n".encode('utf-8'))
                else:
                    print("Error on SET_PARAMETER_IN_DEVICE request")
                    client_socket.sendall("ERR\r\n".encode('utf-8'))

            # Get parameter command.

            if "GET_PARAMETER," in request:

                sections = [item.strip() for item in request.split(",") if item.strip() != ""]

                parameterName = sections[1]
 
                print(f"Getting value of parameter {parameterName}")

                response = sscpInitiator.get_parameter_value(parameterName)

                print(f"Value of parameter: {response}")

                print("Sending response:")
                print(response)

                if( response is not None ):
                    client_socket.sendall((str(response)+"\r\n").encode('utf-8'))
                else:
                    client_socket.sendall("ERR\r\n".encode('utf-8'))

            # Get parameter from device command.

            if "GET_PARAMETER_FROM_DEVICE" in request:

                sections = [item.strip() for item in request.split(",") if item.strip() != ""]

                parameterName = sections[1]

                print(f"Getting value of parameter {parameterName} from device")

                response = sscpInitiator.get_parameter_value_from_device(parameterName)

                print("Response:")
                print(response)

                print("Sending response:")
                print(response)

                if( response is not None ):
                    client_socket.sendall((str(response) + "\r\n").encode('utf-8'))
                else:
                    client_socket.sendall("ERR\r\n".encode('utf-8'))

    client_socket.close()