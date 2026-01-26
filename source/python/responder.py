
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
    
    def read_register(self, name: str):

        reg = self.device.get_register(name)

        p = packet.packet()
        p.encode( device_id = self.device.number, ack = False, read = True, write = False, reg = reg.number, value = 0 )

        #if( self.interface == InterfaceType.SOCKET ):

responder = responder('test.yaml', InterfaceType.SOCKET)