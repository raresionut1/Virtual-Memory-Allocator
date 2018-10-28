//311CD Constantin Rares

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

unsigned char *v; //declararea globala a arenei
unsigned int n, start, ok; //dimensiunea arenei, indexul de start
                           //si verificarea daca arena e goala sau nu

void INITIALIZE(unsigned int size){
     
    v = NULL; 

    v = (unsigned char*)calloc(size, sizeof(unsigned char)); 

    //alocarea dinamica a arenei in heap

    n = size;

    //setarea dimensiunii arenei

    start = 0;

    //setarea indicelui de start

    ok = 0;

    //indicarea faptului ca arena este goala

} 

void FINALIZE(){
    free(v);
    //eliberarea spatiului alocat dinamic pentru intreaga arena
}

void DUMP(){
    unsigned int i, j ,a;
    for( i = 0; i < n; i=i+16){
        a=16;
        printf("%08X\t",i); //afisarea indicilor fiecarei linii

        if(( (n-i)/16 ) == 0 ) a=n%16;

        //verificarea daca marimea arenei este multiplu de 16 sau nu

        if(a >= 8){ //afisarea bytilor din arena
            for(j = 0; j < 8; j++)
                printf("%02X ", *(v+i+j));
            printf(" ");
            }
        else 
            for(j = 0; j < a; j++)
                printf("%02X ", *(v+i+j));

        for(j = 8; j < a; j++)
                printf("%02X ", *(v+i+j));

        printf("\n");
    }
}

void int_to_pointer(unsigned int x, unsigned int pos){
    //castarea unui unsigned int in unsigned char pointer
    unsigned int mask=0x000000FF;
    *(v+pos) = mask & x;
    *(v+pos+1) = mask & (x>>8);
    *(v+pos+2) = mask & (x>>16);
    *(v+pos+3) = mask & (x>>24);
}

unsigned int pointer_to_int(unsigned int pos){
    //castarea din unsigned char pointer intr-un unsigned int
     return ((int)*(v+pos+3) << 24) | 
            ((int)*(v+pos+2) << 16) |
            ((int)*(v+pos+1) << 8)  | 
            ((int)*(v+pos));
}

unsigned int ALLOC(unsigned int size){
    unsigned int inceput, sfarsit, aux;
    // La finalul functiei, "inceput" va fi pozitia de inceput a blocului, iar 
    // "sfarsit", pozitia primului byte ocupat, de dupa secventa de bytes
    // liberi. "aux" reprezinta inceputul blocului precedent

    if (n < 12) return 0; // Verificarea spatiului arenei

    if(start >= (size+12)){ //Verifica daca exista spatiu pentru alocarea
        inceput = 0;        //blocului inainte de primul bloc existent
        sfarsit = start;
        start = 0;
    }
    else{


    sfarsit = pointer_to_int(start);

    if(sfarsit == 0){// Daca indexul de start al blocului urmator este 0,
        sfarsit = n; // atunci semnifica ca nu mai avem niciun bloc urmator,
                     // deci sfarsitul va fi chiar finalul arenei
    }

    if (ok == 0) // Verifica daca arena e goala sau nu
        inceput = 0;

    else if(sfarsit == n){ //initializarea inceputului
            aux = start;
            inceput = start + pointer_to_int(start+8) + 12;
        }
        else{
            aux = start;
            inceput = pointer_to_int(sfarsit+4) 
                      + 12 
                      + pointer_to_int(pointer_to_int(sfarsit+4) + 8);
            }

    while ((size + 12) > (sfarsit - inceput)){ //cautarea efectiva a unei
                                     //secvente de bytes liberi de dimensiune
                                    //cel putin cat cea a blocului

        if (sfarsit == n) return 0; //daca sfarsitul a ajuns deja la finalul
        aux = sfarsit; //arenei si inca se cauta spatiu de alocare, inseamna
                       //ca nu exista nicio secventa de bytes liberi suficient
                       //de mare pentru a aloca blocul

        sfarsit = pointer_to_int(sfarsit); //schimbarea pozitiei de sfarsit

        if (sfarsit == 0)
            sfarsit = n;


        inceput = aux + 12+ pointer_to_int(aux+8);
                            //schimbarea pozitiei de inceput
    }

    }

    if ( sfarsit != n){
        int_to_pointer(inceput, sfarsit+4);
            //schimbarea indexului blocului urmator
        int_to_pointer(sfarsit, inceput);
            //initializarea primului index al blocului alocat
    }

    if ( inceput != 0){
        int_to_pointer(inceput, aux);
            //schimbarea indexului blocului precedent
        int_to_pointer(aux ,inceput+4);}
            //initializarea indexului al doilea al blocului alocat

    int_to_pointer(size, inceput+8);
            //initializarea indexului al treilea al blocului alocat

    ok++; //modificarea numarului de blocuri din arena

    return inceput+12; //indexul sectiunii de date din noul bloc
}

void FREE(unsigned int index){
    unsigned int i;
    unsigned int a = pointer_to_int(index - 12); //primul index 
    // (bloc urmator)
    unsigned int b = pointer_to_int(index - 8); //al doilea index 
    // (bloc precedent)
    unsigned int c = pointer_to_int(index - 4); //al treilea index
    // (marimea blocului)

    if (start==(index-12)) //verifica daca blocul este primul din arena
        start=a;
    else
        int_to_pointer(a, b); //modificarea sectiunii de gestiune a
                              //blocului precedent

    if(a != 0)
        int_to_pointer(b, a+4); //modificarea sectiunii de gestiune a
                                //blocului urmator


    for(i=0; i < (12 + c); i++) 
        *(v+index-12+i) = 0; //eliberarea spatiului

    ok--; //modificarea numarului de blocuri din arena
}

void FILL(unsigned int index, unsigned int size, unsigned int value){

    unsigned int i;
    
    index = index - 12; //inceputul sectiunii de gestiune a blocului

    while(size){ //umplerea bytilor
        if(size < pointer_to_int(index + 8)){
            for(i=0; i < size; i++)
                *(v+index+12+i) = value; //umplerea in cazul in care exista
            break;                       //suficienti bytes in blocul actual
        }
        else {
            for(i=0; i < pointer_to_int(index + 8); i++){
                *(v+index+12+i) = value; //umplerea in cazul in care NU exista
                size--;                  //suficienti bytes in blocul actual
            }  
            index = pointer_to_int(index); //trecerea la blocul urmator
            if(!index)  //verificarea daca mai exista blocuri
                break;
        }

    }
}

unsigned int ALLOCALIGNED(unsigned int size, unsigned int align){
    unsigned int inceput, sfarsit, aux, a;
    // La finalu functiei, "inceput" va fi pozitia de inceput a blocului, iar 
    // "sfarsit", pozitia primului byte ocupat, de dupa secventa de bytes
    // liberi. "aux" reprezinta inceputul blocului precedent

    if(12%align!=0) a=(align-(12%align));
        else a=0; //a-ul reprezinta cate pozitii sunt necesare pentru aliniere


    if (n < (12+a)) return 0; // Verificarea spatiului arenei

    if(start >= (size+12+a)){//Verifica daca exista spatiu pentru alocarea
        inceput = 0;         //blocului (aliniat) inainte de primul
        sfarsit = start;     //bloc existent
        start = a;
    }
    else{


    sfarsit = pointer_to_int(start);

    if(sfarsit == 0){// Daca indexul de start al blocului urmator este 0,
        sfarsit = n; // atunci semnifica ca nu mai avem niciun bloc urmator,
                     // deci sfarsitul va fi chiar finalul arenei
    }

    if (ok == 0){ // Verifica daca arena e goala sau nu
        inceput = 0;
        start = a;}

    else if(sfarsit == n){ //initializarea inceputului
            aux = start;
            inceput = start + pointer_to_int(start+8) + 12;
        }
        else{
            aux = start;
            inceput = pointer_to_int(sfarsit+4) 
                      + 12 
                      + pointer_to_int(pointer_to_int(sfarsit+4) + 8);
            }

    if((inceput+12)%align!=0) a=(align-((inceput+12)%align));
        else a=0;

    while ((size + 12 + a) > (sfarsit - inceput)){ //cautarea efectiva a unei
                                     //secvente de bytes liberi de dimensiune
                                    //cel putin cat cea a blocului (aliniat)

        if (sfarsit == n) return 0; //daca sfarsitul a ajuns deja la finalul
        aux = sfarsit; //arenei si inca se cauta spatiu de alocare, inseamna
                       //ca nu exista nicio secventa de bytes liberi suficient
                       //de mare pentru a aloca blocul (aliniat)

        sfarsit = pointer_to_int(sfarsit); //schimbarea pozitiei de sfarsit

        if (sfarsit == 0)
            sfarsit = n;


        inceput = aux + 12+ pointer_to_int(aux+8);
        if((inceput+12)%align!=0) a=(align-((inceput+12)%align));
        else a=0; //schimbarea valorii numarului de pozitii necesare
                  //pentru aliniere
        
                            //schimbarea pozitiei de inceput
    }

    }


    inceput = inceput + a; //alinierea blocului
    
    if ( sfarsit != n){
        int_to_pointer(inceput, sfarsit+4);
            //schimbarea indexului blocului urmator
        int_to_pointer(sfarsit, inceput);
            //initializarea primului index al blocului alocat
    }

    if ( inceput != a){
        int_to_pointer(inceput, aux);
            //schimbarea indexului blocului precedent
        int_to_pointer(aux ,inceput+4);}
            //initializarea indexului al doilea al blocului alocat

    int_to_pointer(size, inceput+8);
            //initializarea indexului al treilea al blocului alocat

    ok++;  //modificarea numarului de blocuri din arena

    return inceput+12; //indexul sectiunii de date din noul bloc
}

unsigned int REALLOC(unsigned int index, unsigned int size){
    unsigned int i;
    unsigned int marime = pointer_to_int(index - 4);
    unsigned char *w;

    w = NULL; 
    w = (unsigned char*)calloc(marime, sizeof(unsigned char));
    //alocarea dinamica a unui vector de unsigned charuri, pentru a stoca un
    //backup al blocului ce trebuie realocat

    if(marime > size) marime = size;    //verificarea daca blocul trebuie
        for(i=0; i < marime; i++)       //cropat sau extins ca marime
            *(w + i) = *(v + index +i); //(doar datele acestuia)
        //backupul blocului ce urmeaza a fi eliberat


    FREE(index); //eliberarea blocului din arena

    unsigned int x = ALLOC(size); //alocarea unui nou bloc, cu noua dimensiune

    if(x == 0){     //verificarea daca alocarea a avut loc cu succes
        free(w);    //eliberarea spatiului alocat dinamic pentru vectorul
                    //de backup
        
        return 0;   //returnarea in caz de eroare (nu exista spatiu suficient)
    }

    if(marime > size) marime = size;    //verificarea daca blocul trebuie
    for(i=0; i < marime; i++)           //cropat sau extins ca marime
        *(v + x + i) = *(w + i);        //(doar datele acestuia)
        //copierea datelor blocului

    free(w);    //eliberarea spatiului alocat dinamic pentru vectorul
                //de backup

    return x;   //returnarea indexului blocului realocat

}

void parse_command(char* cmd)
{
    const char* delims = " \n";

    char* cmd_name = strtok(cmd, delims);
    if (!cmd_name) {
        goto invalid_command;
    }

    if (strcmp(cmd_name, "INITIALIZE") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        
        INITIALIZE(size);


    } else if (strcmp(cmd_name, "FINALIZE") == 0) {
        
        FINALIZE();

    } else if (strcmp(cmd_name, "DUMP") == 0) {

        DUMP();


    } else if (strcmp(cmd_name, "ALLOC") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);

        printf("%u\n", ALLOC(size));

    } else if (strcmp(cmd_name, "FREE") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);

        FREE(index);

    } else if (strcmp(cmd_name, "FILL") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        char* value_str = strtok(NULL, delims);
        if (!value_str) {
            goto invalid_command;
        }
        uint32_t value = atoi(value_str);
        
        FILL(index, size, value);

    } else if (strcmp(cmd_name, "ALLOCALIGNED") == 0) {
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        char* align_str = strtok(NULL, delims);
        if (!align_str) {
            goto invalid_command;
        }
        uint32_t align = atoi(align_str);
        
        printf("%u\n", ALLOCALIGNED(size, align));

    } else if (strcmp(cmd_name, "REALLOC") == 0) {
        char* index_str = strtok(NULL, delims);
        if (!index_str) {
            goto invalid_command;
        }
        uint32_t index = atoi(index_str);
        char* size_str = strtok(NULL, delims);
        if (!size_str) {
            goto invalid_command;
        }
        uint32_t size = atoi(size_str);
        printf("%u\n", REALLOC(index, size));

    } else {
        goto invalid_command;
    }

    return;

invalid_command:
    printf("Invalid command: %s\n", cmd);
    exit(1);
}

int main(void)
{
    ssize_t read;
    char* line = NULL;
    size_t len;

    /* parse input line by line */
    while ((read = getline(&line, &len, stdin)) != -1) {
        /* print every command to stdout */
        printf("%s", line);

        parse_command(line);
    }

    free(line);



    return 0;
}
