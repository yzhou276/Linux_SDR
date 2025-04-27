#!/bin/bash
STREAMING_APPLICATION=udpFifoStreamer2
killall -9 "$STREAMING_APPLICATION"
echo "Content-type: text/html" # Tells the browser what kind of content to expect
echo "" # An empty line. Mandatory, if it is missed the page content will not load
echo "<html><body><p><em>"
echo "loading PL...<br>"
fpgautil -b design_1_wrapper.bit.bin
echo "</p></em><p>"
echo "configuring Codec...<br>"
./configure_codec.sh
echo "</p>"
echo "initializing the UDP Fifo streaming engine...<br>"
echo "Destination IP address: 192.168.1.3<br>"
echo "Destination UDP port: 25344<br>"
./"$STREAMING_APPLICATION" &
echo "<p><em>All Done!</em></p>" 
echo "</body></html>"
