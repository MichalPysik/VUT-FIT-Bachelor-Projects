#!/usr/bin/env python3.8

# IPK Projekt 1
# Autor: Michal Pysik (login: xpysik00)

import sys
import re
import socket
import pathlib



# funkce na chybove ukonceni programu
def error_exit(message):
	print("Error: " + message, file=sys.stderr)
	sys.exit(1)



# funkce na kontrolu a predani parametru
def load_params(args):
	if len(args) != 4:
		error_exit("Invalid number of parameters!")

	NAMESERVER = ''
	SURL = ''

	for i in [0, 2]:
		if args[i] == '-n':
			NAMESERVER = args[i+1]
		elif args[i] == '-f':
			SURL = args[i+1]
		else:
			error_exit("Invalid parameter " + args[i])

	if not NAMESERVER or not SURL:
		error_exit("Cannot enter the same parameter twice")
	elif not re.fullmatch('^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}:\d{1,5}$', NAMESERVER):
		error_exit("Invalid Nameserver parameter IP address format")
	elif not re.fullmatch('^fsp://[-\w.]+/\S+$', SURL):
		error_exit("Invalid SURL parameter format")

	return NAMESERVER, SURL



# funkce pro pripojeni k name serveru a zaslani pozadavku o nalezeni daneho souboru a preklad jmena na adresu a port
def NSP_request(NAMESERVER, domain_name):
	Address = NAMESERVER.split(':')[0]
	Port = int(NAMESERVER.split(':')[1])
	Request = "WHEREIS " + domain_name

	# pripojeni na name server
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
		s.settimeout(50)
		s.connect((Address, Port))
	except:
		error_exit("Could not connect to name server \'" + NAMESERVER + "\'")

	# zaslani pozadavku na name server
	try:
		s.sendall(Request.encode())
		result = s.recv(4096).decode()
		while not result:
			result = s.recv(4096).decode()
	except:
		error_exit("Problem with sending request to name server \'" + NAMESERVER + "\'")

	if result[:2] != "OK":
		if result[4:10] == 'Syntax':
			error_exit("Name server \'" + NAMESERVER + "\' could not understand the received request \'" + Request + "\''")
		else:
			error_exit("Could not find file server \'" + domain_name + "\' on name server \'" + NAMESERVER + "\'")

	s.close()
	return result[3:]



# funkce pro pripojeni k file serveru, zaslani pozadavku GET a ziskani obsahu daneho souboru
def FSP_request(FS_address, domain_name, file_path):
	Address = FS_address.split(':')[0]
	Port = int(FS_address.split(':')[1])
	Request = "GET " + file_path + " FSP/1.0\r\nHostname: " + domain_name + "\r\nAgent: xpysik00\r\n\r\n"

	# pripojeni na file server
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP
		s.settimeout(50)
		s.connect((Address, Port))
	except:
		error_exit("Could not connect to file server \'" + domain_name + "\'")

	# zaslani pozadavku na file server
	try:
		s.sendall(Request.encode())
		result = bytes() # tentokrat data ukladame jako binarni
		part = s.recv(4096)
		while part:
			result = result + part
			part = s.recv(4096)
	except:
		error_exit("Problem with sending request to file server \'" + domain_name + "\'")

	if result[:17].decode() == 'FSP/1.0 Not Found':
		error_exit("File \'" + file_path + "\' not found on file server \'" + domain_name + "\'")
	elif result[:19].decode() == 'FSP/1.0 Bad Request':
		error_exit("File server \'" + domain_name + "\' could not understand the request\nResponded with:\n" + result.decode())
	elif result[:15].decode() != 'FSP/1.0 Success':
		error_exit("File server \'" + domain_name + "\'\nResponded with:\n" + result.decode())

	s.close()
	return result.split(b'\r\n\r\n', 1)[1] # navracime az binarni data za hlavickou



# funkce pro pozadavek GET ALL file serveru
def FSP_request_all(FS_address, domain_name, specific_folder = None):
	# ziskani indexu
	fp_list = FSP_request(FS_address, domain_name, 'index').decode().split()

	fn_list = [] # jmena souboru (s relativni cestou vuci slozce predane jako argument skriptu)
	fc_list = [] # obsahy souboru

	for file_path in fp_list:
		if not str.isspace(file_path): # posledni prvek po splitu byva prazdny radek

			if specific_folder != None: # pokud nechceme vsechny soubory na serveru, ale jen z urcite slozky, tak ignorujeme ostatni soubory
				if specific_folder not in file_path:
					continue
				file_name = file_path[len(specific_folder):]
			else:
				file_name = file_path

			try:
				file_content = FSP_request(FS_address, domain_name, file_path)
			except:
				print("Warning: Could not download file \'" + file_name + "\', continuing with downloading remaining files", file=sys.stderr)
				continue

			fn_list.append(file_name)
			fc_list.append(file_content)

	return fn_list, fc_list




# hlavni funkce main
def main():
	# zpracovani parametru a nacteni do promennych
	NAMESERVER, SURL = load_params(sys.argv[1:])
	domain_name = SURL.split('/')[2]
	file_path = SURL.split('/', 3)[3]
	file_name = file_path.split('/')[-1:][0]

	# zaslani NSP pozadavku, ziskani adresy daneho file serveru
	FS_address = NSP_request(NAMESERVER, domain_name)

	# GET pro jeden soubor
	if file_name != '*':
		# zasladni FSP pozadavku danemu file serveru a ziskani obsahu souboru
		file_content = FSP_request(FS_address, domain_name, file_path)

		# vytvoreni noveho souboru a zapsani obdrzeneho obsahu
		try:
			f = open(file_name, 'wb') # zapisujeme primo binarni data
			f.write(file_content)
			f.close()
			print("Successfully downloaded file \'" + file_name + "\' from file server \'" + domain_name + "\' to local directory")
		except:
			error_exit("Problem with creating file \'" + file_name + "\' in local directory")

	# GET ALL
	else:
		if file_path == '*':
			fn_list, fc_list = FSP_request_all(FS_address, domain_name) # chceme vsechny soubory ze serveru
		else:
			fn_list, fc_list = FSP_request_all(FS_address, domain_name, file_path[:-1]) # chceme vsechny soubory jen z urcite slozky

		succ_cnt = 0
		# vytvoreni vsech novych souboru v cyklu
		for i in range(len(fn_list)):
			try:
				if '/' in fn_list[i]: #kdyz je soubor v podslozkach, ty musi pripadne vytvorit
					pathlib.Path(fn_list[i].rsplit('/', 1)[0]).mkdir(parents=True, exist_ok=True)
				f = open(fn_list[i], 'wb') # zapisujeme primo binarni data
				f.write(fc_list[i])
				f.close()
				print("Successfully downloaded file \'" + fn_list[i] + "\' from file server \'" + domain_name + "\' to local directory")
				succ_cnt = succ_cnt + 1
			except:
				error_exit("Problem with creating file \'" + fn_list[i] + "\' in local directory")

		if succ_cnt:
			print("Total of " + str(succ_cnt) + " files have been downloaded successfully!")
		else:
			if file_path == '*':
				print("The server you are trying to download all files from is empty!")
			else:
				print("The folder you are trying to download all files from is either empty or does not exist!")

	sys.exit(0)






# automaticke volani funkce main (pokud neni tento soubor pouzit pouze jako modul)
if __name__ == '__main__':
	main()
