/*
	dynamicky retezec
	xtrisk05

*/

#ifndef DSTRING_H_INCLUDE
#define DSTRING_H_INCLUDE

typedef struct dString{
	int size;
	int aloc;
	char last;
	int currind;
	int dedented;
	char* value;
}dStr;



/*
	vraci dString ukazatel na alokovanou pamet
*/
dStr * crt_dStr();

/*
	pridava znak na konec retezce
	@param1 dStr* - ukazatel na vytvoreny dString
	@param2 char - hodnota pridavaneho znaku
	
	@return  0 - znak byl pridan
			 x - znak byl pridan, musela byt provedena reallokace, x aktualni aloc. znaku
			-1 - znak nebyl pridan
*/
int push_dStr(dStr*, char );

/*
	zkopiruje hodnotu z ukazatele char* o delce int,
	do vytvoreneho dStr (predchazi volani crt_dStr())
	@param1 dStr* - vytovoreny dStr
	@param2 char* - ukazatel na kopirovany retezec
	@param3 int - delka kop. retezce
	@return  x - pocet zkopirovanych znaku == @param3
			-1 - retezec nebyl zkopirovan
*/
int load_dStr(dStr*, char*, int);

/*
	vymaze veskery obsah retezce, delku nastavi na 0
	@param1 dStr* - ukazatel na dStr jehoz obsah se ma smazat
	@return 0 - delka dStr
*/
int empt_dStr(dStr* );

/*
	vraci ukazatel na novy alokovany a hodnotou zkopirovany dStr 
	@param1 dStr* - kopirovany dStr
	@return dStr* - novy dStr, se stejnou hodnotou jako @param1
			NULL - puvodni dStr* byl NULL 
*/
dStr* cp_dStr(dStr* );

/*
	uvolni pamet pouzivanou strukturou
	@param1 dStr* - dealokovany dStr
		veskera pam. dealokovana
*/
void dstr_dStr(dStr*);

/*
	vyjme a vrati naposledy vlozeneho znak
*/
char pop_dStr(dStr*);

/*
	Porovna dString s polem
	@param1 dStr* - dStr
	@param2 char[] - pole
*/

void realloc_dStr(dStr*, int);

dStr* concat_dStr(dStr*, dStr*);

#endif