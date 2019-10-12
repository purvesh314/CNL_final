import ipaddress
import math
import os
import subprocess as sp

def input_ipaddress():
    while True:
        try:
            ip = input('Enter the IP address: ')
            return ipaddress.ip_address(ip)
        except ValueError:
            print('Not a valid IP address !')

def get_ip_class_subnet(ip_address):
    ip_start = int(str(ip_address).split('.')[0])
    if ip_start >=1 and ip_start <= 126:
        ip_class = 'A'
        default_subnet = '255.0.0.0'
    elif ip_start >= 128 and ip_start <= 191:
        ip_class = 'B'
        default_subnet = '255.255.0.0'
    elif ip_start >= 192 and ip_start <= 223:
        ip_class = 'C'
        default_subnet = '255.255.255.0'
    elif ip_start >= 224 and ip_start <= 239:
        ip_class = 'D'
        default_subnet = '255.255.255.255'
    else:
        ip_class = 'E'
        default_subnet = None
    return ip_class, default_subnet

def ping(ip, start_ip_value, last_ip_value):
    status, result = sp.getstatusoutput("ping -c1 -w2 " + str(ip))
    if status == 0:
        print('Ping to', str(ip), 'was successful')
    elif int(str(ip).split('.')[-1]) < start_ip_value or int(str(ip).split('.')[-1]) > last_ip_value:
        print('System with IP', str(ip), 'is outside your subnetwork')
    else:
        print('Ping was unsuccessful due to other reason')

if __name__ == '__main__':
    print()
    ip_address = input_ipaddress()
    ip_class, default_subnet = get_ip_class_subnet(ip_address)

    if ip_class in 'ABC':
        print('\nClass of', ip_address, 'is', ip_class, '& Default Subnet mask is', default_subnet)
        number_of_subnets = int(input('\nEnter the number of subnets you want: '))
        bits_to_borrow = math.ceil(math.log(number_of_subnets, 2))
        print('Bits to borrow from the Host ID are', bits_to_borrow)

        # Find new subnet mask
        power = 7
        last_subnet_value = 0
        for i in range(bits_to_borrow):
            last_subnet_value += int(math.pow(2, power))
            power = power - 1
        splitted_subnet = default_subnet.split('.')
        subnet_mask = splitted_subnet[0] + '.' + splitted_subnet[1] + '.' + splitted_subnet[2] + '.' + str(last_subnet_value)
        print('\nNew Subnet mask is', subnet_mask)

        os.system('ifconfig wlo1 ' + str(ip_address) + ' netmask ' + str(subnet_mask)) # set the new static IP with new subnet_mask

        last_ip_part = int(str(ip_address).split('.')[-1])
        ip_split = int(256 / number_of_subnets)
        part_to_which_ip_belongs = 0

        # Find the range of IP that you belong to
        splitted_ip = str(ip_address).split('.')
        ip_start = splitted_ip[0] + '.' + splitted_ip[1] + '.' + splitted_ip[2]
        start_ip_value = 0
        last_ip_value = 0
        for i in range(number_of_subnets):
            start_ip_value = ip_split * i
            last_ip_value = start_ip_value + ip_split - 1
            if last_ip_part >= start_ip_value and last_ip_part <= last_ip_value:
                start_ip_range = ip_start + '.' + str(start_ip_value)
                last_ip_range = ip_start + '.' + str(last_ip_value)
                print('\nYour IP', ip_address, 'belongs to', start_ip_range, '-', last_ip_range)

                # Pinging to other machine
                print('\n--------------  Ping to other machine  ---------------')
                ip_address_other = input_ipaddress()
                ping(ip_address_other, start_ip_value, last_ip_value)
                print()
    else:
        print('Class D and E are not for public use')
