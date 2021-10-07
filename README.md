README
program dns resolver, projekt ISA 2020
autor: David Triska, xtrisk05
-------------------------------------------------------------------------------------------------------------------

- prelozeni:
	$ make

- spusteni:
	$ ./dns -s server -f filter_file [-p port]

- vyznam jednotlivych parametru programu:
	- povinne parametry:
		-s server: 
			hodnota server obsahuje IPv4 adresu, nebo domenu DNS serveru, na ktery se budou preposilat DNS dotazy
		-f filter_file:
			hodnota filter_file, je jmeno souboru obsahujici seznam zakazanych domen, ktere resolver filtruje. Kazdy radek soboru obsahuje jedinou domenu, prazdne radky a radky zacinajici znakem '#' jsou ignorovany.

	- nepovinne parametry:
		-p port:
			specifikuje cislo portu, na kterem bude resolver naslouchat. Pokud parametr neni zadan vychozi port je 53. Povolene cisla portu jsou z intervalu <0, 65535>.
		-h :
			Pri spusteni s timto parametrem program vypise napovedu.

- ukonceni behu programu:
	CTRL + C

- vyhledavani v seznamu zakazanych domen:
	Ukladani a vyhledavani je implementovano hashovaci tabulkou. Hashovaci funkce je zalozena na algoritmu djb2.

- snadne testovani pomoci programu dig: $ dig @127.0.0.1 -p xxxx domain

- popis chovani programu: 
	Po spusteni programu, program otestuje zda jsou zadane vsechny povinne argumenty, pokud nektery z nich chybi konci s navratovym kodem -1 a na stderr vypisuje chybove hlaseni.
	Pote program naplnuje vyhledavaci tabulku s rozptylenymi symboly, ktera slouzi k ulozeni a vyhledavani zakazanych domen. Tato operace muze chvili trvat (pri naplneni testovacim souborem https://dbl.oisd.nl/ tato operace trvala zhruba 9s), dokonceni teto operace je signalizovano vypisem na stderr "vyhledavaci tabulka naplnena".
	Nasledne program vytvari a navazuje sockety na prislusne adresy a porty. Port na kterem program odesila DNS dotazy na zadany server, je nasledujici port nez na kterem program nasloucha, pokud tento port neni volny, dotazovaci socket se navaze na prvni volny port. Program nasledne nasloucha na zadanem/vychozim (53) portu dns dotazy.
	Kazdy prichozi datagram nejdrive zkontroluje zda se a) jedna o validni DNS dotaz b) zda je tento dotaz typu A.
	a) Pokud prichozi datagram nebyl DNS dotaz, resolver zasila DNS odpoved s chybovym kodem 1 - Format Error, kde jako ID uvede prvni 2byty prichoziho datagramu (pokud byl dlouhy alespon 2 byty). 
	b) Pokud byl dotaz typu A, porovna dotazovanou domenu, se zakazanyma domena i zda dotazovana domena, neni poddomenou zakazane domeny. Pokud dotaz byl na zakazanou domenu, program odesila tazateli DNS odpoved s chybovym kodem 5 - Reffused to perform..., Jinak dotaz preposila na DNS server, odpoved z DNS serveru preposila tazatali.
	Pokud dotaz nebyl typu A, preposila dotaz na DNS server, odpoved ze serveru preposila tazatali.
	Resolver takto nasloucha v nekonecne smycce. Ukonceni programu je provedeno stiskem klaves CTRL + C, program pred ukoncenim uvolnuje alokovanou pamet, pote konci s navratovym kodem 0.

- seznam souboru:
	```
	|--- main.c 	
	|--- Makefile
	|--- README
	|--- src
		  |--- dStr.c
		  |--- dString.h
		  |--- hashTable.c
		  |--- hashTable.h
		  |--- suportFunctions.c
		  |--- suportFunctions.h
	```

-------------------------------------------------------------------------------------------------------------------