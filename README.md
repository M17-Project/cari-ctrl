# cari-ctrl
Example CARI control tool

## Parameters
| Parameter          | Description                                                                         | Mandatory? |
|--------------------|-------------------------------------------------------------------------------------|------------|
| -d IP:port         | Destination (RRU) IP and port (for the control channel)*                            | yes        |
| -s IP:port         | Source (BBU) IP and port (for baseband uplink)**                                    |            |
| -r                 | Reset remote device                                                                 |            |
| -p *val*           | RF power setpoint in dBm (where ALC is available) as a decimal number               |            |
| -rf *val*          | RX frequency in Hertz as an integer                                                 |            |
| -tf *val*          | TX frequency in Hertz as an integer                                                 |            |
| -rc *val*          | RX frequency correction in ppm as a decimal number                                  |            |
| -tc *val*          | TX frequency correction in ppm as a decimal number                                  |            |
| -afc *1/0*         | Automatic Frequency Control (where available), 1-on, 0-off                          |            |
| -rx *1/0*          | Activate RX baseband downstream, 1-on, 0-off                                        |            |

*) This is the address of the Remote Radio Unit (RRU), with port set with `-ctrl` argument.
**) This is the address of the ZMQ baseband publisher, running at the Baseband Unit (BBU).
