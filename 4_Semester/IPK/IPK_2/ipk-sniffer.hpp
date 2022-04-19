/*	IPK Projekt 2
	Varianta ZETA: Sniffer paketů

	Autor: Michal Pyšík (login: xpysik00) */

#ifndef IPK_SNIFFER_HPP
#define IPK_SNIFFER_HPP

#include <iostream>
// C++ std::string
#include <string>
// C string.h
#include <string.h>
#include <time.h>
#include <signal.h>
#include <getopt.h>
#include <pcap/pcap.h>
#include <net/ethernet.h>
#include <linux/if_arp.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>



// definice kvuli citelnejsim long options v getopt
#define no_argument 0
#define required_argument 1


// Globalni promenne
bool port_specified = false;
int port;
char which_packets = 0b0000; // uvazujeme jen spodni 4 bity, prvni z nich zleva urcuje zda chceme TCP, druhy UDP, apod.
int how_many_packets = 1;
pcap_t *interface = nullptr; // otevreny interface, globalni kvuli user interrupt eventu (CTRL + C)
bool interface_any = false; // uzivatel muzi vybrat pseudo-interface 'any', ktery ma 16 B hlavicku misto ethernet 14 B



// vytiskne vsechna dostupna rozhrani, vraci 0 pri uspechu, 1 pri chybe
int print_interfaces();


// vytiskne datum a cas ve formatu RFC3339
void print_RFC3339(const struct pcap_pkthdr *header);

// vytiskne cely obsah packetu, vcetne ethernet hlavicky, IP hlavicky, atd. (kompletne vsechna data)
void print_raw_data(const u_char * Buffer, int Size);


// Nasledujici 4 funkce volaji predesle 2 funkce ve svem tele

// Zpracuje TCP packet, implicitne IPv4, lze prepnout nepovinnym parametrem na IPv6
void process_TCP(const u_char * Buffer, int Size, const struct pcap_pkthdr *header, bool IPv6_mode = false);

// Zpracuje UDP packet, volitelny IPv6 mode
void process_UDP(const u_char * Buffer, int Size, const struct pcap_pkthdr *header, bool IPv6_mode = false);

// Zpracuje ICMP packet, volitelny ICMPv6 mode
void process_ICMP(const u_char * Buffer, int Size, const struct pcap_pkthdr *header, bool IPv6_mode = false);

// Zpracuje ARP, cte 6 bytove MAC adresy primo z ethernet headeru
void process_ARP(const u_char * Buffer, int Size, const struct pcap_pkthdr *header);


// Rozhodne o typu protokolu a protokolu daneho packetu, rozhodne kterou funkci volat na zpracovani
void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer);


// Zpracovava user interruption event (CTRL + C), kvuli dealokaci pameti
void on_user_interrupt(int s);


// hlavni funkce main
int main(int argc, char *argv[]);



#endif
