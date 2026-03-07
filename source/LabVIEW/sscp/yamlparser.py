
import time
import sys
import yaml

configFile = sys.argv[1]

with open(configFile,"r") as configFileHandle:

    data = yaml.safe_load(configFileHandle)

    with open("config.yml", "w") as tempConfigFileHandle:

        yaml.dump(data, tempConfigFileHandle)

    # Get number of registers.

    register_count = 0

    while(True):

        try:
            value = data['regs']['reg_{_count}'.format(_count = register_count)]
            register_count = register_count + 1
        except:
            break

    #print(f"Number of registers: {register_count}")

    # Go through each register and add parameters for each register.

    parameter_count = 0

    for register_iterator in range(0, register_count):

        register_iterator_name = 'reg_{_iterator}'.format( _iterator = register_iterator )

        parameter_count = 0

        # Add register to list.

        register_number = int( data['regs'][register_iterator_name]['number'] )
        register_name   = data['regs'][register_iterator_name]['name']

        # Count the number of parameters.

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

            csvEntryTuple = (register_number,register_name,parameter_number,parameter_name,
                                    parameter_startbit,parameter_bitlength,parameter_min,parameter_max,parameter_dtype)
                   
            print(csvEntryTuple)

    # Get default device configuration.

    deviceName = data['defconfig']['name']
    deviceNumber = data['defconfig']['device_number']
    serialPort = data['defconfig']['serial_port']
    serialBaudRate = data['defconfig']['serial_baud_rate']
    basePort = data['defconfig']['base_port']
    deviceSerialNumber = data['defconfig']['device_serial_number']
    interfaceTimeout = data['defconfig']['interface_timeout_ms']

    defaultConfigs = f"[deviceName={deviceName},deviceNumber={deviceNumber},serialPort={serialPort},serialBaudRate={serialBaudRate},"
    defaultConfigs = defaultConfigs + f"basePort={basePort},deviceSerialNumber={deviceSerialNumber},interfaceTimeout={interfaceTimeout}]"

    print(defaultConfigs)