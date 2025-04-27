#!/bin/bash

# default values 
SEND_UDP_APPLICATION=udpSender
ip=192.168.1.3
port=25344
numPackets=100
# Read POST data -- GPT suggested this
read -n "$CONTENT_LENGTH" POST_DATA
# Parse ADDRESS, PORT, and NUMPACKETS from POST
ip=$(echo "$POST_DATA" | sed -n 's/.*ip=\([^&]*\).*/\1/p')
port=$(echo "$POST_DATA" | sed -n 's/.*port=\([^&]*\).*/\1/p')
num_packets=$(echo "$POST_DATA" | sed -n 's/.*num_packets=\([^&]*\).*/\1/p')

# Send HTTP header
echo "Content-type: text/html"
echo ""

# Send page content
echo "<html><body>"
echo "Sending $num_packets packets to $ip:$port...<br>"

./"$SEND_UDP_APPLICATION" -i "$ip" -p "$port" -n "$num_packets" > /dev/null 2>&1 &

echo "<p>Done!</p>"
echo "</body></html>"