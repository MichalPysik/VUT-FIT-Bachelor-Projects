/*	ISA Project
	Variant 2: File transfer through a hidden channel

	Author: Michal Pyšík (login: xpysik00) */

#ifndef ISA_SECRET_HPP
#define ISA_SECRET_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <openssl/aes.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define MAX_PACKET_SIZE 1500
#define MAX_DATA_SIZE 1440 //Extra space for IP + ICMP + secret_prot headers and padding
#define CLIENT_SIGNATURE 0xaaaa //signature of every packet coming from client
#define SERVER_SIGNATURE_OK 0xbbbb //signature of every packet coming from server, excluding corrupted file packet message
#define SERVER_SIGNATURE_CORRUPTED 0xcccc //signature of packet coming from server, asking the client to resend corrupted data packet
#define MAX_CLIENT_ID 32 //maximum active clients at any single time instance

static const uint8_t key[] = "xpysik00"; //key for (en/de)cryption
AES_KEY aes_key; //saved padded key for AES calculations
std::ifstream file; //client's opened file - global for SIGINT signal handling
char *info_message; //client's first message - global for SIGINT signal handling
bool verbose; //verbose mode - prints useful information to stdout

// structure for custom secret protocol header
typedef struct secret_prot
{
	uint16_t signature;
	uint16_t id_client;
	uint32_t sequence;
} Secret_prot;

// server-size structure for saving info for incoming client requests
typedef struct client_request
{
	char* filename;
	std::ofstream file;
	uint32_t expected_file_bytes;
	uint32_t expected_packet_count;
	uint32_t expected_seq;
	uint32_t packets_received;
	bool occupied;
} Client_request;


// prints help (program information, usage, etc.) to stdout
void print_help();

// handles SIGINT signal (CTRL + C) while using client
void handle_client_sigint(int s);

// handles SIGINT signal (CTRL + C) while using server
void handle_server_sigint(int s);

// resolves hostname to an IP address, while addressing different possible errors
int hostname_to_ip(char *hostname, struct sockaddr_in *ip_converted_struct);

// calculates and returns ICMP header checksum, lenght excludes IP header
uint16_t ICMP_checksum(uint16_t *icmp_hdr, int lenght);

// encrypts data using AES, expects 16-byte blocks
void encrypt_data(uint8_t *data, int data_size);

// decrypts data using AES, expects 16-byte blocks
void decrypt_data(uint8_t *data, int data_size);

// sends ICMP packet to specified receiver
int send_ICMP_packet(int *sock, struct sockaddr_in *ip_receiver_struct, char *data, int data_len, int ICMP_TYPE, uint16_t signature, uint16_t id_client, uint32_t sequence);

// function for client-side usage
int client_side(std::string filename, sockaddr_in *ip_receiver_struct);

// function for server-side usage (listen mode)
int server_side();

// the main function
int main(int argc, char *argv[]);


#endif
