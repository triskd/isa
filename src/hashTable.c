/*
	Hashovaci tabulka pro uchovani seznamu zakazanych domen
	xtrisk05
*/

#include "hashTable.h"
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>



/*
	Hashovaci funkce zalozena na algoritmu djb2
	http://www.cse.yorku.ca/~oz/hash.html
*/


int Hash(char *str){
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return (int) (hash % TABLE_SIZE);
}

HashTable * createHashTable(){

	HashTable * table = malloc(sizeof(HashTable));

	if(table != NULL){

		table->records = malloc(sizeof(HTRecord*)* TABLE_SIZE);

		if(table->records != NULL){
			for(int i = 0; i < TABLE_SIZE; i++){
				table->records[i] = NULL;
			}

			return table;
		}

	}

	return NULL;

}

HTRecord * createHTRecord(char* domainName){

	HTRecord * record = malloc(sizeof(HTRecord));
	
	if(record != NULL){
		record->domain = crt_dStr();
		record->nextRecord = NULL;
		
		int length = strlen(domainName);

		if(load_dStr(record->domain, domainName, length) != -1){
			return record;
		}
		
		//pokud nedoslo ke zkopirovani, uvolnim zabranou pamet
		dstr_dStr(record->domain);
		free(record);

	}
	
	return NULL;	

}

int isInHashTable(HashTable* table, char* domainName){

	int hash = Hash(domainName);
	HTRecord * workRec = NULL;

	if(table != NULL){

		workRec = table->records[hash];

		if(workRec != NULL){

			while(workRec != NULL){

				if(strcmp(workRec->domain->value, domainName) == 0){
					return 0;
				}else{
					workRec = workRec->nextRecord;
				}

			}

		}

	}

	return -1;
}

int insertHashTable(HashTable* table, char* domainName){

	int hash = Hash(domainName);
	HTRecord * workRec = NULL;

	if(table != NULL){

		// zaznam neni v HashTable
		if(isInHashTable(table, domainName) == -1){

			//v tabulce pod indexem hash dosud neni zadny zaznam
			if(table->records[hash] == NULL){

				//vlozeni zaznamu do tabulky
				table->records[hash] = createHTRecord(domainName);
				return 0;


			}else{
			// v tabulce je jiz pod timto indexem nejaky zaznam, vlozeni na konec seznamu

				workRec = table->records[hash];

				while(1){

					if(workRec->nextRecord == NULL){

						//vlozeni na konec seznamu, pod danym indexem
						workRec->nextRecord = createHTRecord(domainName);
						return 0;
					
					}else{

						workRec = workRec->nextRecord;

					}

				}

			}

		}

	}

	return -1;

}

void destroyHashTable(HashTable* table){

	HTRecord * wRec = NULL;
	HTRecord * nRec = NULL;

	if(table != NULL){

		for(int i = 0; i < TABLE_SIZE; i++){

			wRec = table->records[i];

			if(wRec != NULL){
				
				while(wRec != NULL){
					nRec = wRec->nextRecord;
					dstr_dStr(wRec->domain);
					free(wRec);
					wRec = nRec;	
				}
				

			}

		}

		free(table->records);
		free(table);

	}

}

int fillHashTableFromFile(HashTable* table, char* fileName){
	FILE * file;
	dStr * buffer = crt_dStr();
	char c;
	char lastC = '\n';
	bool skipLine = false;

	if(table != NULL && fileName != NULL){

		file = fopen(fileName, "r");

		if(file != NULL){

			while(1){
				c = fgetc(file);
				if(c == EOF){
					break;
				}
				
				if(lastC == '\n' && c == '#' ){
					skipLine = true;
				}


				if(c != '\n'){
					
					if(skipLine == false){
						push_dStr(buffer, c);
						lastC = c;	
					}

				}else{
					insertHashTable(table, buffer->value);
					// printf("line: %s \n", buffer->value);
					empt_dStr(buffer);
					lastC = '\n';
					skipLine = false;
				}

			}

			fclose(file);


		}

	}

	dstr_dStr(buffer);

}

int removeSubDomain(dStr* outDomain, char* inDomain){

	char c;
	empt_dStr(outDomain); 
	bool subdomain = true;


	for(int i = 0; i < strlen(inDomain); i++){
		c = inDomain[i];

		// printf("%c", c);

		if(subdomain == false){
			push_dStr(outDomain, c);
		}

		if(c == '.'){
			subdomain =	false;
		}

	}

	if(subdomain == true){
		return -1;
	}

	return 0;

}

int isBanned(HashTable* table, char* domain){

	dStr * subDomainStr = crt_dStr();
	dStr* helpStr = crt_dStr();


	if(table != NULL && domain != NULL){

		//domena je v tabulce
		if(isInHashTable(table, domain) == 0){
			dstr_dStr(subDomainStr);
			dstr_dStr(helpStr);
			return 0;
		}else{

			load_dStr(helpStr, domain, strlen(domain));
			while(removeSubDomain(subDomainStr, helpStr->value) != -1){
                // printf("%s\n", helpStr->value);			
            	//domena je v tabulce
				if(isInHashTable(table, subDomainStr->value) == 0){
					dstr_dStr(subDomainStr);
					dstr_dStr(helpStr);
					return 0;
				}
				
				empt_dStr(helpStr);
				load_dStr(helpStr, subDomainStr->value, subDomainStr->size);

			}

		}

		dstr_dStr(subDomainStr);
		dstr_dStr(helpStr);
		return -1;

	}

	dstr_dStr(subDomainStr);
	dstr_dStr(helpStr);
	return -2;

}
