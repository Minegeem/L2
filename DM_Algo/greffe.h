#ifndef __GREFFE_H__
#define __GREFFE_H__
#include "arbres_binaires.h"

int copie(Arbre *dest, Arbre source);
int expansion(Arbre *A, Arbre B);
int expansion_aux(Arbre *G, Arbre A);
#endif