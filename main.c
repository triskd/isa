/*
	ISA DNS resolver 2020
	David Triska, xtrisk05

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include "src/dString.h"
#include "src/hashTable.h"
#include "src/suportFunctions.h"

#include <netdb.h>
#include <signal.h>

#define BUFFER_SIZE 65507		

HashTable * TABLEDESTROY;	//ukazatel na tabulku symbolu
dStr * 	DSTRDESTROY1;		//ukazatel na dynamicky string pro parsovani domen

	
void sigint_handler(int sig){
	destroyHashTable(TABLEDESTROY);
	dstr_dStr(DSTRDESTROY1);

	exit(0);
}

int main(int argc, char ** argv){

	signal(SIGINT, sigint_handler);


	int lokalSocket, querySocket;				// file descriptory socketu
	struct sockaddr_in saLokal, saQuestion;		// struktury pro ulozeni adres a portu
	struct sockaddr_in saDns, saLokalDns;

	unsigned char bufferListen[BUFFER_SIZE];
	unsigned char bufferQuery[BUFFER_SIZE];
	int bytesRead, fromLen;

	dStr * domainDynStr = crt_dStr(); DSTRDESTROY1 = domainDynStr;
	HashTable * table = createHashTable(); TABLEDESTROY = table;


	int argCheck;
	int portArg;
	dStr* fileArg = crt_dStr();
	dStr* serverArg = crt_dStr(); 

	argCheck = handleArguments(argc, argv, &portArg, serverArg, fileArg);

	

	//kontrola argumentu
	switch(argCheck){

		//argumenty jsou OK
		case 0:
		break;

		//argument help
		case 1:
			printf("Napoveda k programu dns:\n");
			printf("_______________________________________________________________________________________________\n");
			printf("- spusteni:\n");
			printf(" ./dns -s server [-p port] -f filter_file\n");
			printf(" - povinne argumenty a jejich hodnoty: -s server, -f filter_file\n");
			printf("   -s server: hodnota server obsahuje IPv4 adresu, ve tvaru x.x.x.x, nebo domenu DNS serveru.\n");
			printf("   -f filter_file: hodnota filter_file je nazev souboru, obsahujiciho zakazane domeny\n");
			printf(" - nepovinne argumenty a jejich hodnoty: -p cislo_portu\n");
			printf("   -p port: hodnota port obsahuje cislo portu na kterem bude dns program naslouchat 0-65535\n");
			printf("_______________________________________________________________________________________________\n");
			printf("autor: David Triska, xtrisk05\n");
			printf("Listopad 2020\n");

			dstr_dStr(domainDynStr);			
			dstr_dStr(fileArg);			
			dstr_dStr(serverArg);
			destroyHashTable(table);				
			exit(0);
		break;

		//chybne zadany dns server
		case -1:
			fprintf(stderr, "Chybne zadan DNS server.\nServer zadavejte IPv4 adresou (x.x.x.x) nebo jeho domenou\n");
			fprintf(stderr, "Pro pro vypis napovedy programu spuste program s prepinace -h.\n");
			
			dstr_dStr(domainDynStr);			
			dstr_dStr(fileArg);			
			dstr_dStr(serverArg);
			destroyHashTable(table);					
			exit(-1);
		break;

		//chybne zadany port
		case -2:
			fprintf(stderr, "Chybne zadany port na kterem program nasloucha DNS dotazum.\n");
			fprintf(stderr, "Cislo portu je cele cislo z intervalu <0,65535>.\n");
			fprintf(stderr, "Pro pro vypis napovedy programu spuste program s prepinace -h.\n");
			
			dstr_dStr(domainDynStr);			
			dstr_dStr(fileArg);			
			dstr_dStr(serverArg);
			destroyHashTable(table);					
			exit(-1);
		break;

		//chyba v zadanem souboru
		case -3:
			fprintf(stderr, "Chyba pri pristupu k souboru s filtrovanymi adresami, soubor se nepodarilo otevrit.\n");
			fprintf(stderr, "Pro pro vypis napovedy programu spuste program s prepinace -h.\n");

			dstr_dStr(domainDynStr);			
			dstr_dStr(fileArg);			
			dstr_dStr(serverArg);
			destroyHashTable(table);					
			exit(-1);
		break;

		//nektery z klicovych argumentu nezadan
		case -4:
			fprintf(stderr, "Nezadan jeden nebo vice povinnych argumentu: -s 'server1' -f 'soubor' .\n");
			fprintf(stderr, "Pro vice informaci spuste program s prepinace -h, pro vypis napovedy programu.\n");

			dstr_dStr(domainDynStr);			
			dstr_dStr(fileArg);			
			dstr_dStr(serverArg);
			destroyHashTable(table);							
			exit(-1);
		break;
	}


	fillHashTableFromFile(table, fileArg->value);
	fprintf(stderr, "vyhledavaci tabulka naplnena.\n");

	lokalSocket = socket(AF_INET, SOCK_DGRAM, 0);
	querySocket = socket(AF_INET, SOCK_DGRAM, 0);

	//test zda se otevrel naslouchaci socket
	if(lokalSocket == -1){
		fprintf(stderr, "Socket na kterem dns program nasloucha se nepodarilo otevrit\n");
		exit(-1);
	}

	//test zda se otevrel dotazovaci socket
	if(querySocket == -1){
		fprintf(stderr, "Socket na kterem se dns program dotazuje se nepodarilo otevrit\n");
		exit(-1);
	}	

	//lokalni adresa k naslouchani dotazu
	saLokal.sin_family = AF_INET;
	saLokal.sin_port = htons(portArg);
	saLokal.sin_addr.s_addr = INADDR_ANY;

	//loalni adresa2 k odesilani dotazu na DNS server
	saLokalDns.sin_family = AF_INET;
	saLokalDns.sin_port = htons(portArg+1);
	saLokalDns.sin_addr.s_addr = INADDR_ANY ;

	//adresa dns serveru
	saDns.sin_family = AF_INET;
	saDns.sin_port = htons(53);
	saDns.sin_addr.s_addr = inet_addr(serverArg->value);
	
	//dealokace dynamickych stringu z argumentu, ktere jiz nepotrebujeme
	dstr_dStr(serverArg);
	dstr_dStr(fileArg);

	//bindovani naslouchaciho socketu
	if(bind(lokalSocket,(struct sockaddr*) &saLokal, sizeof(saLokal)) == -1){
		fprintf(stderr, "Nepodarilo se nabindovat naslouchaci socket na port: %d\n", portArg);
		close(lokalSocket);
		return -1;
	}

	//bindovani dotazovaciho socketu
	int port2 = portArg + 1;
	while(1){
		if(bind(querySocket, (struct sockaddr*) &saLokalDns, sizeof(saLokalDns)) == -1){
			//fprintf(stderr, "nepodarilo se bind() query socket na portu %d\n", portArg);

			if(port2 < 65535){
				port2 += 1;
				saLokalDns.sin_port = htons(port2);
			}else{
				port2 = 0;
			}
			
		}else{
			break;
		}
	}
	


	while(1){


		// printf("waiting for datagram at port: %d\n", portArg);

		fromLen = sizeof(struct sockaddr_in);

		bytesRead = recvfrom(lokalSocket, bufferListen, BUFFER_SIZE, 0,(struct sockaddr*) &saQuestion, &fromLen);

		//kontrola zda byly prijata data
		if(bytesRead != -1){
		
			switch(analyzeDnsPacket(bufferListen, bytesRead, domainDynStr)){

				//validni dns packet, dotaz typu A
				case 0:

					//kontrola zda dotaz neni na zakazanou domenu
					switch(isBanned(table, domainDynStr->value)){
						
						// domena je na seznamu zakazanych domen	
						case 0:
							
							buildErrResponsePacket(bufferListen, bufferQuery, 5);
							//zaslani DNS packetu s chybovym kodem 5 tazateli
							if(sendto(lokalSocket, bufferQuery, 12, 0, (struct sockaddr*) &saQuestion, sizeof(saQuestion)) == -1){

							}
							
							// printf("query A - banned finnished\n");
						break;

						// domena neni v seznamu zakazanych domen
						case -1:

							//preposlani dns dotazu na DNS server
							if(sendto(querySocket, bufferListen, bytesRead, 0, (struct sockaddr *) &saDns, sizeof(saDns)) == -1){
							}

							//odpoved z DNS serveru
							bytesRead = recvfrom(querySocket,bufferQuery, BUFFER_SIZE, 0, (struct sockaddr*) &saDns, &fromLen);
							if(bytesRead == -1){

							}

							//preposlani odpovedi z DNS serveru tazateli
							if(sendto(lokalSocket, bufferQuery, bytesRead, 0, (struct sockaddr*) &saQuestion, sizeof(saQuestion)) == -1){

							}						
						
							// printf("~query A - not banned finnished\n");
						break;

					}

				break;

				//validni dns packet, dotaz jineho typu nez A
				case 1:
						//preposlani dns dotazu na DNS server
						if(sendto(querySocket, bufferListen, bytesRead, 0, (struct sockaddr *) &saDns, sizeof(saDns)) == -1){
							
						}

						//odpoved z DNS serveru
						bytesRead = recvfrom(querySocket,bufferQuery, BUFFER_SIZE, 0, (struct sockaddr*) &saDns, &fromLen);
						if(bytesRead == -1){

						}

						//preposlani odpovedi z DNS serveru tazateli
						if(sendto(lokalSocket, bufferQuery, bytesRead, 0, (struct sockaddr*) &saQuestion, sizeof(saQuestion)) == -1){

						}		

						// printf("~query not A - finished\n");				
				break;

				//nevalidni dns packet
				case -1:
					buildErrResponsePacket(bufferListen, bufferQuery, 1);
					//zaslani dns packetu s chybovym kodem 1 tazateli
					if(sendto(lokalSocket, bufferQuery, 12, 0, (struct sockaddr*) &saQuestion, sizeof(saQuestion)) == -1){
					}
					
					// printf("~unkwon query \n");
				break;

			}

		}

		empt_dStr(domainDynStr);
	
	}

	close(querySocket);
	close(lokalSocket);

}
