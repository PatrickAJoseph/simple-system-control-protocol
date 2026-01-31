
import packet
import parser
import socket

from enum import Enum
from typing import IO

class InterfaceType(Enum):
    SOCKET = 0
    SERIAL = 1
    BLE    = 2

class responder:
    
    interface: InterfaceType
    device: parser.device
    file: str
    base_port: int
    iwrr_port: int
    irrw_port: int
    host: str
    iwrr_socket: socket.socket
    irrw_listen_socket: socket.socket
    irrw_socket: socket.socket
    iwrr_address: tuple
    irrw_address: tuple

    def __init__(self, file:str, interface: InterfaceType):

        self.file = file
        self.interface = interface

        self.device = parser.device(self.file)
        self.callback_table = []

        self.host = "127.0.0.1"
        self.base_port = self.device.baseport
        self.iwrr_port = self.base_port + 2*self.device.number
        self.irrw_port = self.base_port + 2*self.device.number + 1

        if( interface == InterfaceType.SOCKET ):

            # Initiator read responder write socket.
            # Initiator is the client.
            # Responder is the socket server.

            print("Responder: Opening a responder socket client @ port {_port} and IP address {_ip}".format( _port = self.iwrr_port, _ip = self.host))

            self.iwrr_socket = socket.socket()
            self.iwrr_socket.connect( ( self.host, self.iwrr_port ) )

            print("Responder: Connected to responder socket client @ port {_port} and IP address {_ip}".format( _port = self.iwrr_port, _ip = self.host))

            print("Responder: Opening a responder socket server @ port {_port}".format( _port = self.irrw_port))

            self.irrw_listen_socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

            print("Responder: Binding a responder socket server @ port {_port} and IP address {_ip}".format( _port = self.iwrr_port, _ip = self.host))

            self.irrw_listen_socket.bind( ( self.host, self.irrw_port ) )

            print("Responder: Done binding a responder socket server @ port {_port} and IP address {_ip}".format( _port = self.iwrr_port, _ip = self.host))

            # Initiator write responder read.
            # Initiator is the socket server.
            # Responder is the socket client.

            print("Responder: Listening for a responder socket server connection @ port {_port} and IP address {_ip}".format( _port = self.iwrr_port, _ip = self.host))

            self.irrw_listen_socket.listen()

            # Wait for a responder to connect to this port.

            (self.irrw_socket, self.irrw_address) = self.irrw_listen_socket.accept()

            print("Responder: Accepted a connection at responder socket server @ port {_port} and IP address {_ip}".format( _port = self.iwrr_port, _ip = self.host))


    def add_register_handle(self, reg: int, handle):

        is_unique = True

        for callback_entry in self.callback_table:

            try:
                if( callback_entry[0] == reg ):
                    is_unique = False
                    break
            except:
                break
        
        if(is_unique):
            self.callback_table.append( ( reg, handle ) )
    
    def stringify_packet(self, packet):

        s_packet = b''

        for p in packet:

            x = p.to_bytes(1)
            s_packet = s_packet + x

        return s_packet

    def unstringify_packet(self, packet: bytes):

        p = []

        for _p in packet:

            p.append(int(_p))
        
        return p

    def recv_exact(self, sock, size):
        data = b""
        while len(data) < size:
            chunk = sock.recv(size - len(data))
            if not chunk:
                raise ConnectionError("Socket closed")
            data += chunk
        return data

    def process(self):

        if( self.interface == InterfaceType.SOCKET ):

            try:
                request = self.recv_exact( self.iwrr_socket, 16)
            except:

                print("Responder: socket: Connection closed by initiator")

                return False

            if request == b'':
                print("Responder: socket: Connection closed by initiator")
                return False

            request_packet = packet.packet()
            result = request_packet.decode( self.unstringify_packet(request) )

            print("Responder: socket: Received a request from initiator: {_packet}".format( _packet = self.stringify_packet( request_packet.bytes ) ))
            
            if(result):
                print("Responder: socket: No errors in received packet.")
            else:
                print("Responder: socket: Error in received packet.")

            request_packet.info()

            # Handle read operation.

            if( request_packet.read and result ):

                reg_name = self.device.registers[request_packet.reg].name

                print("Responder: socket: Processing read request for register ID {_regid} and name {_regname}".format( _regid = request_packet.reg, _regname = reg_name ))

                reg_value = self.device.registers[request_packet.reg].value

                try:

                    print("Responder: socket: Calling user provided callback function.")

                    self.callback_table[request_packet.reg][1]( self, self.device.registers[request_packet.reg].name, request_packet)

                    print("Responder: socket: User provided callback function is called.")

                except:

                    print("Responder: socket: No callback function provided for register")

                    pass

                response_packet = packet.packet()

                print("Responder: socket: Encoding response packet")

                response_packet.encode( device_id = request_packet.device_id, ack = True, read = True, write = False, reg = request_packet.reg, value = self.device.registers[request_packet.reg].value )

                print("Responder: socket: Encoded response packet information")

                response_packet.info()

                string_response_packet = self.stringify_packet(response_packet.bytes)

                try:

                    result = True

                    self.irrw_socket.sendall(string_response_packet)

                except:

                    result = False

            # Handle write operation.

            if( request_packet.write and result ):

                reg_name = self.device.registers[request_packet.reg].name

                self.device.registers[request_packet.reg].value = request_packet.value

                print("Responder: socket: Processing write request for register ID {_regid} and name {_regname}".format( _regid = request_packet.reg, _regname = reg_name ))

                try:

                    print("Responder: socket: Calling user provided callback function.")

                    self.callback_table[request_packet.reg][1]( self, self.device.registers[request_packet.reg].name, request_packet)

                    print("Responder: socket: User provided callback function is called.")

                except:

                    print("Responder: socket: No callback function provided for register")

                    pass

                response_packet = packet.packet()

                print("Responder: socket: Encoding response packet")

                response_packet.encode( device_id = request_packet.device_id, ack = True, read = False, write = True, reg = request_packet.reg, value = self.device.registers[request_packet.reg].value )

                print("Responder: socket: Encoded response packet information")

                response_packet.info()

                string_response_packet = self.stringify_packet(response_packet.bytes)

                try:

                    result = True

                    self.irrw_socket.sendall(string_response_packet)

                except:

                    result = False

        return result


def reg_0_callback(r: responder, register_name: str, packet: packet.packet):
    
    if( packet.read ):

        print("Register 0 read request received")

    if( packet.write ):

        print("Register 0 write request received")

    print("Register 0 callback called")

def reg_1_callback(r: responder, register_name: str, packet: packet.packet):
    
    if( packet.read ):

        print("Register 1 read request received")

    if( packet.write ):

        print("Register 1 write request received")

    print("Register 1 callback called")


x = responder('test.yaml', InterfaceType.SOCKET)

x.add_register_handle(0, reg_0_callback)
x.add_register_handle(1, reg_1_callback)

while x.process() == True:
    pass