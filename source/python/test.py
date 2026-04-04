
import initiator
import time

def ss1306_write(x: initiator, data: list):

    for i in range (0, len(data)):
        x.set_parameter_value("I2C_WRITE_DATA_index", i)
        x.set_parameter_value_in_device("I2C_WRITE_DATA_byte", int(data[i]))

    x.set_parameter_value("I2C_TRANSFER_CONFIG_readsize", 0)
    x.set_parameter_value("I2C_TRANSFER_CONFIG_writesize", len(data))
    x.set_parameter_value_in_device("I2C_TRANSFER_CONFIG_transfer", 1)
    print(x.get_parameter_value_from_device("I2C_TRANSFER_CONFIG_writesize"))
    print("Status:")
    print(x.get_parameter_value_from_device("I2C_TRANSFER_CONFIG_status"))


def set_cursor(x, col, page):
    ss1306_write(x, [0x00, 0xB0 + page])                 # Page address
    ss1306_write(x, [0x00, 0x00 + (col & 0x0F)])         # Lower column
    ss1306_write(x, [0x00, 0x10 + (col >> 4)])           # Higher column

def write_char(x, ch):
    if ch in font:
        ss1306_write(x, [0x40, 0xFF])   # 0x40 = data
        ss1306_write(x, [0x40, 0x00])        # space between chars

def write_string(x, text):
    for ch in text:
        write_char(x, ch)

x = initiator.initiator('bluetooth_daq.yml', initiator.InterfaceType.SERIAL)

parameter_names = x.device.get_parameter_names()

x.set_parameter_value("I2C_CONTROL_fast_mode_enable", 1)
x.set_parameter_value("I2C_CONTROL_address", int(0x3C))
x.set_parameter_value_in_device("I2C_CONTROL_timeout", 1000)

# Display OFF
ss1306_write(x, [0x00, 0xAE])

# Clock divide
ss1306_write(x, [0x00, 0xD5, 0x80])

# Multiplex
ss1306_write(x, [0x00, 0xA8, 0x3F])

# Offset
ss1306_write(x, [0x00, 0xD3, 0x00])

# Start line
ss1306_write(x, [0x00, 0x40])

# Charge pump
ss1306_write(x, [0x00, 0x8D, 0x14])

# Addressing mode
ss1306_write(x, [0x00, 0x20, 0x00])  # Horizontal

# Segment remap
ss1306_write(x, [0x00, 0xA1])

# COM scan direction
ss1306_write(x, [0x00, 0xC8])

# COM pins
ss1306_write(x, [0x00, 0xDA, 0x12])

# Contrast
ss1306_write(x, [0x00, 0x81, 0xCF])

# Precharge
ss1306_write(x, [0x00, 0xD9, 0xF1])

# VCOM detect
ss1306_write(x, [0x00, 0xDB, 0x40])

# Resume RAM display
ss1306_write(x, [0x00, 0xA4])

# Normal display
ss1306_write(x, [0x00, 0xA6])

# Display ON
ss1306_write(x, [0x00, 0xAF])

for i in range(0,1024):
    ss1306_write(x, [0x40, 0x00])

for i in range(0,1024):
    ss1306_write(x, [0x40, (1 << (i%8)) - 1])

for i in range(0,1024):
    ss1306_write(x, [0x40, (1 << (7 - (i%8))) - 1])