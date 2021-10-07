/*
	autor: xtrisk05
	Dynamicky retezec
*/

#include "dString.h"
#include <stddef.h>
#include <stdlib.h>



/*
	vraci ukazatel typu dStr s alokovanou pameti
*/
dStr * crt_dStr(){
	dStr * str = NULL;
	str = (dStr*) malloc(sizeof(dStr));
	
	if(str != NULL){
		str->value = (char*) malloc(sizeof(char) * 10);
		str->size = 0;
		str->aloc = 10;
		str->last = '\0';
		str->currind = 0;
		str->dedented = 0;
		empt_dStr(str);
		return str;

	}

	return NULL;
}


/*
	realokuje pamet pouzivanou retezcem na novou velikost size
*/
void realloc_dStr(dStr* str ,int size){

	if(str != NULL){

		if(str->size < size){
		
			str->value = (char*) realloc(str->value, sizeof(char)*(size+2));
			
			if(str->value != NULL){
		
				str->aloc = size + 2;
		
			}
			

		}	
		

	}

}


/*
	na konec dStr pridava znak c a ukoncovaci znak '\0'
	pokud je alokovano mene dojde k realokaci a naslednemu vlozeni znaku
*/
int push_dStr(dStr* str, char c){

	if((str->size + 2) < str->aloc){

		str->value[str->size++] = c;
		str->value[str->size] = '\0';

		return 0;

	}else{


		str->value = (char*) realloc(str->value, sizeof(char)*(str->aloc+10));

		str->value[str->size++] = c;
		str->value[str->size] = '\0';


		str->aloc += 10;

		if (str->value == NULL){
			return -1;
			// err_exit(INTERNERR, "dStr chyba l42");
			
		}

		return str->aloc;
	}

	return -1;
}


/*
	zpozdene vkladani znaku do dStr, znak se nejdrive vlozi na pozici str->last,
	pri dalsim volani dojde k presunuti znaku na konec retezce a novy znak se vlozi na pozici str->last
*/
int latePush_dStr(dStr* str, char c){
	
	if(str->last != '\0'){
	
		push_dStr(str, str->last);
		str->last = c;

	}else{

		str->last = c;
	}

	return 0;
}

/*
	nahrava do dStr retezec o velikosti size
*/
int load_dStr(dStr* s, char* c, int size){

	if(s != NULL && c != NULL){
		int i = 0;

		while(i < size){
		//	if(c[i] != '\0'){
				push_dStr(s, c[i]);
			//	printf("dStr 57: %c\n", s->value[i]);
				i++;
		//	}
		}


		return s->size;
	
	} else {
	
		return -1;
		// err_exit(INTERNERR, "dStr chyba l82");		
		
	}

	return -1;
}

/*
	pripise vsechny znaky na '\0'
*/
int empt_dStr(dStr* s){
	
	for(int i = 0; i < s->aloc; i++){
		s->value[i] = '\0';
	}

	s->size = 0;

	return s->size;
}

/*
	vraci na ukazatel na dStr jez je alokovany a naplneny obsahem z argumentu s
*/
dStr * cp_dStr(dStr* s){

	if (s != NULL){
		dStr * n = crt_dStr();
		load_dStr(n, s->value, s->size);
		return n;
	}

	return NULL;
}

/*
	uvolneni veskere pouzivane pameti dStr
*/
void dstr_dStr(dStr* str){
	if(str != NULL){
		if (str->value)
		{
			free(str->value);
		}
		str->size = 0;
		str->aloc = 0;
		free(str);
		str = NULL;
	}
}

/*

*/
int compareField(dStr* chain, char chainStat[]){
	int e = 0;
	while(e > (-1)){
		if(chain->value[e] == chainStat[e]){
			e++;
			if(e == 6){
				return 0;
			}
		}else{
			e = -1;
		}
	}
	return 1;
}

/*
	vyjme posledni znak z dStr
*/
char pop_dStr(dStr* str){

	char ret =  str->value[str->size-1];
	str->value[str->size-1] = '\0';
	str->size -= 1;

	return ret;

}

/*
	konkatenace dvou dStr, hodnota str2 je pripojena za posledni znak str1 
*/
dStr* concat_dStr(dStr* str1, dStr* str2){

	dStr* ret = crt_dStr();

	int i = 0;
	
	while(i < str1->size){
		push_dStr(ret, str1->value[i]);
		i++;
	}

	i = 0;

	while(i < str2->size){
		push_dStr(ret, str2->value[i]);
		i++;
	}

	return ret;

}