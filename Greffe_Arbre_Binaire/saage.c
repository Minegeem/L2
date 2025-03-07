#include <stdio.h>
#include <stdlib.h>
#include "saage.h"


static void ecrireArbre(FILE *f, Arbre a, int niveau){
    if (!a) {
        fprintf(f, "NULL\n");
        return;
    }
    if(niveau == 0){
        fprintf(f, "Valeur : %s\n", a->val);
    }else{
        fprintf(f, "\n");
        for (int i = 0; i < niveau; i++) {
            fprintf(f, "    ");
        }
        fprintf(f, "Valeur : %s\n", a->val); 
    }
    for (int i = 0; i < niveau; i++) {
        fprintf(f, "    ");
    }
    fprintf(f, "Gauche : ");
    ecrireArbre(f, a->fg, niveau + 1);

    for (int i = 0; i < niveau; i++) {
        fprintf(f, "    ");
    }
    fprintf(f, "Droite : ");
    ecrireArbre(f, a->fd, niveau + 1);
}

int serialise(char *nom_de_fichier, Arbre A) {
    FILE *out = fopen(nom_de_fichier, "w");
    if (!out) {
        perror("Erreur lors de l'ouverture du fichier");
        return 0;
    }
    ecrireArbre(out, A, 0);
    fclose(out);
    return 1;
}

static Arbre des_aux(FILE *f, Arbre *a) {
    char *ligne = NULL;
    size_t longueur_ligne = 0;
    if (getline(&ligne, &longueur_ligne, f) != -1) {
        char *debut = strstr(ligne, "Valeur : ");
        if (debut) {
            debut += strlen("Valeur : ");
            char *fin = strchr(debut, '\n');
            if (fin) *fin = '\0';
            if (strcmp(debut, "NULL") == 0) {
                *a = NULL;
            } else {
                *a = alloue_noeud(debut);
            }
        }
        // Lire les enfants gauches et droits
        if (getline(&ligne, &longueur_ligne, f) != -1) {
            char *gauche = strstr(ligne, "Gauche :");
            if (gauche) {
                gauche += strlen("Gauche :");
                char *fing = strchr(gauche, '\n');
                if (fing) *fing = '\0';
                if (strcmp(gauche, " NULL") == 0) {
                    (*a)->fg = NULL;
                } else {
                    des_aux(f, &((*a)->fg)); // Lire le sous-arbre gauche récursivement
                }
            }
        }
        if (getline(&ligne, &longueur_ligne, f) != -1) {
            char *droite = strstr(ligne, "Droite :");
            if (droite) {
                droite += strlen("Droite :");
                char *find = strchr(droite, '\n');
                if (find) *find = '\0';
                if (strcmp(droite, " NULL") == 0) {
                    (*a)->fd = NULL;
                } else {
                    des_aux(f, &((*a)->fd)); // Lire le sous-arbre droit récursivement
                }
            }
        }
    }
    free(ligne);
    return *a;
}

int deserialise(char *nom_de_fichier, Arbre *A) {
    FILE *fichier = fopen(nom_de_fichier, "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier");
        return 0;
    }
    // Appeler la nouvelle fonction récursive pour lire le fichier et créer l'arbre correspondant
    *A = des_aux(fichier, A);
    fclose(fichier);
    // Libérer la mémoire allouée pour le nom de fichier
    //free(*nom_de_fichier);
    // Vérifier si l'arbre a été correctement créé
    if (*A == NULL) {
        return 0;
    } else {
        return 1;
    }
}

void afficherArbreEnSaage(Arbre a, int niveau){
    if (!a) {
        printf("NULL\n");
        return;
    }
    if(niveau == 0){
        printf("Valeur : %s\n", a->val);
    }else{
        printf("\n");
        for (int i = 0; i < niveau; i++) {
            printf("    ");
        }
        printf("Valeur : %s\n", a->val); 
    }
    for (int i = 0; i < niveau; i++) {
        printf("    ");
    }
    printf("Gauche : ");
    afficherArbreEnSaage(a->fg, niveau + 1);

    for (int i = 0; i < niveau; i++) {
        printf("    ");
    }
    printf("Droite : ");
    afficherArbreEnSaage(a->fd, niveau + 1);
}