#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "greffe.h"
#include "arbres_binaires.h"
#include "saage.h"



int main(int argc,char *argv[]) {
    if(argc < 2) exit(EXIT_FAILURE);
    if(!strcmp(argv[1],"-E")){
        if(argc > 1){
            Arbre r;
            construit_arbre(&r);
            serialise(argv[2],r);
            liberer(&r);
            //free(argv[2]);
        }
        else
            exit(EXIT_FAILURE);
    }
    else if(!strcmp(argv[1],"-G")){
        if(argc > 2){
            Arbre greffe, arbre;
            deserialise(argv[3], &greffe);
            deserialise(argv[2], &arbre);

            expansion(&arbre,greffe);
            afficherArbreEnSaage(arbre, 0);
            liberer(&arbre);
            liberer(&greffe);
            //free(argv[2]);
            //free(argv[3]);
            exit(EXIT_SUCCESS);
        }
        else
            exit(EXIT_FAILURE);
    }
    else 
        exit(EXIT_FAILURE);
    return 0;
}