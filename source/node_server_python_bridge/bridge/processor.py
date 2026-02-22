import sys
import tempfile

sys.path.append("../../python")

from typing import Optional
import initiator

processor_initiator: Optional[initiator] = None

def process_command(type, reg, param, value = 0):

    global processor_initiator

    print("Type of command: {_type}".format( _type = type ))

    if type == "connect":
        rc = 0
        print("Connnection request received.")

        configfile = tempfile.gettempdir() + '\config.yml'

        print(configfile)

        try:
            processor_initiator = initiator.initiator(configfile, initiator.InterfaceType.SERIAL)
            return "{'type': connect, 'status': pass}" 
        except:
            pass
        
        return "{'type': connect, 'status': fail}"

    elif type == "get":
        rc = 0
        print("Register to get: {_reg}".format(_reg = reg))
        print("Parameter to get: {_param}".format( _param = param))

        try:
            value = processor_initiator.get_parameter_value_from_device(param)
        except:
            rc = -1

        return "{{'type':set , 'register': '{_reg}', 'parameter': '{_param}', 'value': '{_value}', 'return_code': '{_rc}'}}".format(_reg = reg, _param = param, _value= value, _rc = rc)
    elif type == "set":
        print("Register to set: {_reg}".format(_reg = reg))
        print("Parameter to set: {_param}".format( _param = param))
        print("Value to set: {_value}".format( _value = value ))
        rc = 0

        try:
            processor_initiator.set_parameter_value_in_device(param, value)
        except:
            rc = -1

        return "{{'type':set , 'register': '{_reg}', 'parameter': '{_param}', 'value': '{_value}', 'return_code': '{_rc}'}}".format(_reg = reg, _param = param, _value= value, _rc = rc)
    
    return "{'error': 'Unknown command'}"