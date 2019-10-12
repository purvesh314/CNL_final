#include <iostream>
#include <pcap.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

using namespace std;

void packetHandler(u_char *userData, const struct pcap_pkthdr* pkthdr, const u_char* packet);

int tcpcount = 0;
int icmpcount = 0;
int udpcount = 0;
int ipcount = 0 ;
int arpcount = 0 ;

int main(int argc, char **argv)
{   
	pcap_t* fp;
	char errbuf[PCAP_ERRBUF_SIZE];

	fp = pcap_open_offline("CapturedPackets.pcap", errbuf); //open .pcap file
	if (fp == NULL) 
	{
        	cout << "pcap_open_live() failed: " << errbuf << endl; //print errors (if any) in opening the file
        	return 1;
	}
	
	struct bpf_program filter;
	char filter_exp[] = "src 10.10.16.25";
	bpf_u_int32 ip;

	cout<<"Do you want to apply filter for SRC IP(192.168.7.5) ?";
	string ans;
	cin>>ans;
	if (ans =="yes")

	{

	    if (pcap_compile(fp, &filter, filter_exp, 0, ip) == -1) 
	    {
		cout << "Bad filter: " << pcap_geterr(fp);
		return 2;
	    }
	    if (pcap_setfilter(fp, &filter) == -1) 
	    {
		cout << "Error setting filter: " << pcap_geterr(fp);
		return 2;
	    }
	    if (pcap_loop(fp, 0, packetHandler, NULL) < 0) 
	    {

		cout << "pcap_loop() failed: " << pcap_geterr(fp);
		return 1;
	    }

	}
	else
	{   
	    if (pcap_loop(fp, 0, packetHandler, NULL) < 0) 
	    {

		cout << "pcap_loop() failed: " << pcap_geterr(fp);
		return 1;
	    }

	}

	cout<<"---Protocol Summary------\n";
	cout<<"ARP: "<<arpcount<<"\n";
	cout<<"IP Packets: "<<ipcount<<"\n";
	cout<<"TCP Packets: "<<tcpcount<<"\n";
	cout<<"UDP Packets: "<<udpcount<<"\n";
	cout<<"ICMP Packets: "<<icmpcount<<"\n";

	pcap_close(fp);
	return 0;
}

void packetHandler(u_char *userData, const struct pcap_pkthdr* pkthdr, const u_char* packet)
{
  const struct ether_header* ethernetHeader = (struct ether_header*)packet;
  const struct ip* ipHeader;

  if (ntohs(ethernetHeader->ether_type) == ETHERTYPE_IP) 
  {
      ipHeader = (struct ip*)(packet + sizeof(struct ether_header));

      if (ipHeader->ip_p == IPPROTO_TCP) 
      {
          tcpcount++;
      }
      else if(ipHeader->ip_p == IPPROTO_ICMP)
      { 
          icmpcount++;
      }
      else if(ipHeader->ip_p == IPPROTO_UDP)
      {
          udpcount++;
      }
      else if(ipHeader->ip_p == IPPROTO_IP)
      {
          ipcount++;
      }
      
  }
  if (ntohs(ethernetHeader->ether_type)== ETHERTYPE_ARP)
  {
      arpcount++;
  }

}
