
import initiator

x = initiator.initiator('lp_em_cc2340r5_example.yml', initiator.InterfaceType.SERIAL)

parameter_names = x.device.get_parameter_names()

x.set_parameter_value_in_device("led0_blink_interval_ms", 100)
x.set_parameter_value_in_device("led0_blink_count", 10)
x.set_parameter_value_in_device("led0_blink", 1)

x.set_parameter_value_in_device("led1_blink_interval_ms", 250)
x.set_parameter_value_in_device("led1_blink_count", 10)
x.set_parameter_value_in_device("led1_blink", 1)