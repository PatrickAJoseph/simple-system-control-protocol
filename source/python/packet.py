
from crc import Calculator, Crc8

class packet:

    bytes:                  list[int]
    unencoded_bytes:        list[int]
    device_id:              int
    ack:                    bool
    read:                   bool
    write:                  bool
    reg:                    int
    value:                  int
    crc8:                   int

    def __init__(self):

        self.unencoded_bytes = []
        self.bytes = []

    def encode(self, device_id: int, ack: bool, read: bool, write: bool, reg: int, value: int):

        self.device_id = device_id
        self.ack = ack
        self.read = read
        self.write = write
        self.reg = reg
        self.value = value

        self.unencoded_bytes = []

        # Add start of frame character '*'

        self.unencoded_bytes.append(int(0x2a))

        temp = int(0)
        temp |= (int(device_id) << 3)
        temp |= (int(ack) << 2)
        temp |= (int(read) << 1)
        temp |= (int(write) << 0)

        # Add device ID and control byte.

        self.unencoded_bytes.append(int(temp))

        # Add register ID.

        self.unencoded_bytes.append(int(reg))
        
        # Add the register data bytes.

        self.unencoded_bytes.append( int( (int(value) & 0xFF000000) >> 24 ) )
        self.unencoded_bytes.append( int( (int(value) & 0x00FF0000) >> 16 ) )
        self.unencoded_bytes.append( int( (int(value) & 0x0000FF00) >> 8 ) )
        self.unencoded_bytes.append( int( (int(value) & 0x000000FF) >> 0 ) )

        # Calculate CRC8-CCITT.

        calculator = Calculator(Crc8.CCITT)

        self.crc8 = calculator.checksum(self.unencoded_bytes[1:])

        # Add CRC8 to unencoded packet.

        self.unencoded_bytes.append(int(self.crc8))

        # Add end of frame character '#'.

        self.unencoded_bytes.append(int(0x23))

        # Convert the unencoded bytes to encoded bytes.

        # Add start of frame character.

        self.bytes.append(self.unencoded_bytes[0])

        # Encode device control byte.

        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[1] >> 4) & 0x0F])
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[1] >> 0) & 0x0F])

        # Encode register ID byte.

        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[2] >> 4) & 0x0F])
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[2] >> 0) & 0x0F])

        # Encode register data bytes.

        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[3] >> 4) & 0x0F])
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[3] >> 0) & 0x0F])
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[4] >> 4) & 0x0F])
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[4] >> 0) & 0x0F])
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[5] >> 4) & 0x0F])
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[5] >> 0) & 0x0F])        
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[6] >> 4) & 0x0F])
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[6] >> 0) & 0x0F])        

        # Encode CRC byte.

        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[7] >> 4) & 0x0F])
        self.bytes.append(b'0123456789ABCDEF'[(self.unencoded_bytes[7] >> 0) & 0x0F])        

        # Add end of frame character.

        self.bytes.append(self.unencoded_bytes[8])

        return self.bytes

    def hexchar_to_int(self, hexchar):

        if( ( hexchar >= 48 ) and ( hexchar <= 57 ) ):
            return hexchar - 48
        
        if( ( hexchar >= 65 ) and ( hexchar <= 70 ) ):
            return (hexchar - 65) + 10
        
        return 0

    # Decode the 16-byte string.

    def decode(self, bytes: list[int]):

        self.unencoded_bytes = []
        self.bytes = bytes

        # Check length of byte string.

        if( len(bytes) != 16 ):
            return False
        
        # Check start of frame character.

        if( self.bytes[0] != int(0x2A) ):
            return False
        
        # Check end of frame character.

        if( self.bytes[15] != int(0x23) ):
            return False
        
        # After frame checks, proceed to convert the encoded packet into
        # unencoded packet.

        temp = 0

        self.unencoded_bytes.append(self.bytes[0])

        temp = 0
        temp |= ( self.hexchar_to_int(self.bytes[1]) << 4 )
        temp |= ( self.hexchar_to_int(self.bytes[2]) << 0 )
        self.unencoded_bytes.append(temp)

        temp = 0
        temp |= ( self.hexchar_to_int(self.bytes[3]) << 4 )
        temp |= ( self.hexchar_to_int(self.bytes[4]) << 0 )
        self.unencoded_bytes.append(temp)

        temp = 0
        temp |= ( self.hexchar_to_int(self.bytes[5]) << 4 )
        temp |= ( self.hexchar_to_int(self.bytes[6]) << 0 )
        self.unencoded_bytes.append(temp)

        temp = 0
        temp |= ( self.hexchar_to_int(self.bytes[7]) << 4 )
        temp |= ( self.hexchar_to_int(self.bytes[8]) << 0 )
        self.unencoded_bytes.append(temp)

        temp = 0
        temp |= ( self.hexchar_to_int(self.bytes[9]) << 4 )
        temp |= ( self.hexchar_to_int(self.bytes[10]) << 0 )
        self.unencoded_bytes.append(temp)

        temp = 0
        temp |= ( self.hexchar_to_int(self.bytes[11]) << 4 )
        temp |= ( self.hexchar_to_int(self.bytes[12]) << 0 )
        self.unencoded_bytes.append(temp)

        temp = 0
        temp |= ( self.hexchar_to_int(self.bytes[13]) << 4 )
        temp |= ( self.hexchar_to_int(self.bytes[14]) << 0 )
        self.unencoded_bytes.append(temp)

        self.unencoded_bytes.append(self.bytes[15])

        # Verify CRC8-CCITT.

        calculator = Calculator(Crc8.CCITT)

        self.crc8 = calculator.checksum(self.unencoded_bytes[1:7])

        if(self.crc8 != 0):
            return False

        # Extract device control byte.

        self.device_id = (self.unencoded_bytes[1] >> 3) & 0xFF
        self.ack       = bool( ( self.unencoded_bytes[1] >> 2 ) & 0x1 )
        self.read      = bool( ( self.unencoded_bytes[1] >> 1 ) & 0x1 )
        self.write     = bool( ( self.unencoded_bytes[1] >> 0 ) & 0x1 )

        # Extract register ID byte.

        self.reg       = self.unencoded_bytes[2]

        # Extract register data value.

        self.value = 0
        self.value |= ( self.unencoded_bytes[3] << 24 )
        self.value |= ( self.unencoded_bytes[4] << 16 )
        self.value |= ( self.unencoded_bytes[5] << 8 )
        self.value |= ( self.unencoded_bytes[6] << 0 )

        return True