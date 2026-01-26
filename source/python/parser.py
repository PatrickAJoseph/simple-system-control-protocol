
import yaml
from enum import Enum
from typing import Type
from typing import IO
import struct

# Enum for parameter datatypes.

class parameter_type(Enum):
    UINT = 0
    INT = 1
    FLOAT = 2

# Parameter class.

class parameter:
    number:         int                         # Parameter number.
    reg:            int                         # Register number
    name:           str                         # Parameter name.
    startbit:       int                         # Start bit in register.
    bitlength:      int                         # Number of bits in parameter bit field.
    type:           parameter_type              # Data type of parameter.
    min:            float                       # Minimum parameter value.
    max:            float                       # Maximum parameter value.
    value:          float                       # Value.

    ##
    #   @brief:     Initialize private members of the parameter class.
    #

    def __init__(self, reg: int, number: int, name: str, startbit: int, bitlength: int, type: parameter_type, min: float, max: float):
        self.number = number
        self.reg    = reg
        self.name = name
        self.startbit = startbit
        self.bitlength = bitlength
        self.type = type
        self.min = min
        self.max = max
        self.value = 0

    #
    # @brief: Validate and set parameter value.
    #

    def set(self, value):

        _value = 0

        # Limit value of passed parameter to permitted values.

        if( self.type == parameter_type.UINT ):

            _value = int(value)

            if( _value < int(self.min) ):
                _value = int(self.min)

            if( _value > int(self.max) ):
                _value = int(self.max)

            self.value = float(_value)

        if( self.type == parameter_type.INT ):

            _value = int(value)

            if( _value < int(self.min) ):
                _value = int(self.min)

            if( _value > int(self.max) ):
                _value = int(self.max)

            self.value = float(_value)

        if( self.type == parameter_type.FLOAT ):

            _value = value

            if( _value < self.min ):
                _value = self.min

            if( _value > self.max ):
                _value = self.max

            self.value = float(_value)

        return self.value

    ##
    #   @brief: Get parameter value.
    #

    def get(self):
        return self.value

    ##
    #   @brief: Encode parameter value.
    #

    def encode(self):

        result = 0
        _value = 0
        bitmask = (1 << int(self.bitlength) ) - 1
        bitpos = self.startbit

        # Validate if parameter value is within range.

        self.set(self.value)

        # Generate bit-shifted encoded value.

        if( self.type == parameter_type.UINT ):
            result = ( int(self.value) & ( bitmask ) ) << bitpos

        if( self.type == parameter_type.INT ):
            result = ( int(self.value) & bitmask ) << bitpos

        if( self.type == parameter_type.FLOAT ):
            result = ( int(struct.pack(">f", self.value).hex(),16) & bitmask ) << bitpos

        # Make sure value stays within 32-bit range.

        result = result & 0xFFFFFFFF

        return result

    ##
    #   @brief: Decode parameter value from register value.
    #

    def decode(self, regval: int):

        # Calcuate bit mask and bit position.

        bitmask = ( 1 << int(self.bitlength) ) - 1
        bitpos  = int(self.startbit)

        # Extract bitfield for target parameter.

        result = (regval >> bitpos) & bitmask

        # Limit range to 32-bits.

        result = result & 0xFFFFFFFF

        # Conversion from raw value to integer value.

        if( self.type == parameter_type.UINT ):
            self.value = float(result)
        
        if( self.type == parameter_type.INT ):
            
            # For signed integer values, value of integer ranges from
            # -(2^bitlength - 1) to +(2^bitlength - 1). 

            if( result > ( bitmask >> 1 ) ):
                result = result - bitmask + 1
                result = -result
            else:
                result = int(result)

            result = result & 0xFFFFFFFF

            self.value = float(result)

        # Convert from IEEE754 32-bit format to floating point number.

        if( self.type == parameter_type.FLOAT ):
            self.value = float( struct.unpack('!f', struct.pack('!I', result))[0] )

        return self.value

    # Prints information about the parameter.

    def info(self):

        print("\n\n")

        print("Parameter name: {_name}".format(_name = self.name))
        print("Parameter number: {_number}".format(_number = self.number))
        print("Register number: {_number}".format(_number = self.reg))
        print("Parameter start bit: {_startbit}".format(_startbit = self.startbit))
        print("Parameter bit length: {_bitlength}".format(_bitlength = self.bitlength))
        print("Parameter datatype: {_datatype}".format( _datatype = self.type))
        print("Parameter minimum value: {_min}".format( _min = self.min ))
        print("Parameter maximum value: {_max}".format( _max = self.max ))
        print("Parameter value: {_value}".format( _value = self.value ))

        print("\n\n")

# A class representing a register.

class register:
    number: int                         # Register number.
    name: str                           # Register name.
    parameters: list[parameter]         # Parameters under the register.
    value: int                          # Register value.

    # Class initialization function.

    def __init__(self, number: int, name: str, parameters: list[Type]):
        self.number = number
        self.name = name
        self.parameters = parameters
        self.value = 0
    
    # Adds a parameter to the parameter list.

    def add_parameter(self, parameter):
        self.parameters.append(parameter)
    
    # Encodes the parameters under the register and then
    # acumulates it into a single 32-bit register value.

    def encode(self):
        result = 0

        for parameter in self.parameters:
            result |= parameter.encode()

        self.value = result & 0xFFFFFFFF

        return result

    # Decodes the parametes under the register.
    # Make sure to call set() function to update the register
    # values before calling this function.

    def decode(self):

        for parameter in self.parameters:
            parameter.decode(self.value)
    
    # Get 32-bit register value.

    def get(self):

        return self.value & 0xFFFFFFFF
    
    # Set 32-bit register value.

    def set(self, value: int):
        value = value & 0xFFFFFFFF
        self.value = value
        return value

    # Set parameter value under a register.

    def set_parameter_value(self, name: str, value):

        for parameter in self.parameters:

            if( parameter.name == name ):

                parameter.set(value)
        
        self.encode()
    
    # Get parameter value under a register.

    def get_parameter_value(self, name: str):
        
        self.decode()

        for parameter in self.parameters:
            
            if( parameter.name == name ):

                return parameter.value

    # Get handle of a parameter under a register.

    def get_parameter(self, name: str):

        for parameter in self.parameters:

            if( parameter.name == name ):

                return parameter

        return None

    # Prints information about device register.

    def info(self):

        print("\n\n")

        print("Register name: {_name}".format(_name = self.name))
        print("Register number: {_value}".format(_value = self.number))
        print("Register value: {_value}".format(_value = hex(self.value)))

        print("\n\n")

# Class to represent a device node.

class device:
    name: str
    registers: list[register]
    number: int
    baseport: int
    file: IO

    def __init__(self, file):
        self.file = file
        self.registers = []
        self.number = 0
        self.baseport = 0
        
        with open(file, "r") as f:

            data = yaml.safe_load(f)

            # Load initial settings.

            name = data['defconfig']['name']
            number = int(data['defconfig']['device_number'])
            baseport = int(data['defconfig']['base_port'])

            # Count the number of registers.

            register_count = 0

            while(True):

                try:
                    value = data['regs']['reg_{_count}'.format(_count = register_count)]
                    register_count = register_count + 1
                except:
                    break

            # Go through each register and add parameters for each register.

            parameter_count = 0

            for register_iterator in range(0, register_count):

                register_iterator_name = 'reg_{_iterator}'.format( _iterator = register_iterator )

                parameter_count = 0

                # Add register to list.

                register_number = int( data['regs'][register_iterator_name]['number'] )
                register_name   = data['regs'][register_iterator_name]['name']

                reg = register(register_number, register_name, [])

                self.registers.append(reg)

                # Count the number of registers.

                while(True):

                    try:
                        parameter_iterator_name = 'parameter_{_iterator}'.format( _iterator = parameter_count )
                        test = data['regs'][register_iterator_name][parameter_iterator_name]
                        parameter_count = parameter_count + 1
                    except:
                        break

                # Add parameter to the parameters list under the register.

                for parameter_iterator in range(0, parameter_count):

                   parameter_iterator_name = 'parameter_{_iterator}'.format(_iterator = parameter_iterator)
                   
                   parameter_name       =  data['regs'][register_iterator_name][parameter_iterator_name]['name']
                   parameter_number     =  int(data['regs'][register_iterator_name][parameter_iterator_name]['number'])
                   parameter_startbit   =  int(data['regs'][register_iterator_name][parameter_iterator_name]['startbit'])
                   parameter_bitlength   =  int(data['regs'][register_iterator_name][parameter_iterator_name]['bitlength'])
                   parameter_min         =  float(data['regs'][register_iterator_name][parameter_iterator_name]['min'])
                   parameter_max         =  float(data['regs'][register_iterator_name][parameter_iterator_name]['max'])
                   parameter_dtype       =  data['regs'][register_iterator_name][parameter_iterator_name]['type']
                   parameter_datatype    =  parameter_type.UINT

                   if( parameter_dtype == 'uint' ):
                        parameter_datatype = parameter_type.UINT                       

                   if( parameter_dtype == 'int' ):
                        parameter_datatype = parameter_type.INT

                   if( parameter_dtype == 'float' ):
                        parameter_datatype = parameter_type.FLOAT

                   p = parameter(register_number, parameter_number, parameter_name, parameter_startbit, parameter_bitlength, parameter_datatype, parameter_min, parameter_max)

                   self.registers[register_number].parameters.append(p)

    # Function to get a register handle.

    def get_register(self, name: str):

        for register in self.registers:

            if( register.name == name ):

                return register

        return None
    
    def get_register(self, number: int):

        for register in self.registers:

            if( register.number == number ):

                return register

        return None

    # Function to set a device parameter value in parser base.

    def set_parameter_value(self, name: str, value):

        for register in self.registers:

            for parameter in register.parameters:

                if( parameter.name == name ):

                    parameter.set(value)
                    register.encode()
                    return True
        
        return False
    
    # Function to get a device parameter value in parser base.

    def get_parameter_value(self, name: str):

        for register in self.registers:

            for parameter in register.parameters:

                if( parameter.name == name ):

                    register.decode()
                    parameter.get()
                    return True
        
        return False