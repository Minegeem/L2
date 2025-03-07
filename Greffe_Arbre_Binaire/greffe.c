#include "greffe.h"
#include "arbres_binaires.h"
#include <string.h>
#include <stdlib.h>
#include "greffe.h"
#include "arbres_binaires.h"
#include <string.h>
#include <stdlib.h>



int copie(Arbre *dest, Arbre source) {
    if (!source) return 1;

    *dest = alloue_noeud(source->val);
    if (!*dest) return 0;

    if (source->fg) {
        if (!((*dest)->fg = alloue_noeud(source->fg->val))) {
            liberer(&(*dest)->fd);
            free(*dest);
            *dest = NULL;
            return 0;
        }
        if(!copie(&((*dest)->fg), source->fg)){
            return 0;
        }
    }

    if (source->fd) {
        if (!((*dest)->fd = alloue_noeud(source->fd->val))) {
            liberer(&(*dest)->fg);
            free(*dest);
            *dest = NULL;
            return 0;
        }
        if(!copie(&((*dest)->fd), source->fd)){
            return 0;
        }
    }
    return 1;
}


int expansion_aux(Arbre *G, Arbre A){
    if(!A ) return 1; 

    if(*G && !(*G)->fg && !(*G)->fd){
        if(!copie(&((*G)->fg), A->fg)){
            return 0;
        }
        if(!copie(&((*G)->fd), A->fd)){
            return 0;
        }
        return 1;
    }
    else if(*G && !(*G)->fg && (*G)->fd){
     
        if(!copie(&((*G)->fg), A->fg)){
            return 0;
        }
        return expansion_aux(&((*G)->fd), A);
    }
    else if(*G && (*G)->fg && !(*G)->fd){
       
        if(!copie(&((*G)->fd), A->fd)){
            
            return 0;
        }
        return expansion_aux(&((*G)->fg), A);
    }
    else
        return expansion_aux(&((*G)->fg), A) && expansion_aux(&((*G)->fd), A);
}

int expansion(Arbre *A, Arbre B){
    if (!(*A) || !B) return 1;
    if (strcmp((*A)->val, B->val) == 0){
        int a = expansion(&((*A)->fg),B);
        int b = expansion(&((*A)->fd),B);
        Arbre G = NULL; 
        if(!copie(&G, B)){
            return 0;
        }  
        int c = expansion_aux(&G, *A);
        if(!copie(&(*A), G)){
            return 0;
        }
        liberer(&G);
        return c && a && b;
    }else 
        return expansion(&(*A)->fg, B) && expansion(&(*A)->fd, B);
}
