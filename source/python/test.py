
import initiator
import time

x = initiator.initiator('lp_em_cc2340r5_example.yml', initiator.InterfaceType.BLE)

parameter_names = x.device.get_parameter_names()

#x.set_parameter_value("led0_blink_interval_ms", 100)
#x.set_parameter_value("led0_blink_count", 10)
#x.set_parameter_value_in_device("led0_blink", 1)

#x.set_parameter_value("led1_blink_interval_ms", 250)
#x.set_parameter_value("led1_blink_count", 20)
#x.set_parameter_value_in_device("led1_blink", 1)

#x.set_parameter_value("led0_blink_interval_ms", 100)
#x.set_parameter_value("led0_blink_count", 25)
#x.set_parameter_value_in_device("led0_blink", 1)

#x.set_parameter_value("led1_blink_interval_ms", 50)
#x.set_parameter_value("led1_blink_count", 100)
#x.set_parameter_value_in_device("led1_blink", 1)

print(x.get_parameter_value_from_device("button0_press_count"))
print(x.get_parameter_value_from_device("button1_press_count"))