
import packet
import parser
import socket

from enum import Enum
from typing import IO

class InterfaceType(Enum):
    SOCKET = 0
    SERIAL = 1
    BLE    = 2

class initiator:
    
    interface: InterfaceType
    device: parser.device
    file: str
    base_port: int
    iwrr_port: int
    irrw_port: int
    host: str
    iwrr_socket: socket.socket
    iwrr_listen_socket: socket.socket
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
            
            print("Initiator: Opening a initiator socket server @ port {_port} and IP address {_ip}".format(_port = self.iwrr_port, _ip = self.host))

            self.iwrr_listen_socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM )

            self.iwrr_listen_socket.bind( ( self.host, self.iwrr_port ) )

            print("Initiator: Completed binding initiator socket server @ port {_port}, IP address {_ip}".format(_port = self.iwrr_port, _ip = self.host))

            # Initiator write responder read.
            # Initiator is the socket server.
            # Responder is the socket client.

            print("Initiator: Started listening to incoming connections @ port {_port}, IP address {_ip}".format(_port = self.iwrr_port, _ip = self.host))

            self.iwrr_listen_socket.listen()

            print("Initiator: Got a connection request @ port {_port}, IP address {_ip}".format(_port = self.iwrr_port, _ip = self.host))

            # Wait for a responder to connect to this port.

            (self.iwrr_socket, self.irrw_address) = self.iwrr_listen_socket.accept()

            print("Initiator: Accepted a connection @ port {_port}, IP address {_ip}".format(_port = self.iwrr_port, _ip = self.host))

            # Initiator read responder write socket.
            # Initiator is the client.
            # Responder is the socket server.

            print("Initiator: Opening a initiator socket client @ port {_port}".format(_port = self.irrw_port))

            self.irrw_socket = socket.socket()
            self.irrw_socket.connect( ( self.host, self.irrw_port ) )

            print("Initiator: Connected to a initiator socket client @ port {_port}".format(_port = self.irrw_port))


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

    def read_register(self, name: str):

        reg = self.device.get_register(name)

        p = packet.packet()
        p.encode( device_id = self.device.number, ack = False, read = True, write = False, reg = reg.number, value = 0 )

        if( self.interface == InterfaceType.SOCKET ):

            print("Initiator: socket: Posting read request for register: {_name}".format(_name = name))

            self.iwrr_socket.sendall(self.stringify_packet(p.bytes))

            print("Initiator: socket: Requested for register: {_name}".format(_name = name))

            #response = self.irrw_socket.recv(16)

            response = self.recv_exact( self.irrw_socket, 16 )

            print("Initiator: socket: Received response : {_response}".format(_response = self.stringify_packet(response)))

            print("Initiator: socket: Response packet information.")

            response_packet = packet.packet()

            ret = response_packet.decode(self.unstringify_packet(response))

            response_packet.info()

            if( ret == True ):

                print("Initiator: socket: Response packet valid.")

                self.device.registers[response_packet.reg].set(response_packet.value)
            else:

                print("Initiator: socket: Response packet invalid.")

                response_packet.info()

                pass

    def write_register(self, name: str, _value: int ):

        reg = self.device.get_register(name)

        p = packet.packet()
        p.encode( device_id = self.device.number, ack = False, read = False, write = True, reg = reg.number, value = _value )

        if( self.interface == InterfaceType.SOCKET ):

            print("Initiator: socket: Posting write request for register: {_name}".format(_name = name))

            self.iwrr_socket.sendall(self.stringify_packet(p.bytes))

            print("Initiator: socket: Requested for register: {_name}".format(_name = name))

            response = self.recv_exact( self.irrw_socket, 16 )

            print("Initiator: socket: Received response : {_response}".format(_response = self.stringify_packet(response)))

            print("Initiator: socket: Response packet information.")

            response_packet = packet.packet()

            ret = response_packet.decode(self.unstringify_packet(response))

            response_packet.info()

            if( ret == True ):

                print("Initiator: socket: Response packet is valid.")

            else:

                print("Initiator: socket: Response packet is invalid !")

                response_packet.info()

                pass

    def set_parameter_value(self, name: str, value) -> bool:
        return self.device.set_parameter_value(name, value)
    
    def set_parameter_value_in_device(self, name: str, value) -> bool:

        ret = self.device.set_parameter_value(name, value)

        if self.device.get_parameter_register(name) is None:

            return False

        register_name = self.device.get_parameter_register(name).name
        register_value = self.device.get_parameter_register(name).value

        self.write_register( register_name, register_value )

    def get_parameter_value(self , name: str) -> bool:

        return self.device.get_parameter_value(name)

    def get_parameter_value_from_device(self, name: str):

        register = self.device.get_parameter_register(name)

        register_name = register.name

        ret = self.read_register( register_name )

        if( ret == False ):
            return False
        
        return self.device.get_parameter_value(name)

x = initiator('test.yaml', InterfaceType.SOCKET)

parameter_names = x.device.get_parameter_names()

for parameter_name in parameter_names:

    min, max = x.device.get_parameter_limits(parameter_name)

    for i in range( int(min), int(max) + 1 ):
        x.set_parameter_value_in_device(parameter_name, i)

        ret = x.get_parameter_value_from_device(parameter_name)

        print("Parameter name: {_name}".format(_name = parameter_name))
        print("Expected value: {_expected}".format(_expected = i))
        print("Observed value: {_observed}".format(_observed = ret))

        if( ret == i ):
            print(">>>> PASSED")
        else:
            print(">>>> FAILED")
            break
