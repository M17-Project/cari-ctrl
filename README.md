# cari-ctrl
Example CARI control tool

### Dependencies:
- libzmq3

### CLI parameters:

```
Usage: ./cari-ctrl [OPTIONS]

CARI Ctrl

Required options:
  -d, --dest=ADDR       Destination (RRU) IP and port (for the control channel)
                        This is the address of the Remote Radio Unit (RRU), with port set with `--ctrl` argument.
  -s, --source=ADDR     Source (BBU) IP and port (for baseband uplink)
                        This is the address of the ZMQ baseband publisher, running at the Baseband Unit (BBU).

Optional options:
  -r, --reset           Reset remote device
  -p, --power=DBM       RF power setpoint in dBm (where ALC is available) as a decimal number
  -f, --rf=FREQ         RX frequency in Hertz as an integer (420000000-450000000)
  -F, --tf=FREQ         TX frequency in Hertz as an integer (420000000-450000000)
  -c, --rc=PPM          RX frequency correction in ppm as a decimal number (-100.0 to 100.0)
  -C, --tc=PPM          TX frequency correction in ppm as a decimal number (-100.0 to 100.0)
  -a, --afc=ENABLE      Automatic Frequency Control (where available), 1-on, 0-off
  -R, --rx=ENABLE       Activate RX baseband downstream, 1-on, 0-off
  -h, --help            Display this help message and exit

Example:
  ./cari-ctrl -d 192.168.1.200:17002 -s 192.168.1.100:17004 -p 20.5 --rf 433000000
```
