# Linux_SDR
Linux Software Defined Radio

## Web Setup Instructions
To set up the web interface, you will need the compressed project folder `linux_sdr_Zhou.tgz`, available [here](/submission/linux_sdr_Zhou.tgz).

### 1. Boot Petalinux
- Boot the Zybo board with Petalinux via **SD card** or **QSPI**.

### 2. Configure Network Settings
- Open the Petalinux serial console using a terminal application (e.g., PuTTY, MobaXterm, Tera Term) with the settings **115200 8N1**.
- Set the Zybo board's IP address:
  ```bash
  ifconfig eth0 <ip address> netmask 255.255.255.0
  ```
  You may close the serial console after setting the IP address.

### 3. Transfer the Web Project to the Board
- SSH into the Zybo board:
  - Host: `<ip address>`
  - Port: 22
  - User: student 
- Transfer `linux_sdr_Zhou.tgz` to the board using SCP, SFTP, or tools like WinSCP or MobaXterm.

### 4. Extract the Web Project
- SSH into the board and navigate to the directory where `linux_sdr_Zhou.tgz` was uploaded.
- Extract the contents:
  ``` bash
  tar -xvf linux_sdr_Zhou.tgz
  ```
- A new `web/` directory will be created, containing `index.html` and required CGI scripts under `web/cgi-bin/`.
- Ensure CGI scripts are executable:
  ``` bash
  cd web/cgi-bin/
  chmod +x *
  ```

### 5. Start the HTTP Server
- Run the provided network initialization script to configure the IP address and start the HTTP server 
  ``` bash
  Usage: ./init_network.sh -i <IP_ADDRESS> (default=192.168.1.24) -p <HTTP_PORT> (default=8080)

  sudo ./init_network.sh -i <ip address> -p <port>
  ```
  Example:
  ``` bash
  zybo_linux:~$ sudo ./init_network.sh -i 192.168.1.25 -p 8081
  Password:
  Network configuration for eth0:
  eth0      Link encap:Ethernet  HWaddr 00:0A:35:00:1E:53
            inet addr:192.168.1.25  Bcast:192.168.1.255  Mask:255.255.255.0
            inet6 addr: fe80::20a:35ff:fe00:1e53/64 Scope:Link
            UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
            RX packets:13451 errors:0 dropped:0 overruns:0 frame:0
            TX packets:410757 errors:0 dropped:0 overruns:0 carrier:0
            collisions:0 txqueuelen:1000
            RX bytes:5577252 (5.3 MiB)  TX bytes:425756425 (406.0 MiB)
            Interrupt:40 Base address:0xb000

  Starting HTTP server on port 8081...
  HTTP server is running on port 8081.
  Network initialization complete.
  ```
  Upon success, you will see network information and confirmation that the HTTP server is running.

### 6. Access the Web Interface
- Open a web browser and navigate to:
  `http://<ip address>:<port>`
- The `index.html` page will load, allowing you to control the radio tuner through the browser.

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

## milestone 2 - Radio + Custom FIFO Peripheral

The PL design includes an axi4 stream fifo for sending data from PL into PS

The milestone 2 directory contains following files:
* design_1_wrapper.bit.bin (the fpga bitstream binary)
* fifo_reader (arm executable that reads 480,000 samples from the FIFO)
* fifo_reader.c (c source file)

fifo base address: 0x43c00000
radio tuner base address: 0x43c10000
