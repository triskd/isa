/*
	autor: David Triska, xtrisk05
	podpurne funkce pro dns resolver

*/

#include "suportFunctions.h"

/*
	return 
		 1 - bit is set
		 0 - bit is not set
		-1 - n is bigger than sizeof(word)
*/
int isNBitOfWordSet(char word, int bit){

	if((bit >= 0 && bit <= 7) ){

		if(word & (1 << bit)){
			return 1;
		}

		return 0;

	}

	return -1;
}


/*
	funkce zkontroluje zda se jedna o validni DNS packet,
	pokud je packet DNS dotaz typu A, vytahne z packetu 
	dotazovanou domenu

	return
		 0	- jedna se validni query DNS packet, dotaz typu A, do argumentu dStr* queryDomain vlozi domenu
		 1	- jedna se o validni query DNS packet, dotaz jineho typu nez A, do argumentu se nevklada domena
		-1	- nejedna se o validni DNS packet
*/
int analyzeDnsPacket(char* packet, int bytesRead, dStr * queryDomain){


	bool headerOk = true;

	//kontrola DNS hlavicky
	//////////////////////////////////////////////////////////
	if(bytesRead >= 2){
		//kontrola QR-bit = 0
		if(isNBitOfWordSet(packet[2], 7) != 0){
			headerOk = false;
		}

		//kontrola OPCODE = 0, vsechny 4 bity OPCODE == 0
		for(int bitI = 6; bitI >= 3; bitI--){
			if(isNBitOfWordSet(packet[2], bitI) != 0){
				headerOk = false;
				break;
			}
		}

		//kontrola TC-bit = 0
		if(isNBitOfWordSet(packet[2], 1) != 0){
			headerOk = false;
		}
	}else{
		headerOk = false;
	}

	if(headerOk == false){
		return -1;
	}else{
		// printf("\nDNS hlavicka OK\n");
	}
	//////////////////////////////////////////////////////////

	//extrakce domeny
	int byteI = 12;
	int labelCount = packet[byteI];
	
	while(byteI+labelCount < bytesRead){
		
		for(int i = byteI+1; i <= byteI+labelCount; i++){
			//printf("%c", packet[i]);
			push_dStr(queryDomain, packet[i]);
		}	
			
		

		byteI += labelCount+1;
		labelCount = packet[byteI];

		if(labelCount == 0){
			break;
		}

		//vlozeni tecky, mezi jednotlive labely
		push_dStr(queryDomain, '.');

	}	
	
	// printf("%s\n",queryDomain->value);	

	//kontrola zda je dotaz typu A
	if(packet[byteI+1] == 0 && packet[byteI+2] == 1){
		//printf("\ndotaz typu A\n");
		return 0;
	}	


	return 1;
}

/*
	funkce upravi (vytvori, pokud puvodni datagram nebyl vubec typu DNS)
	hlavicku dotazovaciho DNS packetu,
	aby odpovidala odpovedi a nastavi prislusny chybovy kod

	
*/
int buildErrResponsePacket(char* inPacket, char* outPacket, int errCode){ 

	// format error, packet nebyl dns
	if(errCode == 1){
		if(strlen(inPacket) >= 4){
			outPacket[0] = inPacket[0];
			outPacket[1] = inPacket[1];
			outPacket[2] = inPacket[2];
			outPacket[3] = inPacket[3];
		}

		outPacket[2] |= 1ul << 7;	//dns opdoved
		outPacket[3] |= 1ul << 0;	
		outPacket[3] |= 0ul << 1;
		outPacket[3] |= 0ul << 2;
		outPacket[3] |= 0ul << 3;
	}

	//refused to perform, dotaz byl na zakazanou domenu
	if(errCode == 5){
		outPacket[0] = inPacket[0];
		outPacket[1] = inPacket[1];
		outPacket[2] = inPacket[2];
		outPacket[3] = inPacket[3];
		
		outPacket[2] |= 1ul << 7;	//dns opdoved
		outPacket[3] |= 1ul << 0;	
		outPacket[3] |= 0ul << 1;
		outPacket[3] |= 1ul << 2;
		outPacket[3] |= 0ul << 3;

	}

}


/*
	zkontroluje a ulozi argumenty programu
	return 
		 0 	- argumenty OK
		-1	- chyba v zadanem dns serveru
		-2	- chyba v zadanem portu
		-3	- soubor neslo otevrit
		-4 	- nezadan jeden nebo vice pozadovanych argumentu
		 1	- napoveda

*/
int handleArguments(int argc,char** argv, int *port, dStr* server, dStr* file){

	int opt;
	bool serverBool, fileBool, portBool = false;
	FILE * f  = NULL;


	while( (opt = getopt(argc, argv, ":p:s:f:h")) != -1){

		switch(opt){

			case 'p':
				*port = atoi(optarg);

				if(*port < 0 || *port > 65535){
					*port = 0;
					return -2;
				}

				portBool = true;
			break;

			case 's':
				if(inet_addr(optarg) == -1){
					struct hostent* host = gethostbyname(optarg);
					if(host != NULL){
						load_dStr(server, inet_ntoa(*(struct in_addr*) host->h_addr_list[0]), strlen(inet_ntoa(*(struct in_addr*) host->h_addr_list[0])) );
					}else{
						return -1;
					}

				}else{
					load_dStr(server, optarg, strlen(optarg));
				}
				serverBool = true;
			break;

			case 'f':
				f = fopen(optarg, "r");
				if(f != NULL && strcmp(optarg, "") != 0){
					load_dStr(file, optarg, strlen(optarg));
					fileBool = true;
					fclose(f);
				}else{
					return -3;
				}

			break;

			case 'h':
				return 1;
			break;

		}

	}

	if(serverBool == false || fileBool == false){
		return -4;
	}

	if(portBool == false){
		*port = 53; 	// defaultni port
	}

	return 0;

}