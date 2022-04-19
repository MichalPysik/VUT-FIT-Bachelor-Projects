/*	IPK Projekt 2
	Varianta ZETA: Sniffer paketů

	Autor: Michal Pyšík (login: xpysik00) */

#include "ipk-sniffer.hpp"



int print_interfaces()
{
	pcap_if_t *all_devs;
	pcap_if_t *curr_dev;
	char errbuf[PCAP_ERRBUF_SIZE];

	if(pcap_findalldevs(&all_devs, errbuf) == -1)
	{
		std::cerr << "Error with finding active interfaces: " << errbuf << std::endl;
		return 1;
	}

	for(curr_dev = all_devs; curr_dev != nullptr; curr_dev = curr_dev->next)
	{
		std::cout << curr_dev->name << std::endl;
	}

	pcap_freealldevs(all_devs);
	return 0;
}




void print_RFC3339(const struct pcap_pkthdr *header)
{
	struct tm *tm = localtime(&header->ts.tv_sec); // ulozime unixovy timestamp v sekundach
	char date_time[20];
	strftime(date_time, sizeof(date_time), "%FT%T", tm); // YYYY-MM-DD'T'HH-MM-SS format na datum a cas
	char timezone[7];
	strftime(timezone, sizeof(timezone), "%z", tm); // Casove pasmo
	timezone[6] = '\0';
	timezone[5] = timezone[4];
	timezone[4] = timezone[3];
	timezone[3] = ':'; // z formatu '+ABCD' udelame '+AB:CD'

	int milisecs = (int)(header->ts.tv_usec / 1000); // prevest mikrosekundy na milisekundy a zaokrouhlit (na integer)

	// pri tisku vyplnujeme pripadne milisekundy zleva nulami (na 3 cislice)
	std::cout << date_time << "." << (milisecs < 100 ? "0" : "") << (milisecs < 10 ? "0" : "") << milisecs << timezone << " ";
}




void print_raw_data(const u_char * Buffer, int Size)
{
	u_char *data = (u_char *)(Buffer);

	for(int i = 0; i < Size; i += 16)
	{
		printf("0x%.4x:  ", i);

		for(int j = i; j < (i + 16); j++) // tisk HEXA bytu
		{
			if(j >= Size)
			{
				for(int z = 0; z < (i + 16 - j); z++) std::cout << "   "; // 3 mezery za kazdy chybejici byte do 16
				if( (j - i) < 8 ) std::cout << " "; // jeste extra mezera kdyz konci v prvnich 8 bytech
				break;
			}
			printf("%.2x ", data[j]);
			if(j == i+7) std::cout << " ";
		}

		std::cout << " "; //extra mezera, bytu nemusi byt nutne 16 tak ji tiskneme az zde

		for(int j = i; j < (i + 16); j++) // tisk ASCII bytu
		{
			if(j >= Size) break;
			if(isprint(data[j])) printf("%c", data[j]);
			else std::cout << ".";
			if(j == i+7) std::cout << " ";
		}

		std::cout << std::endl;
	}
}




void process_TCP(const u_char * Buffer, int Size, const struct pcap_pkthdr *header, bool IPv6_mode)
{
	struct sockaddr_in source, dest; // struktury pro IP odesilatele a prijemce v4
	char v6buffer[128]; // pro IPv6 je proces odlisny, inet_ntop potrebuje extra buffer

	struct iphdr *ip_header;
	struct ip6_hdr *ip6_header;
	unsigned short iph_len;

	if(!IPv6_mode) // IPv4
	{
		memset(&source, 0, sizeof(source));
		memset(&dest, 0, sizeof(dest));
		ip_header = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
		iph_len = ip_header->ihl*4;
		source.sin_addr.s_addr = ip_header->saddr;
		dest.sin_addr.s_addr = ip_header->daddr;
	}
	else // IPv6
	{
		ip6_header = (struct ip6_hdr *)( Buffer  + sizeof(struct ethhdr) );
		iph_len = 40; // delka IPv6 hlavicky je vzdy 40 bytu
	}

	struct tcphdr *tcp_header = (struct tcphdr*)(Buffer + iph_len + sizeof(struct ethhdr));

	if(port_specified) //pokud je specifikovan port, ukoncime funkci pokud mu neodpovida prijemce ani odesilatel
	{
		if( ntohs(tcp_header->source) != port && ntohs(tcp_header->dest) != port ) return;
	}

	std::cout << std::endl;
	print_RFC3339(header);

	std::cout << (IPv6_mode ? inet_ntop(AF_INET6, &ip6_header->ip6_src, v6buffer, 128) : inet_ntoa(source.sin_addr)) << " : " << ntohs(tcp_header->source) << " > "; // IP : port odesilatele
	std::cout << (IPv6_mode ? inet_ntop(AF_INET6, &ip6_header->ip6_dst, v6buffer, 128) : inet_ntoa(dest.sin_addr)) << " : " << ntohs(tcp_header->dest) << ", "; // IP : port prijemce
	std::cout << "length " << Size << " bytes" << std::endl << std::endl;

	print_raw_data(Buffer, Size);
	std::cout << std::endl;

	--how_many_packets; //dekrementace pocitadla zpracovanych packetu
}




void process_UDP(const u_char * Buffer, int Size, const struct pcap_pkthdr *header, bool IPv6_mode)
{
	struct sockaddr_in source, dest; // struktury pro IP odesilatele a prijemce v4
	char v6buffer[128]; // pro IPv6 je proces odlisny, inet_ntop potrebuje extra buffer

	struct iphdr *ip_header;
	struct ip6_hdr *ip6_header;
	unsigned short iph_len;

	if(!IPv6_mode) // IPv4
	{
		memset(&source, 0, sizeof(source));
		memset(&dest, 0, sizeof(dest));
		ip_header = (struct iphdr *)( Buffer  + sizeof(struct ethhdr) );
		iph_len = ip_header->ihl*4;
		source.sin_addr.s_addr = ip_header->saddr;
		dest.sin_addr.s_addr = ip_header->daddr;
	}
	else // IPv6
	{
		ip6_header = (struct ip6_hdr *)( Buffer  + sizeof(struct ethhdr) );
		iph_len = 40; // delka IPv6 hlavicky je vzdy 40 bytu
	}

	struct udphdr *udp_header = (struct udphdr*)(Buffer + iph_len + sizeof(struct ethhdr));

	if(port_specified) //pokud je specifikovan port, ukoncime funkci pokud mu neodpovida prijemce ani odesilatel
	{
		if( ntohs(udp_header->source) != port && ntohs(udp_header->dest) != port ) return;
	}

	std::cout << std::endl;
	print_RFC3339(header);

	std::cout << (IPv6_mode ? inet_ntop(AF_INET6, &ip6_header->ip6_src, v6buffer, 128) : inet_ntoa(source.sin_addr)) << " : " << ntohs(udp_header->source) << " > "; // IP : port odesilatele
	std::cout << (IPv6_mode ? inet_ntop(AF_INET6, &ip6_header->ip6_dst, v6buffer, 128) : inet_ntoa(dest.sin_addr)) << " : " << ntohs(udp_header->dest) << ", "; // IP : port prijemce
	std::cout << "length " << Size << " bytes" << std::endl << std::endl;

	print_raw_data(Buffer, Size);
	std::cout << std::endl;

	--how_many_packets; //dekrementace pocitadla zpracovanych packetu
}




void process_ICMP(const u_char * Buffer, int Size, const struct pcap_pkthdr *header, bool IPv6_mode)
{
	struct sockaddr_in source, dest; // struktury pro IP odesilatele a prijemce
	char v6buffer[128];

	struct iphdr *ip_header;
	struct ip6_hdr *ip6_header;
	unsigned short iph_len;

	if(!IPv6_mode) // IPv4
	{
		memset(&source, 0, sizeof(source));
		memset(&dest, 0, sizeof(dest));
		ip_header = (struct iphdr *)( Buffer + sizeof(struct ethhdr));
		iph_len = ip_header->ihl*4;
		source.sin_addr.s_addr = ip_header->saddr;
		dest.sin_addr.s_addr = ip_header->daddr;
	}
	else // IPv6
	{
		ip6_header = (struct ip6_hdr *)( Buffer + sizeof(struct ethhdr) );
		iph_len = 40; // Fixni velikost IPv6 hlavicky
	}

	std::cout << std::endl;
	print_RFC3339(header);

	std::cout << (IPv6_mode ? inet_ntop(AF_INET6, &ip6_header->ip6_src, v6buffer, 128) : inet_ntoa(source.sin_addr)) << " > "; // IP odesilatele
	std::cout << (IPv6_mode ? inet_ntop(AF_INET6, &ip6_header->ip6_dst, v6buffer, 128) : inet_ntoa(dest.sin_addr)) << ", "; // IP prijemce
	std::cout << "length " << Size << " bytes" << std::endl << std::endl;

	print_raw_data(Buffer, Size);
	std::cout << std::endl;

	--how_many_packets; //dekrementace pocitadla zpracovanych packetu
}




void process_ARP(const u_char * Buffer, int Size, const struct pcap_pkthdr *header)
{
	// nasledujici 2 radky nakonec podle fora nejsou potreba, cteme MAC adresy pouze z Ethernet headeru
	//unsigned char HW_size = Buffer[18]; // zjistime delku MAC adres z ARP headeru
	//unsigned char IP_size = Buffer[19]; // a delku IP adres z ARP headeru

	const u_char *src_MAC = &Buffer[6]; // MAC adresa odesilatele, podporujeme 6 bytu (8 na ethernetu ne)
	const u_char *dst_MAC = &Buffer[0]; // MAC adresa prijemce je na zacatku Ethernet headeru, hned za ni odesilatel

	std::cout << std::endl;
	print_RFC3339(header);

	for(int i = 0; i < 5; i++) printf("%02x:", src_MAC[i]); // formatovany tisk MAC odesilatele
	printf("%02x > ", src_MAC[5]); // posledni byte bez dvojtecky, s mezerami a sipkou

	for(int i = 0; i < 5; i++) printf("%02x:", dst_MAC[i]); // formatovany tisk MAC prijemce
	printf("%02x, ", dst_MAC[5]); // posledni byte bez dvojtecky, s carkou a mezerou

	std::cout << "length " << Size << " bytes" << std::endl << std::endl;

	print_raw_data(Buffer, Size);
	std::cout << std::endl;

	--how_many_packets; //dekrementace pocitadla zpracovanych packetu
}




void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer)
{
	int size = header->len;

	if(interface_any) buffer += 2; // uprava pro specialni pcap pseudohlavicku, pro interface 'any'

	if(buffer[12] == 0x08 && buffer[13] == 0x00) //pokud je typ IPv4
	{
		struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));

		switch(ip_header->protocol) // cislo protokolu
		{
			case 6: // TCP Protokol
				if(which_packets & 0b1000) process_TCP(buffer, size, header);
				break;

			case 17: // UDP Protokol
				if(which_packets & 0b100) process_UDP(buffer, size, header);
				break;

			case 1: // ICMP
				if(which_packets & 0b1) process_ICMP(buffer, size, header);
				break;

			default:
				break;
		}
	}
	else if(buffer[12] == 0x86 && buffer[13] == 0xdd) // typ IPv6
	{
		u_char ipv6_protocol = buffer[20]; // byte 20 (byte 6 IPv6 headeru) znaci next header, tzn. cislo protokolu

		switch(ipv6_protocol) // cislo protokolu
		{
			case 6: // TCP protokol
				if(which_packets & 0b1000) process_TCP(buffer, size, header, true);
				break;

			case 17: // UDP Protokol
				if(which_packets & 0b100) process_UDP(buffer, size, header, true);
				break;

			case 58: // ICMPv6 Protokol
				if(which_packets & 0b1) process_ICMP(buffer, size, header, true);
				break;

			default:
				break;
		}
	}
	else if(buffer[12] == 0x08 && buffer[13] == 0x06) // typ ARP
	{
		if(which_packets & 0b10) process_ARP(buffer, size, header);
	}
}




void on_user_interrupt(int s)
{
	std::cerr << std::endl << "Warning: User interruption, freeing all used memory and ending the program correctly" << std::endl;

	if(interface != nullptr) pcap_close(interface);

	exit(0);
}





int main(int argc, char *argv[])
{
	// Nastaveni handleru uzivatelskeho ukonceni programu (CTRL + C)
	struct sigaction interrupt_handler;
	interrupt_handler.sa_handler = on_user_interrupt;
	sigemptyset(&interrupt_handler.sa_mask);
	interrupt_handler.sa_flags = 0;
	sigaction(SIGINT, &interrupt_handler, nullptr);

	//deklarace lokalnich promennych
	int c;
	bool show_interfaces = true;
	std::string interface_name;
	bool missing_arg = true;


	// struktura na dlouhe option argumenty pro getopt
	const struct option longopts[] =
	{
		{"interface", required_argument, 0, 'i'},
		{"tcp", no_argument, 0, 't'},
		{"udp", no_argument, 0, 'u'},
		{"arp", no_argument, 0, 'a'},
		{"icmp", no_argument, 0, 'c'},
		{0,0,0,0}
	};

	int option_index = 0;


	// Parsing argumentu pomoci getopt
	while((c = getopt_long(argc, argv, ":i:p:n:tu", longopts, &option_index)) != -1)
	{
		switch(c)
		{
			case 'i':
				missing_arg = false;
				interface_name = optarg;
				if(interface_name == "any") interface_any = true; // prepinac kvuli specialni hlavicce
				show_interfaces = false;
				break;

			case 'p':
				try {
					port = std::stoi(optarg);
				} catch(...) {
					std::cerr << "Error: -p option\'s argument must be a valid integer value specifying port" << std::endl;
					return 1;
				}
				port_specified = true;
				break;

			case 'n':
				try {
					how_many_packets = std::stoi(optarg);
				} catch(...) {
					std::cerr << "Error: -n option\'s argument must be a valid integer value >= 1" << std::endl;
					return 1;
				}
				if(how_many_packets < 1)
				{
					std::cerr << "Error: -n option\'s argument must be a valid integer value >= 1" << std::endl;
					return 1;
				}
				break;

			case 't':
				which_packets |= 0b1000;
				break;

			case 'u':
				which_packets |= 0b100;
				break;

			case 'a':
				which_packets |= 0b10;
				break;

			case 'c':
				which_packets |= 0b1;
				break;

			case ':':
				if(optopt == 'i') // chybejici argument u optionu, ktery ho ocekava
				{
					missing_arg = false;
					show_interfaces = true;
				}
				else
				{
					std::cerr << "Error: Missing argument for option -" << (char)(optopt) << std::endl;
					return 1;
				}
				break;

			default:
				std::cerr << "Error: Invalid option or redundant option argument" << std::endl;
				return 1;

		} // konec switch(c)
	} // konec while(getopt)

	if(missing_arg)
	{
		//std::cerr << "Error: Missing obligatory option -i {argument} or --interface {argument}" << std::endl;
		//return 1;
		show_interfaces = true; // puvodne jsem myslel ze je argument -i povinny, na foru se nakonec ukazalo ze neni
	}

	if(!which_packets) which_packets = 0b1111; // pokud neni specifikovano, zobrazujeme vse


	if(show_interfaces) return print_interfaces(); // pokud neni specifikovan interface, vytisknou se vsechny aktivni


	char errbuf[PCAP_ERRBUF_SIZE];
	interface = pcap_open_live(interface_name.c_str(), 65536, 0, 100, errbuf); // Pripojeni na zvoleny interface
	if(interface == nullptr)
	{
		std::cerr << "Error: Could not connect to the selected interface, please refer to the error message below" << std::endl << errbuf << std::endl;
		return 1;
	}

	while(how_many_packets) pcap_loop(interface, 1, process_packet, NULL);

	pcap_close(interface);

	return 0;
} // Konec main
