#ifndef __ARBRES_BINAIRES_H__
#define __ARBRES_BINAIRES_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _noeud{
    char *val;
    struct _noeud *fg, *fd;
} Noeud, *Arbre;

void afficherArbrePrefixe(Noeud *racine);
Noeud * alloue_noeud(char *s);
void liberer(Arbre *A);
Arbre cree_A_1(void);
Arbre cree_A_3(void);
Arbre cree_A_2(void);
Arbre cree_A_2_v2(FILE *file);
Arbre cree_A_3_v2(FILE *file);
int construit_arbre(Arbre *a);


#endif