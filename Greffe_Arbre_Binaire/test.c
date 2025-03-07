#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "greffe.h"
#include "arbres_binaires.h"
#include "saage.h"


int main(){    
    Arbre a_1 = cree_A_1();
    Arbre a_2 = cree_A_2();
    Arbre a_3 = cree_A_3();
    afficherArbrePrefixe(a_1);
    printf("\n*****************************\n");
    
    afficherArbrePrefixe(a_2);
    printf("\n*****************************\n");
    
    afficherArbrePrefixe(a_3);
    printf("\n************ copier A1 *****************\n");
    Arbre b_1 = NULL;
    copie(&b_1,a_1);
    afficherArbrePrefixe(b_1);

    printf("\n************ greffer sur A1 *****************\n");
    Arbre g_1 = alloue_noeud("binaire");
    g_1->fg = alloue_noeud("lexicographique");
    g_1->fd = alloue_noeud("n-aire");
    expansion(&a_1, g_1);
    afficherArbrePrefixe(a_1);

    printf("\n************ G2 *****************\n");
    Arbre g_2 = alloue_noeud("Camomille");
    g_2->fg = alloue_noeud("Lilas");
    g_2->fd = alloue_noeud("Rose");
    afficherArbrePrefixe(g_2);

    printf("\n*************** greffer sur A2 *********\n");
    expansion(&a_2, g_2);
    afficherArbrePrefixe(a_2);

    printf("\n************** G3 ******************\n");
    Arbre g_3 = alloue_noeud("Intel Core i9");
    g_3->fg = alloue_noeud("Intel Core i9");
    g_3->fg->fg = alloue_noeud("Apple M3 Max");
    g_3->fg->fd = alloue_noeud("AMD Ryzen 9");
    afficherArbrePrefixe(g_3);

    printf(("\n*********** Greffer sur A3 ********\n"));
    expansion(&a_3,g_3);
    afficherArbrePrefixe(a_3);

    printf("\n******* Construire un arbre ***********\n");
    Arbre r;
    construit_arbre(&r);
    afficherArbrePrefixe(r);

    //printf("\n************ Sérialisation  A1 **************\n");
    //serialise("test.saage", a_1);

    printf("\n************ Sérialisation  A2 **************\n");
    serialise("test2.saage", a_2);
    printf("\n************ Désérialisation  A1 **************\n");
    Arbre sa_1 = NULL;
    deserialise("test.saage", &sa_1);
    afficherArbrePrefixe(sa_1);
    printf("**************************\n");

    liberer(&a_1);
    liberer(&a_2);
    liberer(&a_3);
    liberer(&b_1);
    liberer(&r);
    liberer(&sa_1);

}