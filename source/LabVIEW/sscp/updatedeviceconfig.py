import yaml
import sys

with open("config.yml","r") as configFile:

    data = yaml.safe_load(configFile)

    deviceName = sys.argv[1]
    deviceNumber = sys.argv[2]
    serialPort = sys.argv[3]
    serialBaudRate = sys.argv[4]
    basePort = sys.argv[5]
    deviceSerialNumber = sys.argv[6]
    interfaceTimeout = sys.argv[7]

    data['defconfig']['device_name'] = deviceName
    data['defconfig']['device_number'] = int(deviceNumber)
    data['defconfig']['serial_port'] = serialPort
    data['defconfig']['serial_baud_rate'] = int(serialBaudRate)
    data['defconfig']['base_port'] = int(basePort)
    data['defconfig']['device_serial_number'] = int(deviceSerialNumber)
    data['defconfig']['interface_timeout_ms'] = int(interfaceTimeout)

    with open("tempconfig.yml","w") as tempConfigFile:
        yaml.safe_dump(data, tempConfigFile)