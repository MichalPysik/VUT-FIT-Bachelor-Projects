/*	ISA Project
	Variant 2: File transfer through a hidden channel

	Author: Michal Pyšík (login: xpysik00) */

#include "secret.hpp"




// prints help (program information, usage, etc.) to stdout
void print_help()
{
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Secret allows user to send encrypted files hidden inside ICMP ECHO_REQUEST/RESPONSE packets" << std::endl << std::endl;
	std::cout << "You can run Secret as client to send a file, or as server to save incoming files into the current directory" << std::endl << std::endl;
	std::cout << "Notes" << std::endl << " - Secret needs to be run with root privileges to work correctly" << std::endl;
	std::cout << " - Secret supports only IPv4 protocol (IPv6 is not supported)" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Usage: secret -r <file> -s <ip|hostname> [-l] [-v] [-h]" << std::endl << std::endl;
	std::cout << "Example (client): secret -r my_file -s 127.0.0.1" << std::endl;
	std::cout << "Example (client): secret -r ../images/cat.jpg -s merlin.fit.vutbr.cz -v" << std::endl << "Example (server): secret -l" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Options" << std::endl << std::endl << " -r <file>  <-- file to send as client" << std::endl;
	std::cout << " -s <ip|hostname>  <-- IPv4 address or hostname of the receiving server" << std::endl;
	std::cout << " -l  <-- runs the program in listen (server) mode" << std::endl;
	std::cout << " -v  <-- enables verbose mode (printing useful information to stdout)" << std::endl << " -h  <-- prints help" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

}




// handles SIGINT signal (CTRL + C) while using client
void handle_client_sigint(int s)
{
	if(verbose) std::cout << std::endl << "User interrupt, closing the client..." << std::endl;
	delete[] info_message;
	file.close();
	exit(0);
}




// handles SIGINT signal (CTRL + C) while using server
void handle_server_sigint(int s)
{
	if(verbose) std::cout << std::endl << "User interrupt, closing the server..." << std::endl;
	exit(0);
}




// resolves hostname to an IP address, while addressing different possible errors
int hostname_to_ip(char *hostname, struct sockaddr_in *ip_converted_struct)
{
	struct hostent *host;
	if( (host = gethostbyname(hostname)) == NULL )
	{
		switch(h_errno)
		{
			case HOST_NOT_FOUND:
				std::cerr << "Error: The entered hostname was not found" << std::endl;
				break;

			case TRY_AGAIN:
				std::cerr << "Error: The local server did not receive a response from an authorized server while resolving the entered hostname, you can try again later" << std::endl;
				break;

			case NO_RECOVERY:
				std::cerr << "Error: An Irrecoverable error has occured while resolving the entered hostname" << std::endl;
				break;

			case NO_DATA:
				std::cerr << "Error: The entered hostname does exists, but there is no corresponding IP address" << std::endl;
				break;

			default:
				std::cerr << "Error: An unknow error has occured while resolving the entered hostname" << std::endl;
				break;
		}
		std::cerr << "Note: If you meant to enter an IP address instead of a hostname, please make sure to use the correct format" << std::endl;
		return 1;
	}

	if(host->h_addrtype == AF_INET6)
	{
		std::cerr << "Error: The entered hostname was resolved to an IPv6 address, which is not supported by this program" << std::endl;
		return 1;
	}
	ip_converted_struct->sin_family = host->h_addrtype;
	memcpy(&(ip_converted_struct->sin_addr.s_addr), host->h_addr, host->h_length);

	return 0;
}




// calculates and returns ICMP header checksum, lenght excludes ethernet and IP header
uint16_t ICMP_checksum(uint16_t *icmp_hdr, int lenght)
{
	if(lenght < 0)
	{
		std::cerr << "Error: ICMP header checksum call error, negative ICMP header lenght" << std::endl;
		exit(1);
	}

	uint32_t sum = 0;
	uint16_t retval = 0;
	uint16_t odd;
	uint16_t *icmp_ptr = icmp_hdr;

	while(lenght > 1)
	{
		sum += *icmp_ptr++;
		lenght -= 2;
	}

	if(lenght == 1)
	{
		*(uint8_t*)(&odd) = *(uint8_t*)icmp_ptr;
		sum += odd;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	retval = ~sum;

	return retval;
}




// encrypts data using AES, expects 16-byte blocks
void encrypt_data(uint8_t *data, int data_size)
{
	AES_set_encrypt_key(key, 128, &aes_key);
	uint8_t *data_ptr = data;

	do {
		AES_encrypt(data_ptr, data_ptr, &aes_key);
		data_size -= 16;
		if(data_size > 0) data_ptr += 16;
	} while(data_size > 0);
}




// decrypts data using AES, expects 16-byte blocks
void decrypt_data(uint8_t *data, int data_size)
{
	AES_set_decrypt_key(key, 128, &aes_key);
	uint8_t *data_ptr = data;
	
	do {
		AES_decrypt(data_ptr, data_ptr, &aes_key);
		data_size -= 16;
		if(data_size > 0) data_ptr += 16;
	} while(data_size > 0);
}




// sends ICMP packet to specified receiver
/*
	Inspired by: https://stackoverflow.com/questions/9688899/sending-icmp-packets-in-a-c-program
	Zac, asked March 13th, 2012, "Sending ICMP packets it C program", Stack Overflow, Mar 13 '12
*/
int send_ICMP_packet(int *sock, struct sockaddr_in *ip_receiver_struct, char *data, int data_len, int ICMP_TYPE, uint16_t signature, uint16_t id_client, uint32_t sequence)
{
	char send_buffer[ sizeof(struct icmphdr) + sizeof(Secret_prot) + data_len ]; // buffer for outcoming packet
	memset(send_buffer, 0, sizeof(send_buffer));

	struct icmphdr *icmph = (struct icmphdr *)send_buffer; //ICMP structure points to start of buffer (ip header is excluded, made automatically)
	Secret_prot *sec_prot = (Secret_prot*)&send_buffer[sizeof(struct icmphdr)]; //same with our custom secret protocol

	//packet ICMP structure
	icmph->type = ICMP_TYPE; //ECHO (PING) REQUEST or REPLY
	icmph->code = 0; //sub-code 0

	//packet secret protocol structure
	sec_prot->signature = signature; //client or server signature
	sec_prot->id_client = id_client; //client's assigned id from the server
	sec_prot->sequence = sequence; //sequence (number of file data packet) or 0 for new request
	
	memmove(&send_buffer[sizeof(struct icmphdr) + sizeof(Secret_prot)], data, data_len); //copying data into packet

	icmph->checksum = ICMP_checksum( (uint16_t *)icmph, sizeof(send_buffer) ); //ICMP packet checksum

	//send the packet
	if( sendto(*sock, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *)ip_receiver_struct, sizeof(*ip_receiver_struct)) < 0 )
	{
		std::cerr << "Error: There was an error while sending ICMP packet, errno: " << errno << std::endl;
		return 1;
	}

	return 0;
}




// function for client-side usage
int client_side(std::string filename, sockaddr_in *ip_receiver_struct)
{
	signal(SIGINT, handle_client_sigint); //set the client SIGINT signal handler

	int sock;
	if( (sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0 ) //create raw ICMP socket
	{
		std::cerr << "Error: There was an error while creating client's socket" << std::endl;
		std::cerr << "Note: Make sure you run the program with root privileges!" << std::endl;
		return 1;
	}

	file.seekg(0, file.end);
	uint32_t chars_total = file.tellg(); //find out number of characters in file

	uint32_t chars_remain = chars_total;
	uint32_t nof_packets = chars_total/MAX_DATA_SIZE + (chars_total % MAX_DATA_SIZE != 0); //integer ceil

	char* data;
	info_message = new char[4 + filename.length()]; //first message is FILE_BYTE_COUNT(uint32) followed by FILENAME
	(uint32_t&)*info_message = htonl(chars_total); //first 4 bytes are total characters (bytes) in file
	strncpy(&info_message[4], filename.c_str(), filename.length()); //the rest is filename
	
	uint16_t current_read;
	char recv_buffer[MAX_PACKET_SIZE];
	memset(recv_buffer, 0, MAX_PACKET_SIZE);
	uint16_t my_id = 0;
	uint32_t packet_seq = 0; //sequence number of packet
	uint32_t packets_sent = 0; //total number of packets

	struct sockaddr_in server_addr;
	socklen_t server_addr_len = sizeof(struct sockaddr_in);
	struct icmphdr *icmph = (struct icmphdr*)&recv_buffer[sizeof(struct ip)];
	Secret_prot *sec_prot = (Secret_prot*)&recv_buffer[sizeof(struct ip) + sizeof(struct icmphdr)];
	char *data_recv = &recv_buffer[ sizeof(struct ip) + sizeof(struct icmphdr) + sizeof(Secret_prot) ];

	//send info packet (first packet containing only total file size and file name)
	if( send_ICMP_packet(&sock, ip_receiver_struct, info_message, 4+filename.length(), ICMP_ECHO, CLIENT_SIGNATURE, 0, 0) )
	{
		delete[] info_message;
		file.close();
		return 1;
	}
	if(verbose) std::cout << "Sent request to the server - file: " << filename << " - server: " << inet_ntoa(ip_receiver_struct->sin_addr) << std::endl;

	while(true) //waiting for server response
	{
		recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&server_addr, &server_addr_len);
		if( icmph->type == ICMP_ECHOREPLY && sec_prot->signature == SERVER_SIGNATURE_OK && sec_prot->sequence == 0 ) //server is ready, he gave us new ID
		{
			if(sec_prot->id_client == 0) //the server is busy (all IDs are in use)
			{
				file.close();
				delete info_message;
				if(verbose) std::cout << "The server is serving too many clients right now, please try again later!" << std::endl;
				return 0;
			}

			if( memcmp(info_message, data_recv, 4+filename.length()) ) continue; //server has to reply with same message, only IP and ICMP IDs should change
			my_id = sec_prot->id_client;
			if(verbose) std::cout << "Server's response received, sending the file..." << std::endl << std::endl;
			break;
		}
	}
		
	uint16_t extra_bytes; // (data_bytes % 16) to calculate extra padding for encryption (if needed)
	chars_remain = chars_total;
	file.seekg (0, file.beg); //go to the beginning of file
	packet_seq = 1;
	packets_sent = 1;
	bool corrupted = false;

	while(chars_remain > 0) //read the file and send in packets, max block size is MAX_DATA_SIZE
	{
		current_read = chars_remain < MAX_DATA_SIZE ? chars_remain : MAX_DATA_SIZE;

		if( (extra_bytes = (current_read % 16)) ) //last packet must be padded if data-size not divisible by 16
		{
			data = new char[ current_read + 16 - extra_bytes ];
			memset( &data[current_read], 0, 16 - extra_bytes ); //zero byte padding
		}
		else data = new char[current_read];
		
		file.read(data, current_read); //read the file, encrypt it in the data buffer
		encrypt_data( (uint8_t*)data, current_read + (extra_bytes ? (16 - extra_bytes) : 0) );

		while(true) //send the file data packet until receiving confirmation that it is not corrupted
		{
			if( send_ICMP_packet(&sock, ip_receiver_struct, data, extra_bytes ? (current_read + 16 - extra_bytes) : current_read, ICMP_ECHO, CLIENT_SIGNATURE, my_id, packet_seq) )
			{
				delete[] data;
				delete[] info_message;
				file.close();
				return 1;
			}
			if(verbose) std::cout << "Sent data packet " << packets_sent << "/" << nof_packets << "..." << std::endl;
			while(true)
			{
				if( recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&server_addr, &server_addr_len) < (uint32_t)(sizeof(struct ip) + sizeof(struct icmphdr) + sizeof(Secret_prot)) ) continue;
				if(sec_prot->signature == SERVER_SIGNATURE_OK && icmph->type == ICMP_ECHOREPLY && sec_prot->id_client == my_id && sec_prot->sequence == packet_seq)
				{
					//server received whole packet
					if(verbose) std::cout << "Server confirmed receiving correct data packet " << packets_sent << "/" << nof_packets << std::endl;
					corrupted = false;
					break;
				}
				else if(sec_prot->signature == SERVER_SIGNATURE_CORRUPTED && icmph->type == ICMP_ECHOREPLY && sec_prot->id_client == my_id && sec_prot->sequence == packet_seq)
				{
					//server received corrupted packet
					if(verbose) std::cout << "Server received corrupted data packet " << packets_sent << "/" << nof_packets << ", resending..." << std::endl;
					corrupted = true;
					break;
				}
			}
			if(!corrupted) break;
		}

		delete[] data;
		chars_remain -= (uint32_t)current_read; //substract sent file characters from remaining characters
		packets_sent++;
		packet_seq++;
	}

	if(verbose) std::cout << std::endl << "Success!" << std::endl << "The entire file: " << filename << " has been received by: " << inet_ntoa(ip_receiver_struct->sin_addr) << std::endl;


	delete[] info_message;
	file.close();
	return 0;
}




// function for server-side usage (listen mode)
int server_side()
{
	signal(SIGINT, handle_server_sigint); //set the server SIGINT signal handler

	int sock;
	if( (sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0 ) //create raw ICMP socket
	{
		std::cerr << "Error: There was an error while creating server's socket" << std::endl;
		std::cerr << "Note: Make sure you run the program with root privileges!" << std::endl;
		return 1;
	}

	if(verbose) std::cout << "Server is running..." << std::endl;

	int bytes_received, data_received, padding_bytes;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(struct sockaddr_in);
	uint16_t next_client_id = 1;
	uint16_t checksum_check;

	char recv_buffer[MAX_PACKET_SIZE]; //buffer for received data
	struct icmphdr *icmph = (struct icmphdr *)&recv_buffer[sizeof(struct ip)]; //ICMP structure pointer to corresponding part of buffer
	Secret_prot *sec_prot = (Secret_prot *)&recv_buffer[sizeof(struct ip) + sizeof(struct icmphdr)]; //same with secret protocol struct pointer
	char* empy_data = NULL; //server usually communicates without sending any additional data
	char* server_message; //data server sends in first confirm message (otherwise sends only empty data-part packets)
	char* data; //data part of incoming packet from client

	Client_request clients_info[MAX_CLIENT_ID]; //array of structs containing info about clients
	for(int i = 0; i < MAX_CLIENT_ID; i++) clients_info[i].occupied = false; //every id is available at start

	while(true)
	{
		bytes_received = recvfrom(sock, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);
		data_received = bytes_received - sizeof(struct ip) - sizeof(struct icmphdr) - sizeof(Secret_prot);
		data = &recv_buffer[ sizeof(struct ip) + sizeof(struct icmphdr) + sizeof(Secret_prot) ];

		//new request - we need to assign available ID to new client
		if(icmph->type == ICMP_ECHO && sec_prot->signature == CLIENT_SIGNATURE && sec_prot->id_client == 0 && sec_prot->sequence == 0 && data_received > 0)
		{
			if(clients_info[next_client_id-1].occupied) //if the next ID is occupied...
			{
				for(int i = 0; i < (MAX_CLIENT_ID - 1); i++) //...then cycle through the remaining ones...
				{
					next_client_id = (next_client_id % MAX_CLIENT_ID) + 1;
					if(!clients_info[next_client_id-1].occupied) break;
				}
				if(clients_info[next_client_id-1].occupied) //...and if none is available, inform the client that the server is busy (sending secret protocol id=0 packet)
				{
					if( send_ICMP_packet(&sock, &client_addr, empy_data, 0, ICMP_ECHOREPLY, SERVER_SIGNATURE_OK, 0, 0) ) return 1;
					if(verbose) std::cout << std::endl << "Incoming request - client: " << inet_ntoa(client_addr.sin_addr) << ", but there is no available ID, informing the client that the server is busy..." << std::endl << std::endl;
					continue;
				}
			}
			
			clients_info[next_client_id-1].expected_seq = 1; //we expect first data packet
			clients_info[next_client_id-1].packets_received = 0; //we received 0 data packets so far
			clients_info[next_client_id-1].expected_file_bytes = htonl( *(uint32_t*)data ); //save total expected number of bytes in file
			//calculate expected number of packets
			clients_info[next_client_id-1].expected_packet_count = clients_info[next_client_id-1].expected_file_bytes/MAX_DATA_SIZE + (clients_info[next_client_id-1].expected_file_bytes % MAX_DATA_SIZE != 0);
			if(clients_info[next_client_id-1].expected_packet_count == 0) clients_info[next_client_id-1].expected_packet_count = 1; //edge case for receiving empty file
			clients_info[next_client_id-1].filename = new char[data_received - 3]; // -4 for packetcount, +1 for terminating character
			clients_info[next_client_id-1].filename[data_received - 4] = '\0'; //adding terminating character at the end of filename
			strncpy( clients_info[next_client_id-1].filename, &data[4], data_received - 4 ); //save incoming filename
			clients_info[next_client_id-1].file.open(clients_info[next_client_id-1].filename); //open file for write
			clients_info[next_client_id-1].occupied = true; //set the assigned client ID as occupied

			server_message = new char[ data_received ];
			memcpy( server_message, data, data_received ); //copy the total number of file bytes and filename, and send it back to client to confirm

			if( send_ICMP_packet(&sock, &client_addr, server_message, data_received, ICMP_ECHOREPLY, SERVER_SIGNATURE_OK, next_client_id, 0) )
			{
				clients_info[next_client_id-1].file.close();
				delete[] server_message;
				return 1;
			}
			delete[] server_message;
			if(verbose) std::cout << std::endl << "Confirmed incoming request - client: " << inet_ntoa(client_addr.sin_addr) << " - file: " << clients_info[next_client_id-1].filename << std::endl << std::endl;

			next_client_id = (next_client_id % MAX_CLIENT_ID) + 1; //increment next id to serve another client
		}
		else if(icmph->type == ICMP_ECHO && sec_prot->signature == CLIENT_SIGNATURE && sec_prot->id_client > 0 && sec_prot->id_client <= MAX_CLIENT_ID && data_received )
		{
			//data packet from some client
			if( clients_info[sec_prot->id_client-1].expected_seq == sec_prot->sequence && clients_info[sec_prot->id_client-1].file.is_open() )
			{
				//correct expected sequence from valid client
				checksum_check = icmph->checksum; //copy checksum from packet
				icmph->checksum = 0; //set it to 0 to repeat the original checksum calculation
				if( checksum_check == ICMP_checksum((uint16_t *)icmph, bytes_received - sizeof(struct ip)) )
				{
					//received whole (uncorrupted) packet
					clients_info[sec_prot->id_client-1].packets_received++;
					decrypt_data( (uint8_t*)data, data_received ); //decrypt received data
					if(verbose) std::cout << "Received correct data packet " << clients_info[sec_prot->id_client-1].packets_received << "/" << clients_info[sec_prot->id_client-1].expected_packet_count << " - from: " << inet_ntoa( client_addr.sin_addr ) << " - file: " << clients_info[sec_prot->id_client-1].filename << std::endl;

					//sending packet ok reply
					if( send_ICMP_packet(&sock, &client_addr, empy_data, 0, ICMP_ECHOREPLY, SERVER_SIGNATURE_OK, sec_prot->id_client, sec_prot->sequence) ) return 1;

					if( clients_info[sec_prot->id_client-1].packets_received == clients_info[sec_prot->id_client-1].expected_packet_count )
					{
						//final data packet from this client
						padding_bytes = ( (clients_info[sec_prot->id_client-1].expected_file_bytes % 16) ? (16 - (clients_info[sec_prot->id_client-1].expected_file_bytes % 16)) : 0 );
						clients_info[sec_prot->id_client-1].file.write( &recv_buffer[ sizeof(struct ip) + sizeof(struct icmphdr) + sizeof(Secret_prot) ], data_received - padding_bytes ); //write to file excluding the padding bytes

						if(verbose) std::cout << std::endl << "File transfer complete! - client: " << inet_ntoa( client_addr.sin_addr ) << " - file: " << clients_info[sec_prot->id_client-1].filename << std::endl << std::endl;
						clients_info[sec_prot->id_client-1].file.close();
						delete[] clients_info[next_client_id-1].filename;
						clients_info[sec_prot->id_client-1].occupied = false;
					}
					else
					{
						//not last packet from this client
						clients_info[sec_prot->id_client-1].file.write( &recv_buffer[ sizeof(struct ip) + sizeof(struct icmphdr) + sizeof(Secret_prot) ], data_received ); //write to file
						clients_info[sec_prot->id_client-1].expected_seq++; //server will be expecting another packet from this client
					}
				}
				else
				{
					//received corrupted data packet from client
					if(verbose) std::cout << "Received corrupted data packet " << (clients_info[sec_prot->id_client-1].packets_received + 1) << "/" << clients_info[sec_prot->id_client-1].expected_packet_count << " - from: " << inet_ntoa( client_addr.sin_addr ) << " - file: " << clients_info[sec_prot->id_client-1].filename << std::endl;
					
					//sending packet corrupted reply
					if( send_ICMP_packet(&sock, &client_addr, empy_data, 0, ICMP_ECHOREPLY, SERVER_SIGNATURE_CORRUPTED, sec_prot->id_client, sec_prot->sequence) ) return 1;
					
					if(verbose) std::cout << "Asking for resend..." << std::endl;
				}
			} //valid expected sequence if statement
		} //serving ongoing file-transfer if statement
	} //main while loop

	return 0;
}




int main(int argc, char *argv[])
{
	// variables for option arguments
	std::string filename;
	struct sockaddr_in ip_receiver_struct;

	// listen (server) mode toggle
	bool listen_mode = false;

	// command line options/arguments parsing
	int c;
	int client_opcontrol = 0; //checks for duplicate client-side options (prime number trick)
	while( (c = getopt(argc, argv, ":r:s:lvh")) != -1)
	{
		switch(c)
		{
			case 'r':
				client_opcontrol += 7;
				file.open(optarg);
				if(!file)
				{
					std::cerr << "Error: File \'" << optarg << "\' does not exist (or you do not have sufficient permissions to read it)" << std::endl;
					return 1;
 				}
				filename = optarg;
				filename = filename.substr(filename.find_last_of("/\\") + 1); //convert filepath to filename
				break;

			case 's':
				client_opcontrol += 3;
				if(!inet_pton(AF_INET, optarg, &ip_receiver_struct.sin_addr)) //valid IPv4 address??
				{
					if(hostname_to_ip(optarg, &ip_receiver_struct)) return 1; //function prints stderr error message already
				}
				ip_receiver_struct.sin_family = AF_INET;
				break;

			case 'l':
				listen_mode = true;
				break;

			case 'v':
				verbose = true;
				break;	

			case 'h':
				print_help();
				if(file.is_open()) file.close();
				return 0;

			default:
				std::cerr << "Error: Invalid option(s) entered" << std::endl << "Note: run the program with '-h' option to check the correct usage" << std::endl;
				if(file.is_open()) file.close();
				return 1;
		}
	}

	if(listen_mode)
	{
		if(file.is_open()) file.close();
		return server_side();
	}
	else if(client_opcontrol == 0)
	{
		std::cerr << "Error: No options selected" << std::endl;
		std::cerr << "Note: Run the program with '-h' option to check the correct usage" << std::endl;
		return 1;
	}
	else if(client_opcontrol != 10)
	{
		std::cerr << "Error: You have to use both \"-r <file>\" and \"-s <ip|hostname>\" options (each exactly once) to run the program as client" << std::endl;
		std::cerr << "Note: Run the program with '-h' option to check the correct usage" << std::endl;
		return 1;
	}
	else return client_side(filename, &ip_receiver_struct);
}
