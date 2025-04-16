# Linux_SDR
Linux Software Defined Radio

## milestone 1 - UDP Packet Sending

The milestone 1 directory has one executable for Zynq that send {num_packets} to {port} of IP {ip_address}

This executable is cross-compiled on Petalinux using the installed arm compiler, which mean that it is only executable on Petalinux. To execute, run:
`./udpSender -i <ip> -p <port> -n <num_packets>`

Usage:
``
Usage: ./udpSender -i <ip> -p <port> -n <num_packets>
  ip: The IP address of the server.
  port: The port number of the server. The port must be greater than 0
  num_packets: The number of packets to send. num_packets must be greater than 0
``


The directory has the executable's C source file and a python script for receiving UDP packets. You can run the python script on either WinOS or Linux.

To use the python script, run `python3.exe udpReceiver.py -p {port_listen_on}`