
import parser

dev = parser.device('test.yaml')
dev.set_parameter_value("TEST_REG_0_PARAM_0", 65)
dev.set_parameter_value("TEST_REG_0_PARAM_1", 9)
print(hex(dev.registers[0].get()))