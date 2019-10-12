#!/usr/bin/env python
# coding: utf-8

# In[145]:


import re, math


# In[146]:


def separate_octets(ip):
    octets = re.split('(.*)\.(.*)\.(.*)\.(.*)', ip)
    octets.pop(0)
    octets.pop(-1)
    octets = [int(i) for i in octets]
    return octets


# In[147]:


def get_class(list_oct):
    if(list_oct[0] >= 240):
        class_ip = 'E'
    elif(list_oct[0] >= 224):
        class_ip = 'D'
    elif(list_oct[0] >= 192):
        class_ip = 'C'
    elif(list_oct[0] >= 128):
        class_ip = 'B'
    else:
        class_ip = 'A'
    return class_ip


# In[148]:


def get_net_id(list_oct, cl):
    net_id = ""
    if(cl == 'A'):
        net_id += str(list_oct[0])
    elif(cl == 'B'):
        net_id += str(list_oct[0]) + '.' + str(list_oct[1])
    else:
        net_id += str(list_oct[0]) + '.' + str(list_oct[1]) + '.' + str(list_oct[2])
    return net_id


# In[149]:


def get_broadcast(net_id, cl):
    b_id = ''
    if(cl == 'A'):
        b_id += net_id + '.255.255.255'
    elif(cl == 'B'):
        b_id += net_id + '.255.255'
    else:
        b_id += net_id + '.255'
    return b_id


# In[150]:


def get_host_id(list_oct, cl):
    net_id = ""
    if(cl == 'A'):
        net_id += str(list_oct[1]) + '.' + str(list_oct[2]) + '.' + str(list_oct[3])
    elif(cl == 'B'):
        net_id += str(list_oct[2]) + '.' + str(list_oct[3])
    else:
        net_id += str(list_oct[3])
    return net_id


# In[151]:


def get_subnet_mask(net_id, cl, add_fact):
    sm = ''
    if(cl == 'A'):
        sm += '255.' + str(add_fact) + '.0.0'
    elif(cl == 'B'):
        sm += '255.255.' + str(add_fact) + '.0'
    else:
        sm += '255.255.255.' + str(add_fact)
    return sm


# In[152]:


def get_net_addr(net_id, cl):
    net_addr = ''
    if(cl == 'A'):
        net_addr += net_id + '.0.0.0'
    elif(cl == 'B'):
        net_addr += net_id + '.0.0'
    else:
        net_addr += net_id + '.0'
    return net_addr


# In[153]:


def make_ip(list):
    result= ''
    for element in list:
        result += '.' + str(element)

    return result[1:]


# In[156]:


def find_sub_ids(i, dist, cl, inp):
    results = []
    if(cl == 'A'):
        sub_id = []
        brod_id = []
        sub_id = inp[:1]
        brod_id = inp[:1]
        sub_id[1].append(0 + i * dist)
        results.append(sub_id)
        brod_id[1].append(sub_id[1] + dist - 1)
        brod_id[2].append(255)
        brod_id[3].append(255)
        results.append(brod_id)
    elif(cl == 'B'):
        sub_id = []
        brod_id = []
        sub_id = inp[:2]
        brod_id = inp[:2]
        sub_id.append(0 + i * dist)
        results.append(sub_id)
        brod_id.append(sub_id[2] + dist - 1)
        brod_id.append(255)
        results.append(brod_id)
    else:
        sub_id = []
        brod_id = []
        sub_id = inp[:3]
        brod_id = inp[:3]
        sub_id.append(0 + (i * dist))
        results.append(sub_id)
        brod_id.append(sub_id[3] + dist - 1)
        results.append(brod_id)
    return results 


# In[158]:


valid = True

count = 0
while(valid):
    ip = input('Enter IP: ')

    octets = separate_octets(ip)

    for octet in octets:
        if(octet < 0 or octet > 255):
            print('Invalid IP')
            count = 0
        else:
            count = count + 1
    if(count == 4):
        valid = False

sub = int(input('Enter the number of subnets: '))

cl = get_class(octets)
print('IP belongs to class', cl)

if(cl in ['A', 'B', "C"]):
    net_id = get_net_id(octets, cl)
    print('Net ID:', net_id)
    
    host_id = get_host_id(octets, cl)
    print('Host ID:', host_id)

    print('___________________________')
    print('For the network:')
    net_addr = get_net_addr(net_id, cl)
    print('Network ID:', net_addr)
    
    broadcast_id = get_broadcast(net_id, cl)
    print('Broadcast ID:', broadcast_id)

    p = int(math.log(sub, 2))
    sub_id = 0
    for i in range(p):
        sub_id += math.pow(2, 7-i)
    sub_id = int(sub_id)

    subnet_mask = get_subnet_mask(net_id, cl, sub_id)
    print('Subnet Mask:',subnet_mask)
    
    fact = 1
    while(fact < sub):
        fact = fact * 2
    dist = int(256 / fact)
    net_octet = separate_octets(net_addr)
    for i in range(sub):
        ids = find_sub_ids(i, dist, cl, net_octet)
        
        print('___________________________')
        print('Range of Subnet', i, ':')
        print('Subnet ID:', make_ip(ids[0]))
        print('Broadcast ID:', make_ip(ids[1]))


# In[ ]:




