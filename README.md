# cari-ctrl
Example CARI control tool

## Parameters
| Parameter          | Description                                                                         | Mandatory? |
|--------------------|-------------------------------------------------------------------------------------|------------|
| -s "IP:port"       | Source IP and port                                                                  |            |
| -d "IP:port"       | Destination IP and port                                                             | yes        |
| -r                 | Reset remote device                                                                 |            |
| -p *val*           | RF power setpoint in dBm (where ALC is available) as a decimal number               |            |
| -rf *val*          | RX frequency in Hertz as an integer                                                 |            |
| -tf *val*          | TX frequency in Hertz as an integer                                                 |            |
| -rc *val*          | RX frequency correction in ppm as a decimal number                                  |            |
| -tc *val*          | TX frequency correction in ppm as a decimal number                                  |            |
| -afc *1/0*         | Automatic Frequency Control (where available), 1-on, 0-off                          |            |
| -rx *1/0*          | Activate RX baseband downstream, 1-on, 0-off                                        |            |
