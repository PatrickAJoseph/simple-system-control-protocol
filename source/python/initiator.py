
import packet
import parser

from enum import Enum
from queue import Queue
from typing import IO

class InterfaceType(Enum):
    SOCKET = 0
    SERIAL = 1
    BLE    = 2

class initiator:
    
    interface: InterfaceType
    device: parser.device
    request_queue: Queue
    response_queue: Queue
    callback_table: []
    file: str

    def __init__(self, file:str, interface: InterfaceType):

        self.file = file
        self.interface = interface

        self.device = parser.device(self.file)
        self.callback_table = []


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

        self.request_queue.put(p.bytes)