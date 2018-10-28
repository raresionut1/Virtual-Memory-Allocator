Allocates, reallocates and frees blocks of memory from a built-in map of memory, providing efficient memory management at byte level.

//311CD Constantin Rares

	Voi incerca sa explic pe scurt cum am gandit sa implementez fiecare
functie, deoarece fiecare dintre acestea sunt explicate, pe bucati, sub forma 
de commenturi, in fisierul sursa.

	[INITIALIZE]:
		- aloca dinamic, folosing "calloc", o 'arena' de dimensiune ceruta

	[FINALIZE]:
		- elibereaza spatiul alocat dinamic pentru 'arena'

	[DUMP]:
		- folosind tipurile de date %08X si %02X, afiseaza intreaga mapa a
		arenei, cate 16 bytes pe fiecare linie
		- afiseaza de asemenea si indexul de inceput al fiecarei linii

	[int_to_pointer]:
		- casteaza o variabila de tip unsigned int int intr-un unsigned 
		char pointer
		- (Da, stiu ca nu era necesar sa implementez eu aceste 2 functii, 
		mergea direct castat, dar mi-am dat seama prea tarziu1)

	[pointer_to_int]:
		- casteaza din unsigned char pointer intr-o variabila de tip 
		unsigned int

	[ALLOC]:
		- cauta in arena, de la stanga la dreapta, un spatiu suficient de mare
		pentru alocarea noului bloc (cel putin 12 + size bytes)
		- daca s-a gasit un spatiu suficient de mare, se aloca noul bloc la
		indexul de inceput al spatiului gol, se initializeaza sectiunea de
		gestiune a blocului, se modifica sectiunile de gestiune ale blocurilor
		vecine si se schimba numarul blocurilor din arena
		- daca noul bloc este primul din arena (de la stanga la dreapta), se 
		modifica indicele de start
		- se returneaza 0, daca nu s-a gasit niciun spatiu suficient de mare
		pentru alocare, sau indexul de start al sectiunii de date al noului
		bloc alocat, in cazul in care se gaseste un spatiu suficient de mare

	[FREE]:
		- modifica sectiunile de gestiune ale blocurilor vecine
		- elibereaza spatiul ocupat de bloc in arena
		- modifica numarul de blocuri din arena

	[FILL]:
		- umple bytes ce fac parte din blocuri deja alocate in arena, incepand
		cu blocul cu indexul precizat
		- daca nu exista suficienti bytes in blocuri pentru umplerea atator
		bytes cati au fost precizati in arena, se vor umple toti bytii
		pana la terminarea ultimului bloc

	[ALLOCALIGNED]:
		- functioneaza la fel ca si ALLOC, doar ca se cauta un spatiu suficient
		de mare pentru alocarea aliniata a blocului < asta inseamna 
		(12 + size + a) bytes, unde a reprezinta cate pozitii mai sunt necesare
		pentru aliniere

	[REALLOC]:

		- mai intai se aloca dinamic (folosind calloc) un pointer de tip 
		unsigned char, cu scopul de a realiza un backup al blocului ce urmeaza
		a fi realocat
		- se verifica daca dimensiunea data ca parametru este mai mica sau mai
		mare decat dimensiunea datelor blocului, adica daca blocul trebuie
		cropat sau extins ca marime
		- se elibereaza blocul din arena, folosind functia FREE, implementata
		mai devreme
		- se cauta in arena, de la stanga la dreapta, un spatiu suficient de 
		mare pentru alocarea blocului cu noua dimensiune, folosind functia
		ALLOC, implementata mai devreme
		- daca alocarea nu a avut loc cu succes, doar se elibereaza spatiul
		alocat dinamic pentru vectorul de backup si se returneaza valoarea 0
		- daca alocarea a avut loc cu succes, se copiaza toate datele din
		vectorul de backup in noul bloc, apoi se elibereaza spatiul
		alocat dinamic pentru vectorul de backup si se returneaza valoarea
		indexului blocului realocat