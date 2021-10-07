/*
	hashovaci tabulka pro uchovani a vyhledavani seznamu zakazanych domen
	xtrisk05	
*/

#ifndef HASHTABLE_H_INCLUDE
#define HASHTABLE_H_INCLUDE

#include "dString.h"
#include <stdio.h>
#define TABLE_SIZE 10000

int Hash(char*); 

typedef struct HTRecord{
	dStr * domain;
	struct HTRecord * nextRecord;
}HTRecord;

typedef struct HashTable{
	HTRecord ** records;
}HashTable;

//vytvoreni, alokace pameti pro HashTable
HashTable * createHashTable();

//vytvoreni, alokace pameti pro HTRecord
HTRecord * createHTRecord(char*);

//destrukce HashTable, dealokvace veskere pouzite pameti
void destroyHashTable(HashTable * );

//vlozeni nove domeny do HashTable
/*
	return 
		 0	- zaznam uspesne vlozen do HashTable
		-1	- zaznam jiz v tabulce je
		-2	- vnitrni chyba, chyba alokace,....
*/
int insertHashTable(HashTable*, char*);

//prohledani HashTable zda obsahuje zaznam
/*
	return 
		 0	- zaznam je jiz v HashTable
		-1	- zaznam neni v HashTable
*/
int isInHashTable(HashTable*, char*);

int fillHashTableFromFile(HashTable*, char*);

/*
	zkontroluje jestli domena neni v zakazanem seznamu,
	pripadne pokud domena neni subdomenou zakazane domeny
	
	return 
		 0	- zakazana domena
		-1	- neni zakazana
		-2	- vnitrni chyba, table == null || doamin == null
*/
int isBanned(HashTable*, char*);

/*
	odstrani prvni subdomenu domeny

	return
		 0	- vracena domena
		-1	- domena jiz nemela zadnou subdomenu k odstraneni
*/
int removeSubDomain(dStr*, char*);

#endif
