#include "arbres_binaires.h"

/*************************************************************/
/************** Cette fonction affiche un arbre **************/
/*************************************************************/

void afficherArbrePrefixe(Noeud *racine){
    if(racine){
        fprintf(stderr," 1 %s ", racine->val);
        afficherArbrePrefixe(racine->fg);
        afficherArbrePrefixe(racine->fd);
    }
    else fprintf(stderr," 0 ");
}


/*************************************************************/
/************** Cette fonction alloue un noeud ***************/
/*************************************************************/
Noeud * alloue_noeud(char *s){
    Noeud *n = (Noeud*)malloc(sizeof(Noeud));
    if(n){
        n->val = (char *)malloc(strlen(s)+1);
        if(!n->val){
            free(n);
            exit(EXIT_FAILURE);
        }
        strcpy(n->val, s);
        n->fg = n->fd = NULL;
    }
    return n;
}
/*************************************************************/
/**** Cette fonction libére un arbre donné récurssivement ****/
/*************************************************************/
void liberer(Arbre *A) {
    if(!*A) return;
    liberer(&((*A)->fg));
    liberer(&((*A)->fd));
    free((*A)->val); 
    free(*A);
}

/*************************************************************/
/******** Cette fonction crée l'arbre a1 de l'énoncer ********/
/*************************************************************/

Arbre cree_A_1(void){
    Arbre A_1 = alloue_noeud("arbre");
    A_1->fg = alloue_noeud("binaire");
    A_1->fd = alloue_noeud("ternaire");
    return A_1;
}

/*************************************************************/
/******** Cette fonction crée l'arbre a2 de l'énoncer ********/
/*************************************************************/
Arbre cree_A_2(void){
    Arbre a = alloue_noeud("Anémone");
    a->fg = alloue_noeud("Camomille");
    a->fd = alloue_noeud("Camomille");
    a->fd->fg = alloue_noeud("Dahlia");
    a->fd->fg->fd = alloue_noeud("Camomille");
    a->fd->fg->fd->fg = alloue_noeud("Iris");
    a->fd->fg->fd->fd = alloue_noeud("Jasmin");
    return a;
}
/*************************************************************/
/******** Cette fonction crée l'arbre a2 de l'énoncer ********/
/*************************************************************/

Arbre cree_A_2_v2(FILE *file) {
    char val[100];
    if (fscanf(file, "%s", val) != 1 || strcmp(val, "NULL") == 0) {
        return NULL;
    }
    Arbre a = alloue_noeud(val);
    if (a) {
        a->fg = cree_A_2_v2(file);
        a->fd = cree_A_2_v2(file);
    }
    return a;
}

/*************************************************************/
/******** Cette fonction crée l'arbre a3 de l'énoncer ********/
/*************************************************************/
Arbre cree_A_3(void){
    Arbre a = alloue_noeud("Intel Core i9");
    a->fg = alloue_noeud("Apple M3 Max");
    a->fd = alloue_noeud("Intel Core i9");
    a->fg->fd = alloue_noeud("AMD Ryzen 9");
    a->fg->fd->fg = alloue_noeud("Intel Core i9");
    a->fd->fg = alloue_noeud("Intel Core i9");
    return a;
}

/*************************************************************/
/******** Cette fonction crée l'arbre a3 de l'énoncer ********/
/*************************************************************/
Arbre cree_A_3_v2(FILE *file) {
    char val[100];
    if (fscanf(file, "%s", val) != 1 || strcmp(val, "NULL") == 0) {
        return NULL;
    }
    Arbre a = alloue_noeud(val);
    if (a) {
        a->fg = cree_A_3_v2(file);
        a->fd = cree_A_3_v2(file);
    }
    return a;
}

/*************************************************************/
/******** Cette fonction cronstruit un arbre *****************/
/******************en saisissant les données *****************/
/*************************************************************/
int construit_arbre(Arbre *a) {
    char valeur[50];
    int choix;

    printf("Veuillez saisir 1 pour un nœud non vide et 0 pour un nœud vide : ");
    fscanf(stdin, "%d", &choix);

    if (choix == 0) {
        *a = NULL;
        return 1;
    } else if (choix == 1) {
        printf("Veuillez saisir la valeur du nœud : ");
        fscanf(stdin, "%s", valeur);

        *a = alloue_noeud(valeur);

        if (!(*a)) {
            printf("Erreur lors de l'allocation du nœud.\n");
            return 0;
        }

        // Construire le sous-arbre gauche
        if (!construit_arbre(&((*a)->fg))) {
            return 0;
        }

        // Construire le sous-arbre droit
        if (!construit_arbre(&((*a)->fd))) {
            return 0;
        }

        return 1;
    }

    return 0;  // Choix invalide
}


