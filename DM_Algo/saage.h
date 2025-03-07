#ifndef __SAAGE_H__
#define __SAAGE_H__

#include "arbres_binaires.h"
#include "greffe.h"

int creerArbre(Arbre* a, FILE* f);
int serialise(char *nom_de_fichier, Arbre A);
int deserialise(char *nom_de_fichier, Arbre *A);
void afficherArbreEnSaage(Arbre a, int niveau);

#endif