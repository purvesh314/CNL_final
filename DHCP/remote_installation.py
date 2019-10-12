import os
import re
from scapy.all import srp, Ether, ARP

def input_macaddress():
    while True:
        mac_address = input('Please enter a MAC address: ')
        valid = re.match('(?=[a-f0-9]{2}:){5}[a-f0-9]{2}', mac_address, re.I) # Match the pattern of MAC address
        if valid:
            return mac_address
        else:
            print('Not a valid MAC address')

mac_address = input_macaddress() # Input the MAC address
conf.verb = 0 # To stop verbose output of arp from scapy
ans, unans = srp(Ether(dst = "ff:ff:ff:ff:ff:ff") / ARP(pdst = "10.10.10.0/24"), timeout = 2, iface = "p1p1") # The actual arp request on wlo1 interface with 192.168.1.0/24 range
ip_address = None

# Run through all the received entries in arp request and match the mac address
for send, receive in ans:
    if receive.sprintf(r'%Ether.src%') == mac_address: # Match the MAC address
        ip_address = receive.sprintf(r'%ARP.psrc%')

if ip_address is None:
    print('MAC Address not found')
else:
    print('IP address for', mac_address, 'is', ip_address)
    os.system('ssh -Y root@' + ip_address + ' yum install -y -q nasm')
