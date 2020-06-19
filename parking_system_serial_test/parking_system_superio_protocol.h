#ifndef PARKING_SYSTEM_SUPERIO_PROTOCOL_H
#define PARKING_SYSTEM_SUPERIO_PROTOCOL_H

#define LP_LEN 5 //license plate text length without chinese.

#define CMD_GATE_OPEN       0xC0
#define CMD_GATE_CLOSE      0xC1
#define CMD_GATE_IGNORE     0xC2
#define CMD_DISPLAY_LP      0xC3

#define SIG_SENSOR_IN_PRESSED  0xD0
#define SIG_SENSOR_IN_RELEASED 0xD1
#define SIG_SENSOR_OUT_PRESSED  0xD2
#define SIG_SENSOR_OUT_RELEASED 0xD3

#endif // PARKING_SYSTEM_SUPERIO_PROTOCOL_H
