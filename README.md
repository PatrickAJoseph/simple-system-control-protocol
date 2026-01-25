# simple-system-control-protocol
A simple fixed frame transport layer protocol consisting of ASCII character encoding which can be used for register like control of embedded devices. Current supported data link protocols are UART and bluetooth.

The unencoded packet structure is as follows

|    BYTE    |    BITFIELD (0 indexe)   |      BIT FIELD NAME       |                              DESCRIPTION                    |
|------------|--------------------------|---------------------------|-------------------------------------------------------------|
