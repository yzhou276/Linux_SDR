# file: udpReceiver.py
# author: Yuchen Zhou (yzhou276@jh.edu)
# This script receives UDP packets
# data 2025-04-15

import socket
import argparse
import struct
import select 

PACKET_SIZE = 4 + 512 * 2  # uint32_t + 512 int16_t

def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description="UDP Receiver")
    parser.add_argument("-p", "--port", type=int, required=True, help="Port to listen on")
    args = parser.parse_args()

    # Create a UDP socket
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    except socket.error as e:
        print(f"Error creating socket: {e}")
        exit()

    # Bind the socket to the host and port
    try:
        sock.bind(("0.0.0.0", args.port))
        sock.setblocking(False)
    except socket.error as msg:
        print(f"Bind failed. Error: {msg}")
        exit()

    print(f"Listening for UDP packets on port {args.port}...")

    try:
        while True:
            # 100ms timeout
            ready_socks, _, _ = select.select([sock], [], [], 0.1)  
            if sock in ready_socks:
                data, addr = sock.recvfrom(PACKET_SIZE)
                if len(data) != PACKET_SIZE:
                    continue

                packet_id = struct.unpack_from("<I", data, 0)[0]
                samples = struct.unpack_from("<512h", data, 4)
                print(f"Received packet ID: {packet_id}")
    except KeyboardInterrupt:
        pass
    finally:
        sock.close()

if __name__ == "__main__":
    main()