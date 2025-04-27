#!/bin/bash

# Initialize network settings for the SDR application

# Set network interface
INTERFACE="eth0"

# Configure IP address and subnet mask
IP_ADDRESS="192.168.1.24"
SUBNET_MASK="255.255.255.0"

# Default HTTP port
HTTP_PORT="8080"

# Parse arguments for IP address and HTTP port
while getopts "i:p:" opt; do
    case $opt in
        i) IP_ADDRESS="$OPTARG" ;;
        p) HTTP_PORT="$OPTARG" ;;
        *) echo "Usage: $0 -i <IP_ADDRESS> (default=192.168.1.24) -p <HTTP_PORT> (default=8080)" >&2; exit 1 ;;
    esac
done

# Validate that both arguments are provided
if [ -z "$IP_ADDRESS" ] || [ -z "$HTTP_PORT" ]; then
    echo "Error: Both IP address and HTTP port must be specified."
    echo "Usage: $0 -i <IP_ADDRESS> -p <HTTP_PORT>"
    exit 1
fi

# Bring the network interface up
ifconfig $INTERFACE up

# Assign IP address and subnet mask
ifconfig $INTERFACE $IP_ADDRESS netmask $SUBNET_MASK

# Display the network configuration
echo "Network configuration for $INTERFACE:"
ifconfig $INTERFACE


# HTTP server setup
echo "Starting HTTP server on port $HTTP_PORT..."
httpd -p $HTTP_PORT &
# Check if the HTTP server started successfully
if [ $? -ne 0 ]; then
    echo "Error: Failed to start HTTP server."
    exit 1
fi
# Display the HTTP server status
echo "HTTP server is running on port $HTTP_PORT."


echo "Network initialization complete."