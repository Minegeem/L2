#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <MLV/MLV_all.h> 

// Dimensions de la grille en nombre de cases (origine en haut a gauche) :
#define COLONNES 12
#define LIGNES 18

// Les deux camps :
#define ABEILLE 'A'
#define FRELONCAMP 'F'

// Les types d'unites :
#define REINE 'r'
#define OUVRIERE 'o'
#define ESCADRON 'e'
#define GUERRIERE 'g'
#define FRELON 'f'
#define RUCHE 'R'
#define NID 'N'

// Pour la recolte de pollen
#define RECOLTE 'p'

// Les temps necessaires a la production :
#define TREINEA 8
#define TREINEF 8
#define TOUVRIERE 2
#define TGUERRIERE 4
#define TESCADRON 6
#define TFRELON 5
#define TRECOLTE 4


// Les couts necessaires a la production :
#define CREINEA 7
#define CREINEF 8
#define COUVRIERE 3
#define CGUERRIERE 5
#define CESCADRON 6
#define CFRELON 3
#define CRUCHE 10
#define CNID 10

// La force des unites
#define FREINE 6
#define FOUVRIERE 1
#define FGUERRIERE 5
#define FESCADRON 12
#define FFRELON 8

//variable globale:
int nbr_tour;
char Clan[1000];
int posx;
int posy;
char action[255]; 

// La structure Unite :
typedef struct unite {
  char camp; // ABEILLE ou FRELON
  char type; // RUCHE, NID, REINE, OUVRIERE, GUERRIERE, ESCADRON ou FRELON
  int force; // la force de l'unite
  int posx, posy; // position actuelle sur la grille
  int destx, desty; // destination (negatif si immobile)
  char production; // production d'une ruche ou d'un nid et RECOLTE pour la recolte de pollen
  int temps; // nombres de tours total pour cette production
  int toursrestant; // tours restant pour cette production
  struct unite *usuiv, *uprec; // liste des unites affiliees a une ruche ou un nid
  struct unite *colsuiv, *colprec; // liste des autres ruches ou nids (colonies) du même camp
  struct unite *vsuiv, *vprec; // liste des autres unites sur la meme case
} Unite, *UListe;



// La structure Case :
typedef struct {
  Unite *colonie; // S'il y a une ruche ou un nid sur la case
  UListe occupant; // les autres occupants de la case
} Case;

// La structure Grille :
typedef struct {
  Case plateau[LIGNES][COLONNES];
  UListe abeille, frelon;
  int tour; // Numero du tour
  int ressourcesAbeille, ressourcesFrelon;
} Grille;

//alloue une case
Case* init_case(void){
  Case* c = (Case *)malloc(sizeof(Case));
  c->colonie = NULL;
  c->occupant = NULL;
  return c;
}

typedef struct {
  int x,y;
}Point;


//créer un plateau de 18/12 de case alloué
void init_tab(Case plateau[LIGNES][COLONNES]){
    for(int i = 0; i<LIGNES;i++){
      for(int j = 0; j<COLONNES ; j++){
        plateau[i][j] = *init_case();
      }
    }
}
// Unite *nid0 = piece(FRELONCAMP,NID,0,17,11,' ',0,0);
//alloue chaque piece et renvoit un pointeur sur celle ci
Unite* piece(char camp,char type,int force,int posx,int posy,char production,int temps,int toursrestant){
  Unite *r = (Unite *)malloc(sizeof(Unite));
  if(r == NULL) return NULL;
  r->camp = camp;
  r->type = type;
  r->force = force;
  r->posx = posx;
  r->posy = posy;
  r->destx = -1;
  r->desty = -1;
  r->production = production;
  r->temps = temps;
  r->toursrestant = toursrestant;
  r->usuiv = NULL;
  r->uprec = NULL;
  r->colsuiv = NULL;
  r->colprec = NULL;
  r->vsuiv = NULL;
  r->vprec = NULL;
  return r;
}

//creer un type d'abeille (ruche,reine, ouvriere, gueriere,escadron)
Unite* creer_abeille(char abeille , int ruchex ,int ruchey, char production, int toursrestant){ /*Création d'unité coté Abeilles*/
    Unite* nouvelle_abeille;
    switch(abeille){
        case 'R':
        nouvelle_abeille = piece(ABEILLE,RUCHE,0,ruchex,ruchey,production,1,toursrestant);
        break;
        case 'r':
        nouvelle_abeille = piece(ABEILLE,REINE,FREINE,ruchex,ruchey,production,TREINEA,toursrestant);
        break;
        case 'o':
        nouvelle_abeille = piece(ABEILLE,OUVRIERE,FOUVRIERE,ruchex,ruchey,production,TOUVRIERE,toursrestant);
        break;
        case 'g':
        nouvelle_abeille = piece(ABEILLE,GUERRIERE,FGUERRIERE,ruchex,ruchey,production,TGUERRIERE,toursrestant);
        break;
        case 'e':
        nouvelle_abeille = piece(ABEILLE,ESCADRON,FESCADRON,ruchex,ruchey,production,TESCADRON,toursrestant);
        break;
    }
    return nouvelle_abeille;
}


//creer des frelons(nid,frelons,reine)
Unite* creer_frelon(char frelon,int ruchex, int ruchey, char production, int toursrestant ){ /*Création de d'unité coté Frelons*/
    Unite* nouveau_frelon;
    switch(frelon){
        case 'N':
        nouveau_frelon = piece(FRELONCAMP,NID,0,ruchex,ruchey,production,1,toursrestant);
        break;
        case 'r':
        nouveau_frelon = piece(FRELONCAMP,REINE,FREINE,ruchex,ruchey,production,TREINEF,toursrestant);
        break;
        case 'f':
        nouveau_frelon = piece(FRELONCAMP,FRELON,FFRELON,ruchex,ruchey,production,TFRELON,toursrestant);
        break;
    }
    return nouveau_frelon;
}

void affiche_case(int sizex,int sizey,int reine_abeille, int reine_frelon, int ouvriere, int maison, int frelon, int guerriere, int escadron ){
    //valeurs macabres du style 27 et 18 pck 60 trop grand donc tout est basé en fonction de 54 
    //(multiple inferieur de 60 divisible par 2 et 3 pour éviter les nombres à virgules)
    
    //reine abeille (jaune)
    if(reine_abeille) MLV_draw_filled_rectangle( sizex, sizey, 27, 18, MLV_COLOR_YELLOW );
    else MLV_draw_filled_rectangle( sizex, sizey, 27, 18, MLV_COLOR_DARKGREEN );
    //reine frelon (bleu fonce)
    if(reine_frelon) MLV_draw_filled_rectangle( sizex+27, sizey, 27, 18, MLV_COLOR_DARKBLUE );
    else MLV_draw_filled_rectangle( sizex+27, sizey, 27, 18, MLV_COLOR_DARKGREEN );
    //ouvriere (jaune clair)
    if(ouvriere) MLV_draw_filled_rectangle( sizex, sizey+18, 18, 18, MLV_COLOR_LIGHTYELLOW );
    else MLV_draw_filled_rectangle( sizex, sizey+18, 18, 18, MLV_COLOR_DARKGREEN );
    //Ruche/Nid (noir)
    if(maison) MLV_draw_filled_rectangle( sizex+18, sizey+18, 18, 18, MLV_COLOR_BLACK );
    else MLV_draw_filled_rectangle( sizex+18, sizey+18, 18, 18, MLV_COLOR_DARKGREEN );
    //Frelon (bleu)
    if(frelon) MLV_draw_filled_rectangle( sizex+36, sizey+18, 18, 18, MLV_COLOR_BLUE );
    else MLV_draw_filled_rectangle( sizex+36, sizey+18, 18, 18, MLV_COLOR_DARKGREEN );
    //Guerriere (orange)
    if(guerriere) MLV_draw_filled_rectangle( sizex, sizey+36, 27, 18, MLV_COLOR_ORANGE );
    else MLV_draw_filled_rectangle( sizex, sizey+36, 27, 18, MLV_COLOR_DARKGREEN );
    //Escadron (rouge)
    if(escadron) MLV_draw_filled_rectangle( sizex+27, sizey+36, 27, 18, MLV_COLOR_RED );
    else MLV_draw_filled_rectangle( sizex+27, sizey+36, 27, 18, MLV_COLOR_DARKGREEN );
}

void grille(int lig, int col,Grille jeu){
    Unite* tmp;
    int ra, rf, o, m, f, g, e;
    for(int i=0;i<COLONNES;i++){
      
        for(int j=0;j<LIGNES;j++){
            ra = 0;
            rf = 0;
            o = 0;
            m = 0;
            f = 0;
            g = 0;
            e = 0;
        
            if(jeu.plateau[j][i].colonie != NULL) m=1;
    
            tmp = jeu.plateau[j][i].occupant;
     
            while(tmp != NULL){
              switch (tmp->type){
              case 'r':
                if(tmp->camp == ABEILLE) ra=1;
                else rf=1;
                break;
              case 'f':
                f=1;
                break;
              case 'g':
                g=1;
                break;
              case 'e':
                e=1;
                break;
              case 'o':
                o=1;
                break;
              }
              tmp = tmp->vsuiv;
            }
            int it = i*54+600;
            int jt = j*54+20;
            affiche_case(it,jt,ra,rf,o,1,f,g,e);
          }   
        }
    MLV_update_window();
    }



void menu(char* pos, char* action){
    //affichage des coordonnées de l'unite en cours 
    MLV_draw_adapted_text_box(140,140,"Coordonnées de l'unite en cours:",10,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    //case pour faire une action (151,276) (362,389)
    MLV_draw_adapted_text_box(150,275,"Faire une action" , 50,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    // deplacer (149,400) (330,514)
    MLV_draw_adapted_text_box(150,400,"Se Deplacer" , 50,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    //skip (150,525) (344,639)
    MLV_draw_adapted_text_box(150,525,"Passer le tour" , 50,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    // action en cours (151,650) (378,765)
    MLV_draw_adapted_text_box(150,650,"Détruire une unité" , 50,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    // Détruire une unité (250,850)
    MLV_draw_adapted_text_box(140,890,"Action en cours:" , 10,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    MLV_draw_text(390,150,pos,MLV_COLOR_RED);// x à revoir en fonction de l'emplacement du texte
    MLV_draw_text(280,900,action,MLV_COLOR_RED); // x à revoir en fonction de l'emplacement du texte
    MLV_update_window();
    }

char affiche_choix_abeille(){
    int x;
    int y;
    //reine abeillle
    MLV_draw_filled_rectangle(600,300,100,100,MLV_COLOR_YELLOW3);
    MLV_draw_text(627,343,"reine",MLV_COLOR_BLUE);
    // ouvriere
    MLV_draw_filled_rectangle(800,300,100,100,MLV_COLOR_YELLOW3);
    MLV_draw_text(818,343,"ouvriere",MLV_COLOR_BLUE);
    // escadron
    MLV_draw_filled_rectangle(1000,300,100,100,MLV_COLOR_YELLOW3);
    MLV_draw_text(1016,343,"escadron",MLV_COLOR_BLUE);
    // guerriere
    MLV_draw_filled_rectangle(1200,300,100,100,MLV_COLOR_YELLOW3);
    MLV_draw_text(1215,343,"guerriere",MLV_COLOR_BLUE);
    MLV_update_window();
    do{MLV_wait_mouse(&x,&y);}
    while(!(x>600 && x<700 && y>300 && y<400) && !(x>1200 && x<1300 && y>300 && y<400) && !(x>800 && x<900 && y>300 && y<400) && !(x>1000 && x<1100 && y>300 && y<400));
    if(x>600 && x<700 && y>300 && y<400) return 'r';
    if(x>1200 && x<1300 && y>300 && y<400) return 'g';
    if(x>800 && x<900 && y>300 && y<400) return 'o';
    if(x>1000 && x<1100 && y>300 && y<400) return 'e';
    }


char affiche_choix_frelon(){
    // reine frelon
    int x;
    int y;
    MLV_draw_filled_rectangle(800,300,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(827,343,"reine",MLV_COLOR_YELLOW3);
    // frelon
    MLV_draw_filled_rectangle(1000,300,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(1027,343,"frelon",MLV_COLOR_YELLOW3);
    MLV_update_window();
    do{MLV_wait_mouse(&x,&y);}
    while(!(x>800 && x<900 && y>300 && y<400) && !(x>1000 && x<1100 && y>300 && y<400));
    if(x>800 && x<900 && y>300 && y<400) return 'r';
    if(x>1000 && x<1100 && y>300 && y<400) return 'f';
    }

void affiche_choix_deplacement(char* deplacement){
    int x;
    int y;
    MLV_draw_filled_rectangle(700,300,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(715,343,"Nord Ouest",MLV_COLOR_RED);
    
    MLV_draw_filled_rectangle(900,300,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(927,343,"Nord",MLV_COLOR_RED);
    
    MLV_draw_filled_rectangle(1100,300,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(1120,343,"Nord Est",MLV_COLOR_RED);
    
    MLV_draw_filled_rectangle(1100,500,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(1127,543,"Est",MLV_COLOR_RED);
    
    MLV_draw_filled_rectangle(1100,700,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(1127,743,"Sud Est",MLV_COLOR_RED);
    
    MLV_draw_filled_rectangle(900,700,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(927,743,"Sud",MLV_COLOR_RED);
    
    MLV_draw_filled_rectangle(700,700,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(720,743,"Sud Ouest",MLV_COLOR_RED);
    
    MLV_draw_filled_rectangle(700,500,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(727,543,"Ouest",MLV_COLOR_RED);
    
    MLV_update_window();
    do{MLV_wait_mouse(&x,&y);}
    while(!(x>700 && x<800 && y>300 && y<400) && !(x>900 && x<1000 && y>300 && y<400) && !(x>1100 && x<1200 && y>300 && y<400)
    && !(x>1100 && x<1200 && y>500 && y<600) && !(x>1100 && x<1200 && y>700 && y<800) && !(x>900 && x<1000 && y>700 && y<800)
    && !(x>700 && x<800 && y>700 && y<800) && !(x>700 && x<800 && y>500 && y<600));
    if (x>700 && x<800 && y>300 && y<400) strcpy(deplacement, "NO");
    if (x>900 && x<1000 && y>300 && y<400) strcpy(deplacement, "N");
    if (x>1100 && x<1200 && y>300 && y<400) strcpy(deplacement, "NE");
    if (x>1100 && x<1200 && y>500 && y<600) strcpy(deplacement, "E");
    if (x>1100 && x<1200 && y>700 && y<800) strcpy(deplacement, "SE");
    if (x>900 && x<1000 && y>700 && y<800) strcpy(deplacement, "S");
    if (x>700 && x<800 && y>700 && y<800) strcpy(deplacement, "SO");
    if (x>700 && x<800 && y>500 && y<600) strcpy(deplacement, "O");
    }
      
char affiche_choix_oui_non(){
    int x;
    int y;
    MLV_draw_filled_rectangle(800,300,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(827,343,"oui",MLV_COLOR_YELLOW3);
    
    MLV_draw_filled_rectangle(1000,300,100,100,MLV_COLOR_BLUE);
    MLV_draw_text(1027,343,"non",MLV_COLOR_YELLOW3);
    MLV_update_window();
    do{MLV_wait_mouse(&x,&y);}
    while(!(x>800 && x<900 && y>300 && y<400) && !(x>1000 && x<1100 && y>300 && y<400));
    if (x>800 && x<900 && y>300 && y<400) return 'o';
    if (x>1000 && x<1100 && y>300 && y<400) return 'n';
}
void partie(char* tour, char* clan){
    //affichage des coordonnées de l'unite en cours (250,225)
    MLV_draw_adapted_text_box(1510,140,"Nombre de tour:",10,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    //case pour faire une action (1520,275) (1767,389)
    MLV_draw_adapted_text_box(1520,275,"Sauvegarder la Partie" , 50,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    // deplacer (1520,400) (1772,514)
    MLV_draw_adapted_text_box(1520,400,"Charger la sauvegarde" , 50,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    //skip (1521,525) (1730,638)
    MLV_draw_adapted_text_box(1520,525,"Quitter la partie" , 50,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    // action en cours (1520,650) (1696,764)
    MLV_draw_adapted_text_box(1520,650,"Fin du tour" , 50,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    // Détruire une unité 
    MLV_draw_adapted_text_box(1510,890,"Clan du joueur en cours:" , 10,MLV_COLOR_YELLOW,MLV_COLOR_YELLOW,MLV_COLOR_BLACK,MLV_TEXT_CENTER);
    MLV_draw_text(1650,150,tour,MLV_COLOR_RED);// x à revoir en fonction de l'emplacement du texte
    MLV_draw_text(1700,900,clan,MLV_COLOR_RED); // x à revoir en fonction de l'emplacement du texte
    MLV_update_window();
    }

int affiche_choix_action(){
    int x;
    int y;
    do{MLV_wait_mouse(&x,&y);}
    while(!(x>150 && x<330 && y>400 && y<514) && !(x>150 && x<362 && y>276 && y<390)
        && !(x>150 && x<345 && y>525 && y<640)&& !(x>150 && x<378 && y>650 && y<765));
    if (x>150 && x<330 && y>400 && y<514) return 1;//se deplacer
    if (x>150 && x<362 && y>276 && y<390) return 2;//action
    if (x>150 && x<345 && y>525 && y<640) return 3;//skip
    if (x>150 && x<378 && y>650 && y<765) return 4;//detruire
    
    }

int affiche_choix_action_ouvriere(){
    int x;
    int y;
    do{MLV_wait_mouse(&x,&y);}
    while(!(x>150 && x<330 && y>400 && y<514) && !(x>150 && x<362 && y>276 && y<390)
        && !(x>150 && x<378 && y>650 && y<765));
    if (x>150 && x<330 && y>400 && y<514) return 1;//se deplacer
    if (x>150 && x<362 && y>276 && y<390) return 2;//action
    if (x>150 && x<378 && y>650 && y<765) return 3;//detruire
    
    }

int affiche_choix_action_special(){
    int x;
    int y;
    do{MLV_wait_mouse(&x,&y);}
    while(!(x>150 && x<330 && y>400 && y<514) && !(x>150 && x<345 && y>525 && y<640)&& !(x>150 && x<378 && y>650 && y<765));
    if (x>150 && x<330 && y>400 && y<514) return 1;//se deplacer
    if (x>150 && x<345 && y>525 && y<640) return 2;//skip
    if (x>150 && x<378 && y>650 && y<765) return 3;//detruire
    
    }

void retour_au_jeu(Grille *jeu){
    char ann[255];
    char position[20];

    grille(1248,972,*jeu);

    sprintf(position,"%d %d",posx,posy);
    menu(position,action);
    sprintf(ann,"%d",nbr_tour);
    partie(ann,Clan);
    MLV_update_window();
    }

void annonce(char* texte,Grille *jeu){
    int x,y;
    MLV_draw_filled_rectangle(600,300,648,200,MLV_COLOR_RED);
    MLV_draw_text(900,400,texte,MLV_COLOR_WHITE);
    MLV_update_window();
    MLV_wait_mouse(&x,&y);

    retour_au_jeu(jeu);

    }
void sur_annonce(char texte[]){
    MLV_draw_filled_rectangle(600,300,648,200,MLV_COLOR_RED);
    MLV_draw_text(900,400,texte,MLV_COLOR_WHITE);
    }
void sous_annonce(char* texte,Grille *jeu){
  int x,y;
    MLV_draw_filled_rectangle(600,300,648,200,MLV_COLOR_RED);
    MLV_draw_text(610,450,texte,MLV_COLOR_WHITE);
    MLV_update_window();
    MLV_wait_mouse(&x,&y);
    retour_au_jeu(jeu);
    }


//deplace une unité precise (PAS TESTER)
void deplacer(Unite* creat,Grille *jeu){
  //j'ai preferé faire avec un switch, comme ca si on change la condition (genre on passe du scanf à un clique de la souris), on aura juste à changer la manière d'avoir les informations et pas le switch
  char deplacement[3] = "";
  int res;
  int tmpx=creat->posx;
  int tmpy=creat->posy;
  do{
    res = 0;
    affiche_choix_deplacement(deplacement);
    res = strcmp(deplacement,"NO");
    if(res == 0){
       if ((0<=tmpx-1 && tmpx-1<18) && (0<=tmpy-1 && tmpy-1<12)){
        creat->posx = tmpx-1;
        creat->posy = tmpy-1;
        break;
      } 
      else 
        annonce("Impossible d'accéder à cette endroit Réessayer ",jeu);
    }
    res = strcmp(deplacement,"N");
    if(res == 0){
      if (0<=tmpx-1 && tmpx-1<18){
          creat->posx = tmpx-1;
          break;
        } 
      else 
        annonce("Impossible d'accéder à cette endroit Réessayer ",jeu);
    }
    res = strcmp(deplacement,"NE");
    if(res == 0){
      if ((0<=tmpx-1 && tmpx-1<18) && (0<=tmpy+1 && tmpy+1<12)){
          creat->posx = tmpx-1;
          creat->posy = tmpy+1;
          break;
        } 
      else 
        annonce("Impossible d'accéder à cette endroit Réessayer ",jeu);
    }
    res = strcmp(deplacement,"E");
    if(res == 0){
      if (0<=tmpy+1 && tmpy+1<12){
          creat->posy = tmpy+1;
          break;
        } 
      else 
        annonce("Impossible d'accéder à cette endroit Réessayer ",jeu);
    }
    res = strcmp(deplacement,"SE");
    if(res == 0){
      if ((0<=tmpx+1 && tmpx+1<18) && (0<=tmpy+1 && tmpy+1<12)){
          creat->posx = tmpx+1;
          creat->posy = tmpy+1;
          break;
        } 
      else 
        annonce("Impossible d'accéder à cette endroit Réessayer ",jeu);
    }
    res = strcmp(deplacement,"S");
    if(res == 0){
      if (0<=tmpx+1 && tmpx+1<18){
          creat->posx = tmpx+1;
          break;
        } 
      else 
        annonce("Impossible d'accéder à cette endroit Réessayer ",jeu);
    }
    res = strcmp(deplacement,"SO");
    if(res == 0){
      if ((0<=tmpx+1 && tmpx+1<18) && (0<=tmpy-1 && tmpy-1<12)){
          creat->posx = tmpx+1;
          creat->posy = tmpy-1;
          break;
        } 
      else 
        annonce("Impossible d'accéder à cette endroit Réessayer ",jeu);
    }
    res = strcmp(deplacement,"O");
    if(res == 0){
      if (0<=tmpy-1 && tmpy-1<12){
          creat->posy = tmpy-1;
          break;
        } 
      else 
        annonce("Impossible d'accéder à cette endroit Réessayer ",jeu);
    }
  }while(1);
}


//pas oublier de quand la récolte est fini, reset la valeur de production
//pas oublier de vérifier apres chaque tour dans le main si toursrestant est égal à 0, si oui, ajouter une ressource pollen et tuer l'ouvriere sans rapporter de ressource au frelon
void recolter_pollen(Unite* creat){
    if(creat->production = ' '){
        creat->production='p'; /*pour pollen*/ 
        creat->destx = -1;
        creat->desty = -1; //immobile quand récolte du pollen, meurt apres donc pas besoin de modif sa valeur
        creat->temps = 4;
        creat->toursrestant = 4;
  }
}

//lancer de dé aleatoire
int tire_de(){
  return rand()%60 + 1;
}

//combat entre 2 unités de cmap different
int combat(char abeille,char frelons){ 
  int res1 = tire_de();
  int res2 = tire_de();
  switch (abeille){
  case REINE:
    res1 = res1 * FREINE;
    break;
  
  case OUVRIERE:
    res1 = res1 * FOUVRIERE;
    break;
  
  case GUERRIERE:
    res1 = res1 * FGUERRIERE;
    break;
  
  case ESCADRON:
    res1 = res1 * FESCADRON;
    break;
  
  default:
    res1 = 0;
    break;
  }
  switch (frelons){
  case REINE:
    res2 = res2 * FREINE;
    break;

  case FRELON:
    res2 = res2 * FFRELON;
    break;

  default:
    res2 = 0;
    break;
  }
  return res1-res2;
}

//ajoute une colonie a une case mais aussi dans la liste de ruche ou de nid
void ajoute_colonie(Grille* jeu, Unite* new_occupant, int posx, int posy){
    jeu->plateau[posx][posy].colonie = new_occupant;
    Unite* tmp ;
    if(new_occupant->camp == ABEILLE){
      tmp = jeu->abeille;
      if(jeu->abeille == NULL) jeu->abeille = new_occupant;
      else{
        while(tmp->colsuiv != NULL){
          tmp = tmp->colsuiv;
        }
        tmp->colsuiv = new_occupant;
        new_occupant->colprec = tmp;
      }
    }
    
    else{
      tmp = jeu->frelon;
      if(jeu->frelon == NULL) jeu->frelon = new_occupant;
      else {
        while(tmp->colsuiv != NULL){
          tmp = tmp->colsuiv;
        }
        tmp->colsuiv = new_occupant;
        new_occupant->colprec = tmp;
      }
    }
}

//ajoute une Unité a une case du plateau
void ajoute_occupant(Unite** Liste, Unite* new_occupant){    
  Unite* tmp = *Liste;
  if(tmp == NULL) *Liste = new_occupant;
  else {switch (new_occupant->type)
  {
    case ESCADRON:
      new_occupant->vsuiv = tmp;
      tmp->vprec = new_occupant;
      *Liste = new_occupant;
      break;
    case GUERRIERE:
      while(tmp->vsuiv != NULL && tmp->type == ESCADRON){
        tmp = tmp->vsuiv;
      }
      if(tmp->vsuiv == NULL && tmp->vprec == NULL){
        if(tmp->type == ESCADRON){
          tmp->vsuiv = new_occupant;
          new_occupant->vprec = tmp;
        }
        else{
          new_occupant->vsuiv = tmp;
          tmp->vprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else if (tmp->vsuiv == NULL)
      {
        if(tmp->type == ESCADRON){
          tmp->vsuiv = new_occupant;
          new_occupant->vprec = tmp;
        }
        else{
          new_occupant->vprec = tmp->vprec;
          new_occupant->vsuiv = tmp;
          new_occupant->vprec->vsuiv = new_occupant;
          tmp->vprec = new_occupant;
        }
      }
      else if (tmp->vprec == NULL)
      {
        if(tmp->type == ESCADRON){
          new_occupant->vsuiv = tmp->vsuiv;
          new_occupant->vprec = tmp;
          new_occupant->vsuiv->vprec = new_occupant;
          tmp->vsuiv = new_occupant;
        }
        else{
          new_occupant->vsuiv = tmp;
          tmp->vprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else
      {
        new_occupant->vsuiv = tmp;
        new_occupant->vprec = tmp->vprec;
        tmp->vprec->vsuiv = new_occupant;
        tmp->vprec = new_occupant;
      }
      break;

    case REINE:
      if(new_occupant->camp == ABEILLE){
        while (tmp->vsuiv != NULL && (tmp->type == ESCADRON || tmp->type == GUERRIERE)){
          tmp = tmp->vsuiv;
        }
        if(tmp->vsuiv == NULL && tmp->vprec == NULL){
          if(tmp->type == ESCADRON || tmp->type == GUERRIERE){
            tmp->vsuiv = new_occupant;
            new_occupant->vprec = tmp;
        }
        else{
          new_occupant->vsuiv = tmp;
          tmp->vprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else if (tmp->vsuiv == NULL)
      {
        if(tmp->type == ESCADRON || tmp->type == GUERRIERE){
          tmp->vsuiv = new_occupant;
          new_occupant->vprec = tmp;
        }
        else{
          new_occupant->vprec = tmp->vprec;
          new_occupant->vsuiv = tmp;
          new_occupant->vprec->vsuiv = new_occupant;
          tmp->vprec = new_occupant;
        }
      }
      else if (tmp->vprec == NULL)
      {
        if(tmp->type == ESCADRON || tmp->type == GUERRIERE){
          new_occupant->vsuiv = tmp->vsuiv;
          new_occupant->vprec = tmp;
          new_occupant->vsuiv->vprec = new_occupant;
          tmp->vsuiv = new_occupant;
        }
        else{
          new_occupant->vsuiv = tmp;
          tmp->vprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else
      {
        new_occupant->vsuiv = tmp;
        new_occupant->vprec = tmp->vprec;
        tmp->vprec->vsuiv = new_occupant;
        tmp->vprec = new_occupant;
      }
      }else {
        while (tmp->vsuiv != NULL && (tmp->type == ESCADRON || tmp->type == GUERRIERE || tmp->type == OUVRIERE || (tmp->type == REINE && tmp->camp == ABEILLE))){
          tmp = tmp->vsuiv;
        }
        if(tmp->vsuiv == NULL && tmp->vprec == NULL){
        if(tmp->type == ESCADRON || tmp->type == GUERRIERE || tmp->type == OUVRIERE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          tmp->vsuiv = new_occupant;
          new_occupant->vprec = tmp;
        }
        else{
          new_occupant->vsuiv = tmp;
          tmp->vprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else if (tmp->vsuiv == NULL)
      {
        if(tmp->type == ESCADRON || tmp->type == GUERRIERE || tmp->type == OUVRIERE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          tmp->vsuiv = new_occupant;
          new_occupant->vprec = tmp;
        }
        else{
          new_occupant->vprec = tmp->vprec;
          new_occupant->vsuiv = tmp;
          new_occupant->vprec->vsuiv = new_occupant;
          tmp->vprec = new_occupant;
        }
      }
      else if (tmp->vprec == NULL)
      {
        if(tmp->type == ESCADRON || tmp->type == GUERRIERE || tmp->type == OUVRIERE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          new_occupant->vsuiv = tmp->vsuiv;
          new_occupant->vprec = tmp;
          new_occupant->vsuiv->vprec = new_occupant;
          tmp->vsuiv = new_occupant;
        }
        else{
          new_occupant->vsuiv = tmp;
          tmp->vprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else
      {
        new_occupant->vsuiv = tmp;
        new_occupant->vprec = tmp->vprec;
        tmp->vprec->vsuiv = new_occupant;
        tmp->vprec = new_occupant;
      }
    }
      break;
    case OUVRIERE:
      while (tmp->vsuiv != NULL && (tmp->type == ESCADRON || tmp->type == GUERRIERE || (tmp->type == REINE && tmp->camp == ABEILLE))){
        tmp = tmp->vsuiv;
      }
      if(tmp->vsuiv == NULL && tmp->vprec == NULL){
        if(tmp->type == ESCADRON || tmp->type == GUERRIERE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          tmp->vsuiv = new_occupant;
          new_occupant->vprec = tmp;
        }
        else{
          new_occupant->vsuiv = tmp;
          tmp->vprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else if (tmp->vsuiv == NULL)
      {
        if(tmp->type == ESCADRON || tmp->type == GUERRIERE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          tmp->vsuiv = new_occupant;
          new_occupant->vprec = tmp;
        }
        else{
          new_occupant->vprec = tmp->vprec;
          new_occupant->vsuiv = tmp;
          new_occupant->vprec->vsuiv = new_occupant;
          tmp->vprec = new_occupant;
        }
      }
      else if (tmp->vprec == NULL)
      {
        if(tmp->type == ESCADRON || tmp->type == GUERRIERE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          new_occupant->vsuiv = tmp->vsuiv;
          new_occupant->vprec = tmp;
          new_occupant->vsuiv->vprec = new_occupant;
          tmp->vsuiv = new_occupant;
        }
        else{
          new_occupant->vsuiv = tmp;
          tmp->vprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else
      {
        new_occupant->vsuiv = tmp;
        new_occupant->vprec = tmp->vprec;
        tmp->vprec->vsuiv = new_occupant;
        tmp->vprec = new_occupant;
      }
      break;
    case FRELON:
      while (tmp->vsuiv != NULL ){
        tmp = tmp->vsuiv;
      }
      tmp->vsuiv = new_occupant;
      new_occupant->vprec = tmp;
      break;
    }
    
  }
}

//ajoute une unité a une colonie
void ajoute_unite(Unite** Liste, Unite* new_occupant){
  Unite* tmp = *Liste;
  if(tmp == NULL) *Liste = new_occupant;
  else {switch (new_occupant->type)
  {

    case REINE:
      if(new_occupant->camp == ABEILLE){
        while (tmp->usuiv != NULL && tmp->type == RUCHE){
          tmp = tmp->usuiv;
        }
        if(tmp->usuiv == NULL && tmp->uprec == NULL){
          if(tmp->type == RUCHE){
            tmp->usuiv = new_occupant;
            new_occupant->uprec = tmp;
        }
        else{
          new_occupant->usuiv = tmp;
          *Liste = new_occupant;
        }
      }
      else if (tmp->usuiv == NULL)
      {
        if(tmp->type == RUCHE){
          tmp->usuiv = new_occupant;
          new_occupant->uprec = tmp;
        }
        else{
          new_occupant->uprec = tmp->uprec;
          new_occupant->usuiv = tmp;
          new_occupant->uprec->usuiv = new_occupant;
          tmp->uprec = new_occupant;
        }
      }
      else if (tmp->uprec == NULL)
      {
        if(tmp->type == RUCHE){
          new_occupant->usuiv = tmp->usuiv;
          new_occupant->uprec = tmp;
          new_occupant->usuiv->uprec = new_occupant;
          tmp->usuiv = new_occupant;
        }
        else{
          new_occupant->usuiv = tmp;
          tmp->uprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else
      {
        new_occupant->usuiv = tmp;
        new_occupant->uprec = tmp->uprec;
        tmp->uprec->usuiv = new_occupant;
        tmp->uprec = new_occupant;
      }
      }else {
        while (tmp->usuiv != NULL && tmp->type == NID ){
          tmp = tmp->usuiv;
        }
        if(tmp->usuiv == NULL && tmp->uprec == NULL){
        if(tmp->type == NID){
          tmp->usuiv = new_occupant;
          new_occupant->uprec = tmp;
        }
        else{
          new_occupant->usuiv = tmp;
          *Liste = new_occupant;
        }
      }
      else if (tmp->usuiv == NULL)
      {
        if(tmp->type == NID){
          tmp->usuiv = new_occupant;
          new_occupant->uprec = tmp;
        }
        else{
          new_occupant->uprec = tmp->uprec;
          new_occupant->usuiv = tmp;
          new_occupant->uprec->usuiv = new_occupant;
          tmp->uprec = new_occupant;
        }
      }
      else if (tmp->uprec == NULL)
      {
        if(tmp->type == NID){
          new_occupant->usuiv = tmp->usuiv;
          new_occupant->uprec = tmp;
          new_occupant->usuiv->uprec = new_occupant;
          tmp->usuiv = new_occupant;
        }
        else{
          new_occupant->usuiv = tmp;
          tmp->uprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else
      {
        new_occupant->usuiv = tmp;
        new_occupant->uprec = tmp->uprec;
        tmp->uprec->usuiv = new_occupant;
        tmp->uprec = new_occupant;
      }
      break;

    case OUVRIERE:
      while (tmp->usuiv != NULL && (tmp->type == RUCHE || (tmp->type == REINE && tmp->camp == ABEILLE))){
        tmp = tmp->usuiv;
      }
      if(tmp->usuiv == NULL && tmp->uprec == NULL){
        if(tmp->type == RUCHE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          tmp->usuiv = new_occupant;
          new_occupant->uprec = tmp;
        }
        else{
          new_occupant->usuiv = tmp;
          *Liste = new_occupant;
        }
      }
      else if (tmp->usuiv == NULL)
      {
        if(tmp->type == RUCHE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          tmp->usuiv = new_occupant;
          new_occupant->uprec = tmp;
        }
        else{
          new_occupant->uprec = tmp->uprec;
          new_occupant->usuiv = tmp;
          new_occupant->uprec->usuiv = new_occupant;
          tmp->uprec = new_occupant;
        }
      }
      else if (tmp->uprec == NULL)
      {
        if(tmp->type == RUCHE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          new_occupant->usuiv = tmp->usuiv;
          new_occupant->uprec = tmp;
          new_occupant->usuiv->uprec = new_occupant;
          tmp->usuiv = new_occupant;
        }
        else{
          new_occupant->usuiv = tmp;
          tmp->uprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else
      {
        new_occupant->usuiv = tmp;
        new_occupant->uprec = tmp->uprec;
        tmp->uprec->usuiv = new_occupant;
        tmp->uprec = new_occupant;
      }
      break;

    case GUERRIERE:
      while(tmp->usuiv != NULL && (tmp->type == RUCHE || tmp->type == OUVRIERE || (tmp->type == REINE && tmp->camp == ABEILLE))){
        tmp = tmp->usuiv;
      }
      if(tmp->usuiv == NULL && tmp->uprec == NULL){
        if(tmp->type == RUCHE || tmp->type == OUVRIERE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          tmp->usuiv = new_occupant;
          new_occupant->uprec = tmp;
        }
        else{
          new_occupant->usuiv = tmp;
          *Liste = new_occupant;
        }
      }
      else if (tmp->usuiv == NULL)
      {
        if(tmp->type == RUCHE || tmp->type == OUVRIERE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          tmp->usuiv = new_occupant;
          new_occupant->uprec = tmp;
        }
        else{
          new_occupant->uprec = tmp->uprec;
          new_occupant->usuiv = tmp;
          new_occupant->uprec->usuiv = new_occupant;
          tmp->uprec = new_occupant;
        }
      }
      else if (tmp->uprec == NULL)
      {
        if(tmp->type == RUCHE || tmp->type == OUVRIERE || (tmp->type == REINE && tmp->camp == ABEILLE)){
          new_occupant->usuiv = tmp->usuiv;
          new_occupant->uprec = tmp;
          new_occupant->usuiv->uprec = new_occupant;
          tmp->usuiv = new_occupant;
        }
        else{
          new_occupant->usuiv = tmp;
          tmp->uprec = new_occupant;
          *Liste = new_occupant;
        }
      }
      else
      {
        new_occupant->usuiv = tmp;
        new_occupant->uprec = tmp->uprec;
        tmp->uprec->usuiv = new_occupant;
        tmp->uprec = new_occupant;
      }
      }
      break;

    case ESCADRON:
      while (tmp->usuiv != NULL ){
        tmp = tmp->usuiv;
      }
      tmp->usuiv = new_occupant;
      new_occupant->uprec = tmp;
      break;


    case FRELON:
      while (tmp->usuiv != NULL ){
        tmp = tmp->usuiv;
      }
      tmp->usuiv = new_occupant;
      new_occupant->uprec = tmp;
      break;
    }
    
  }
}

void enleve_occupant(Unite** liste, Unite* unite){//supprime un ocuppant d'une case
Unite* tmp;
  if(unite->vsuiv == NULL && unite->vprec == NULL) *liste = NULL;
  else if(unite->vsuiv == NULL) {
    unite->vprec->vsuiv = NULL;
  }
  else if(unite->vprec == NULL){
    *liste = unite->vsuiv;
    unite->vsuiv->vprec = NULL; 
  }
  else{
    tmp = unite->vprec;
    tmp->vsuiv = unite->vsuiv;
    
    unite->vsuiv->vprec = unite->vprec;
  }
  unite->vprec = NULL;
  unite->vsuiv = NULL;
}

void enleve_colonie(Unite** liste, Unite* unite){//supprime une unité d'une colonie
  if(unite->usuiv == NULL && unite->uprec == NULL) *liste = NULL;
  else if(unite->usuiv == NULL) unite->uprec->usuiv = NULL;
  else if(unite->uprec == NULL){
    *liste = unite->usuiv;
    unite->usuiv->uprec = NULL; 
  }
  else{
    unite->uprec->usuiv = unite->usuiv;
    unite->usuiv->uprec = unite->uprec;
  }
  unite->uprec = NULL;
  unite->usuiv = NULL;
}

//initialisation du jeu au depart
void initialisation_jeu(Grille* jeu){
    Unite *R = creer_abeille(RUCHE,0,0,' ',0);
    Unite *g = creer_abeille(GUERRIERE,0,0,' ',0);
    Unite *o = creer_abeille(OUVRIERE,0,0,' ',0);
    Unite *ra = creer_abeille(REINE,0,0,' ',0); 
    ajoute_colonie(jeu,R,0,0);
    ajoute_unite(&R,g);  
    ajoute_unite(&R,o);
    ajoute_unite(&R,ra);
    ajoute_occupant(&jeu->plateau[0][0].occupant,ra);
    ajoute_occupant(&jeu->plateau[0][0].occupant,g);
    ajoute_occupant(&jeu->plateau[0][0].occupant,o);
    Unite *N = creer_frelon(NID,17,11,' ',0);
    Unite *f1 = creer_frelon(FRELON,17,11,' ',0);
    Unite *f2 = creer_frelon(FRELON,17,11,' ',0);
    Unite *rf = creer_frelon(REINE,17,11,' ',0);
    ajoute_colonie(jeu,N,17,11);
    ajoute_unite(&N,rf);
    ajoute_unite(&N,f1);
    ajoute_unite(&N,f2);
    ajoute_occupant(&jeu->plateau[17][11].occupant,f1);
    ajoute_occupant(&jeu->plateau[17][11].occupant,f2);
    ajoute_occupant(&jeu->plateau[17][11].occupant,rf);
    jeu->ressourcesAbeille = 10;
    jeu->ressourcesFrelon = 10;   
}

//verification si il y a un combat sur le plateau a utiliser apres la fin d'un tour
Point verif_combat(Grille jeu){
  Unite *tmp;
  for(int posx = 0; posx<LIGNES; posx++){
    for(int posy = 0; posy<COLONNES; posy++){
      tmp = jeu.plateau[posx][posy].occupant;
      if(tmp == NULL) continue;
      while(tmp->vsuiv != NULL){
        tmp = tmp->vsuiv;
      }
      if(((jeu.plateau[posx][posy].colonie != NULL && jeu.plateau[posx][posy].colonie->camp == ABEILLE) || jeu.plateau[posx][posy].occupant->camp == ABEILLE) && ((jeu.plateau[posx][posy].colonie != NULL && jeu.plateau[posx][posy].colonie->camp == FRELONCAMP) || tmp->camp == FRELONCAMP)){
        Point P = {posx,posy};
        return P;
      }
    }
  }
  Point P = {-1,-1};
  return P;
}

void question(Unite* unit, Grille* jeu){ //pose toute les question du jeux et les executes
  char reponse = ' ';
  int res = 0;
  Unite* tmp;
  Unite* R;
  char ann[255];
  switch(unit->type){

    case RUCHE :
    if(unit->toursrestant > 0){
      sprintf(ann,"La Ruche en case x = %d et y = %d", unit->posx, unit->posy);
      sur_annonce(ann);
      sprintf(ann,"Ne peut pas encore crée de nouvelles unités attendez encore %d tours\n",unit->toursrestant);
      sous_annonce(ann,jeu);
      posx=unit->posx;
      posy=unit->posy;
      strcmp(action,"Impossible");
      retour_au_jeu(jeu);
      break;
    }
    do{
    sprintf(ann,"La Ruche en case x = %d et y = %d", unit->posx, unit->posy);
    sur_annonce(ann);
    sous_annonce("Voulez vous crée une nouvelle unité ?(o ou n)\n",jeu);
    posx=unit->posx;
    posy=unit->posy;
    strcmp(action,"Création d'unités");
    retour_au_jeu(jeu);
    reponse=affiche_choix_oui_non();
    }while(reponse != 'o' && reponse != 'n');
    if(reponse == 'o'){
      
        do{
          reponse=affiche_choix_abeille();
          switch(reponse){
            case 'r':
              if(jeu->ressourcesAbeille < CREINEA){
                annonce("Probleme pas assez de Pollen",jeu);
                reponse = ' '; 
              }
              else{
                Unite* c = creer_abeille(reponse,unit->posx,unit->posy,' ',TREINEA);
                ajoute_occupant(&jeu->plateau[unit->posx][unit->posy].occupant,c);
                ajoute_unite(&jeu->plateau[unit->posx][unit->posy].colonie,c);
                jeu->ressourcesAbeille -= CREINEA;
                unit->temps = TREINEA;
                unit->toursrestant = TREINEA;
              }
              break;

            case 'o':
              if(jeu->ressourcesAbeille < COUVRIERE){
                annonce("Probleme pas assez de Pollen",jeu);
                reponse = ' '; 
              }
              else{
                Unite* c = creer_abeille(reponse,unit->posx,unit->posy,' ',TOUVRIERE);
                ajoute_occupant(&jeu->plateau[unit->posx][unit->posy].occupant,c);
                ajoute_unite(&jeu->plateau[unit->posx][unit->posy].colonie,c);
                jeu->ressourcesAbeille -= COUVRIERE;
                unit->temps = TOUVRIERE;
                unit->toursrestant = TOUVRIERE;
              }
              break;

            case 'g':
              if(jeu->ressourcesAbeille < CGUERRIERE){
                annonce("Probleme pas assez de Pollen",jeu);
                reponse = ' '; 
              }
              else{
                Unite* c = creer_abeille(reponse,unit->posx,unit->posy,' ',TGUERRIERE);
                ajoute_occupant(&jeu->plateau[unit->posx][unit->posy].occupant,c);
                ajoute_unite(&jeu->plateau[unit->posx][unit->posy].colonie,c);
                jeu->ressourcesAbeille -= CGUERRIERE;
                unit->temps = TGUERRIERE;
                unit->toursrestant = TGUERRIERE;
              }
              break;

            case 'e':
              if(jeu->ressourcesAbeille < CESCADRON){
                annonce("Probleme pas assez de Pollen",jeu);
                reponse = ' '; 
              }
              else{
                Unite* c = creer_abeille(reponse,unit->posx,unit->posy,' ',TESCADRON);
                ajoute_occupant(&jeu->plateau[unit->posx][unit->posy].occupant,c);
                ajoute_unite(&jeu->plateau[unit->posx][unit->posy].colonie,c);
                jeu->ressourcesAbeille -= CESCADRON;
                unit->temps = TESCADRON;
                unit->toursrestant = TESCADRON;
              }
              break;

            case 'q':
              break;
          }
        }while(reponse != 'r' && reponse != 'o' && reponse != 'g' && reponse != 'e' && reponse != 'q');
    }
      break;

    case NID :
      if(unit->toursrestant > 0){
        sprintf(ann,"Le Nid en case x = %d et y = %d", unit->posx, unit->posy);
        sur_annonce(ann);
        sous_annonce("Voulez vous crée une nouvelle unité ?(o ou n)\n",jeu);
        posx=unit->posx;
        posy=unit->posy;
        strcmp(action,"Création d'unités");
        retour_au_jeu(jeu);
        reponse=affiche_choix_oui_non();
        break;
      }
      
      do{
      sprintf(ann,"Le Nid en case x = %d et y = %d", unit->posx, unit->posy);
      sur_annonce(ann);
      sous_annonce("Voulez vous crée une nouvelle unité ?",jeu);
      reponse=affiche_choix_oui_non();
      posx=unit->posx;
      posy=unit->posy;
      strcmp(action,"Création d'unités");
      retour_au_jeu(jeu);
      }while(reponse != 'o' && reponse != 'n');
      reponse=affiche_choix_oui_non();
      if(reponse == 'o'){
        reponse=affiche_choix_frelon();
          switch(reponse){
            case 'r':
            if(jeu->ressourcesFrelon < CREINEF){
              annonce("Probleme pas assez de Pollen",jeu);
              reponse = ' '; 
            }
            else{
              Unite* c = creer_frelon(reponse,unit->posx,unit->posy,' ',TREINEF);
              ajoute_occupant(&jeu->plateau[unit->posx][unit->posy].occupant,c);
              ajoute_unite(&jeu->plateau[unit->posx][unit->posy].colonie,c);
              jeu->ressourcesFrelon -= CREINEF;
              unit->temps = TREINEF;
              unit->toursrestant = TREINEF;
            }
            break;

            case 'f':
            if(jeu->ressourcesFrelon < CFRELON){
              annonce("Probleme pas assez de Pollen",jeu);
              reponse = ' '; 
            }
            else{
              Unite* c = creer_frelon(reponse,unit->posx,unit->posy,' ',TFRELON);
              ajoute_occupant(&jeu->plateau[unit->posx][unit->posy].occupant,c);
              ajoute_unite(&jeu->plateau[unit->posx][unit->posy].colonie,c);
              jeu->ressourcesAbeille -= CFRELON;
              unit->temps = TFRELON;
              unit->toursrestant = TFRELON;
            }
            break;

            case 'q':
            break;
          }
      } 
    
    break;

    case REINE:
      if(unit->toursrestant > 0){
        sprintf(ann,"La Reine du camp '%c en case x = %d et y = %d", unit->camp, unit->posx, unit->posy);
        sur_annonce(ann);
        sprintf(ann,"La Ruche en case x = %d et y = %d", unit->posx, unit->posy);
        posx=unit->posx;
        posy=unit->posy;
        sprintf(ann,"Ne peut pas encore crée de nouvelles unités ou se deplacer attendez encore %d tours\n",unit->toursrestant);
        sous_annonce(ann,jeu);
        strcmp(action,"Impossible");
        retour_au_jeu(jeu);
        break;
      }
      
      sprintf(ann,"La Reine du camp '%c' en case x = %d et y = %d\n", unit->camp , unit->posx , unit->posy );
      sous_annonce(ann,jeu);
      sous_annonce("Choisissez parmis les différentes action sur la gauche",jeu);
      do{
      res=affiche_choix_action();
      posx=unit->posx;
      posy=unit->posy;
      if (res==1) strcmp(action,"se déplace");
      if (res==2) strcmp(action,"création d'une ruche");
      if (res==3) strcmp(action,"passer le tour");
      if (res==4) strcmp(action,"Détruire l'unite");
      retour_au_jeu(jeu);
      switch(res){
        case 1:
        enleve_occupant(&jeu->plateau[unit->posx][unit->posy].occupant, unit);
        deplacer(unit,jeu);
        ajoute_occupant(&jeu->plateau[unit->posx][unit->posy].occupant,unit);
        break;
        case 2:
        if(jeu->plateau[unit->posx][unit->posy].colonie != NULL){
          annonce("Une colonie existe deja",jeu);
          res = -1;
        }
        else if(jeu->ressourcesAbeille < CRUCHE){
          annonce("Vous n'avez pas assez de pollen",jeu);
          res = -1;
        }
        else {
          if(unit->camp == ABEILLE)
            R = creer_abeille(RUCHE,unit->posx,unit->posy,' ',1);
          if(unit->camp == FRELONCAMP)
            R = creer_frelon(NID,unit->posx,unit->posy,' ',1);
          ajoute_colonie(jeu,R,unit->posx,unit->posy);
          tmp = unit;
          while(tmp->uprec != NULL){
            tmp = tmp->uprec;
          }
          enleve_colonie(&tmp,unit);
          ajoute_unite(&R,unit);
        }
        break;
        case 3:
        break;
        case 4:
        tmp = unit;
          while(tmp->uprec != NULL){
            tmp = tmp->uprec;
          }
        enleve_colonie(&tmp,unit);
        enleve_occupant(&jeu->plateau[unit->posx][unit->posy].occupant, unit);
        free(unit);
        break;
      }
      }while(res < 1 && res > 4);
      break;

    case OUVRIERE:
      if(unit->toursrestant > 0){
        sprintf(ann,"L'Ouvrière en case x = %d et y = %d", unit->posx, unit->posy);
        sur_annonce(ann);
        sprintf(ann,"Est en train de recuperer du pollen attendez encore %d tours\n", unit->toursrestant);
        sous_annonce(ann,jeu);
      break;
    }
    
    sprintf(ann,"L'Ouvrière en case x = %d et y = %d\n", unit->posx, unit->posy);
    annonce(ann,jeu);
    sous_annonce("Choisissez parmis les différentes actions sur la gauche",jeu);
    do{
    res=affiche_choix_action_ouvriere();
    posx=unit->posx;
    posy=unit->posy;
    if (res==1) strcmp(action,"se déplace");
    if (res==2) strcmp(action,"récolte de pollen");
    if (res==3) strcmp(action,"Détruire l'unite");
    retour_au_jeu(jeu);
    switch(res){
      case 1:
      enleve_occupant(&jeu->plateau[unit->posx][unit->posy].occupant, unit);
      deplacer(unit,jeu);
      ajoute_occupant(&jeu->plateau[unit->posx][unit->posy].occupant,unit);
      break;
      case 2:
      if(jeu->plateau[unit->posx][unit->posy].colonie != NULL) break;
      unit->production = RECOLTE;
      unit->temps = TRECOLTE;
      unit->toursrestant = TRECOLTE;
      break;
      case 3:
      tmp = unit;
      while(tmp->uprec != NULL){
        tmp = tmp->uprec;
      }
      enleve_colonie(&tmp,unit);
      enleve_occupant(&jeu->plateau[unit->posx][unit->posy].occupant, unit);
      free(unit);
      break;
    }
    }while(res < 1 && res > 3);
    break;

    default:
    if(unit->toursrestant > 0){
      sprintf(ann,"L'Unité de type '%c' en case x = %d et y = %d", unit->type, unit->posx, unit->posy);
      sur_annonce(ann);
      sprintf(ann,"Ne peut pas se deplacer attendez encore %d tours", unit->toursrestant);
      sous_annonce(ann,jeu);
      break;
    }
    
    sprintf(ann,"L'Unité de type '%c' en case x = %d et y = %d", unit->type, unit->posx, unit->posy);
    
    annonce(ann,jeu);
    sous_annonce("Choisissez parmis les différentes actions sur la gauche",jeu);
    do{
    res=affiche_choix_action_special();
    posx=unit->posx;
    posy=unit->posy;
    if (res==1) strcmp(action,"se déplace");
    if (res==2) strcmp(action,"passer le tour");
    if (res==3) strcmp(action,"Détruire l'unite");
    retour_au_jeu(jeu);
    switch(res){
      case 1:
      enleve_occupant(&jeu->plateau[unit->posx][unit->posy].occupant, unit);
      deplacer(unit,jeu);
      ajoute_occupant(&jeu->plateau[unit->posx][unit->posy].occupant,unit);
      break;
      case 2:
      break;
      case 3:
      tmp = unit;
      while(tmp->uprec != NULL){
        tmp = tmp->uprec;
      }
      enleve_colonie(&tmp,unit);
      enleve_occupant(&jeu->plateau[unit->posx][unit->posy].occupant, unit);
      free(unit);
      break;
    }
    }while(res < 1 && res > 3);
    break;
  }
}

void tour(Grille* jeu,char beginner){
  Unite *tmp,*tmp2,*tmp3;
  char ann[255];
  if(beginner == ABEILLE){
    tmp = jeu->abeille;
    while(tmp != NULL){
      tmp2 = tmp;
      while(tmp2 !=NULL){
        tmp3 = tmp2->usuiv;
        grille(1248,972,*jeu);
        sprintf(ann,"Les Abeilles ont %d pollen.",jeu->ressourcesAbeille);
        annonce(ann,jeu);
        question(tmp2,jeu);
        tmp2 = tmp3; 
      }
        tmp = tmp->colsuiv;
    }
    tmp = jeu->frelon;
    while(tmp != NULL){
      tmp2 = tmp;
      while(tmp2 !=NULL){
        tmp3 = tmp2->usuiv;
        grille(1248,972,*jeu);
        sprintf(ann,"Les Frelons ont %d ressources.",jeu->ressourcesFrelon);
        annonce(ann,jeu);
        question(tmp2,jeu);
        tmp2 = tmp3;
      }
      tmp = tmp->colsuiv;
    }
  }
  else{
    tmp = jeu->frelon;
    while(tmp != NULL){
      tmp2 = tmp;
      while(tmp2 !=NULL){
        tmp3 = tmp2->usuiv;
        grille(1248,972,*jeu);
        sprintf(ann,"Les Frelons ont %d ressources.",jeu->ressourcesFrelon);
        annonce(ann,jeu);
        question(tmp2,jeu);
        tmp2 = tmp3;
      }
      tmp = tmp->colsuiv;
    }
    tmp = jeu->abeille;
    while(tmp != NULL){
      tmp2 = tmp;
      while(tmp2 !=NULL){
        tmp3 = tmp2->usuiv;
        grille(1248,972,*jeu);
        sprintf(ann,"Les Abeilles ont %d pollen.",jeu->ressourcesAbeille);
        annonce(ann,jeu);
        question(tmp2,jeu);
        
        tmp2 = tmp3;
          
        }
        tmp = tmp->colsuiv;
    }
  }
  
}

void combat_final(Grille* jeu){
  Point P = verif_combat(*jeu);
  Unite* frelon,*col_frelon,*col_abeille,*abeille;
  int res_combat = 0;
  char ann[255];
  while(P.x != -1){
      frelon = jeu->plateau[P.x][P.y].occupant;
      col_frelon = jeu->plateau[P.x][P.y].occupant;
      col_abeille = jeu->plateau[P.x][P.y].occupant;
      abeille = jeu->plateau[P.x][P.y].occupant;
      while(frelon->vsuiv != NULL) frelon = frelon->vsuiv;
      while(col_abeille->uprec != NULL) col_abeille = col_abeille->uprec;
      while(col_frelon->uprec != NULL) col_frelon = col_frelon->uprec;
      if(abeille->camp == ABEILLE && frelon->camp == FRELONCAMP){
        do{
          res_combat = combat(abeille->type,frelon->type);
        }while(res_combat == 0);

        if (res_combat > 0){//abeille gagne
            strcmp(action,"combat");
            posx=P.x;
            posy=P.y;
            sprintf(ann,"L'unité %c a gagner contre l'unite %c",abeille->type,frelon->type);
            annonce(ann,jeu);
            retour_au_jeu(jeu);
            enleve_colonie(&col_frelon,frelon);
            enleve_occupant(&jeu->plateau[P.x][P.y].occupant,frelon);
            free(frelon);
            
          } 
        else {//frelon gagne
          strcmp(action,"combat");
          posx=P.x;
          posy=P.y;
          sprintf(ann,"L'unité %c a gagner contre l'unite %c",frelon->type,abeille->type);
          annonce(ann,jeu);
          retour_au_jeu(jeu);
          switch(abeille->type){
            case REINE:
            jeu->ressourcesFrelon += CREINEA;
            break;
            case GUERRIERE:
            jeu->ressourcesFrelon += CGUERRIERE;
            break;
            case OUVRIERE:
            jeu->ressourcesFrelon += COUVRIERE;
            break;
            case ESCADRON:
            jeu->ressourcesFrelon += CESCADRON;
            break;
          }
          enleve_colonie(&col_abeille,abeille);
          enleve_occupant(&jeu->plateau[P.x][P.y].occupant,abeille);
          free(abeille);
        }
      }

      else if(jeu->plateau[P.x][P.y].colonie->type == RUCHE && frelon->camp == FRELONCAMP){
        sprintf(ann,"L'unité %c a gagner contre l'unite 'R'",frelon->type);
        annonce(ann,jeu);
        abeille = jeu->plateau[P.x][P.y].colonie->usuiv;
        if(jeu->plateau[P.x][P.y].colonie->colprec == NULL && jeu->plateau[P.x][P.y].colonie->colsuiv == NULL){
          jeu->abeille = NULL;
          }
        else if(jeu->plateau[P.x][P.y].colonie->colprec == NULL){
            jeu->abeille = jeu->plateau[P.x][P.y].colonie->colsuiv;
          }
        else if(jeu->plateau[P.x][P.y].colonie->colsuiv == NULL){
          jeu->plateau[P.x][P.y].colonie->colprec->colsuiv = NULL;
        }
        else{
          jeu->plateau[P.x][P.y].colonie->colprec->colsuiv = jeu->plateau[P.x][P.y].colonie->colsuiv;
          jeu->plateau[P.x][P.y].colonie->colsuiv->colprec = jeu->plateau[P.x][P.y].colonie->colprec;
        }
        abeille =jeu->plateau[P.x][P.y].colonie->usuiv;
        if(abeille != NULL){
          Unite *tmp;
          while (abeille != NULL)
            {
              tmp = abeille;
              while(tmp->vprec != NULL) tmp = tmp->vprec;
              if(abeille->usuiv == NULL){
                enleve_occupant(&tmp,abeille);
                free(abeille);
                jeu->plateau[P.x][P.y].colonie = NULL;
                abeille = NULL;
              }
              else{
                enleve_occupant(&tmp,abeille);
                abeille = abeille->usuiv;
                free(abeille->uprec);
              }
            }
        }
        else free(jeu->plateau[P.x][P.y].colonie);
        Unite* N = creer_frelon(NID,P.x,P.y,' ',1);
        ajoute_colonie(jeu,N,P.x,P.y);
        enleve_colonie(&col_frelon,frelon);
        ajoute_unite(&N,frelon);
      }
      else{
        sprintf(ann,"L'unité %c a gagner contre l'unite 'N'",abeille->type);
        annonce(ann,jeu);
        frelon = jeu->plateau[P.x][P.y].colonie->usuiv;
        if(frelon->uprec->colprec == NULL){
          jeu->frelon = frelon->uprec->colsuiv;
        }
        free(frelon->uprec);
        Unite *tmp;
        while (frelon != NULL)
          {
            tmp = frelon;
            while(tmp->vprec != NULL) tmp = tmp->vprec;
            if(frelon->usuiv == NULL){
              enleve_occupant(&tmp,frelon);
              free(frelon);
              jeu->plateau[P.x][P.y].colonie = NULL;
              frelon = NULL;
            }
            else{
              enleve_occupant(&tmp,frelon);
              frelon = frelon->usuiv;
              free(frelon->uprec);
            }
          }
      }
      P = verif_combat(*jeu);
    }
}


void charger_partie(Grille *jeu, char* save,char* joueur_actuelle) {
    FILE *fichier;
    fichier = fopen(save, "r"); 
    char camp,type,production;
    int posx,posy,toursrestant;
    Unite *tmp,*c;
    if (fichier == NULL) {
        printf("Aucune sauvegarde trouvée.\nNous commencont une nouvelle partie.\n");
    } else {    
        fscanf(fichier, "%c %d %d", &(*joueur_actuelle), &jeu->ressourcesAbeille, &jeu->ressourcesFrelon);
        while (fscanf(fichier, " %c %c %d %d %c %d", &camp,&type,&posx,&posy,&production,&toursrestant) >= 5) {
          if(camp == ABEILLE){
            if(type == RUCHE){
              tmp = creer_abeille(type,posx,posy,' ',toursrestant);
              ajoute_colonie(jeu,tmp,posx,posy);
              }
            else{
              c = creer_abeille(type,posx,posy,' ',toursrestant);
              ajoute_unite(&tmp,c);
              ajoute_occupant(&jeu->plateau[posx][posy].occupant,c);
            }
          } 
          else{
            if(type == NID){
              tmp = creer_frelon(type,posx,posy,' ',toursrestant);
              if(jeu->frelon == NULL) jeu->abeille = tmp;
              ajoute_colonie(jeu,tmp,posx,posy);
            }
            else{
              c = creer_frelon(type,posx,posy,' ',toursrestant);
              ajoute_unite(&tmp,c);
              ajoute_occupant(&jeu->plateau[posx][posy].occupant,c);
            }
          }
        }

        fclose(fichier); 
    }
}
    
void sauvegarder_partie(Grille *jeu,char joueur_actuelle, char* save) {
    FILE *fichier;
    Unite* tmp,*tmp2;
    fichier = fopen(save,"w");
    char ann[255];
    if (fichier == NULL) {
        sprintf(ann,"Erreur lors de l'ouverture du fichier %s de sauvegarde.",save);
        sur_annonce(ann);
        sous_annonce("Cette partie à été annulée et pas sauvegarder",jeu);
    }
    else {
        if(joueur_actuelle == ABEILLE){
        fprintf(fichier, "%c %d %d\n", joueur_actuelle, jeu->ressourcesAbeille, jeu->ressourcesFrelon);
      }
      else fprintf(fichier, "%c %d %d\n", joueur_actuelle, jeu->ressourcesFrelon, jeu->ressourcesAbeille);
      tmp = jeu->frelon;
      while(tmp != NULL){
        tmp2 = tmp;
        while(tmp2 !=NULL){
          fprintf(fichier, "%c %c %d %d %c %d\n", tmp2->camp,tmp2->type,tmp2->posx,tmp2->posy,tmp2->production,tmp2->toursrestant);
          tmp2 = tmp2->usuiv;
        }
        tmp = tmp->colsuiv;
    }
    fclose(fichier); 
    annonce("fichier bien enregistré",jeu);
  }
}

int main(){
  srand(time(NULL));
  char ann[255];
  Grille jeu;
  Point P;
  int sauvegarde = 0;
  int res_combat = 0 , sauve2 = 1;
  Unite *tmp,*tmp2;
  jeu.abeille = NULL;
  jeu.frelon = NULL;
  
  char nom_save[20];
  char beginner, save,sauve,charge;
 


  init_tab(jeu.plateau);

  MLV_create_window("Kingdom","Kingdom",1920,1080);
  
 
  do{

  annonce("Voulez vous chargez une partie existante?(o or n)\n",&jeu);

  charge = affiche_choix_oui_non();

  }while(charge != 'o' && charge != 'n');

  if( charge == 'o'){
    printf("Mettez le nom de la partie\n");
    scanf(" %s",nom_save);
    charger_partie(&jeu,nom_save,&beginner);
  }
  else{
    if(rand()%2 == 0)
      beginner = ABEILLE;
    else beginner = FRELONCAMP;
      sprintf(ann,"C'est le joueur %c qui commence.\n",beginner);
      annonce(ann,&jeu);
  }


  if(jeu.abeille == NULL && jeu.frelon == NULL){
    do{
      annonce("Voulez vous sauvegardez ?\n",&jeu);
      save = affiche_choix_oui_non();
    }while(save != 'o' && save != 'n');
    if(save == 'o'){
      printf("Choisissez un nom de sauvegarde.");
      scanf(" %s",nom_save);
      sauvegarde = 1;
    }
    else
      initialisation_jeu(&jeu);
  }
  printf("%c",jeu.plateau[0][0].colonie->type);
  grille(1248,972,jeu);

  while(jeu.abeille != NULL && jeu.frelon != NULL && sauve2 != 0){
    tour(&jeu,beginner);
    combat_final(&jeu);
    tmp = jeu.abeille;
    while(tmp != NULL){
      tmp2 = tmp;
      while(tmp2 !=NULL){
        if(tmp2->toursrestant>0)tmp2->toursrestant--;
        tmp2 = tmp2->usuiv;
        }
        tmp = tmp->colsuiv;
    }
    tmp = jeu.frelon;
    while(tmp != NULL){
      tmp2 = tmp;
      while(tmp2 !=NULL){
        if(tmp2->toursrestant>0)tmp2->toursrestant--;
        tmp2 = tmp2->usuiv;
      }
      tmp = tmp->colsuiv;
    }
    jeu.tour++;
    if(jeu.abeille == NULL)
      annonce("Les Frelons ont gagnés.",&jeu);
    else if(jeu.frelon == NULL)
      annonce("Les Abeilles ont gagnées.",&jeu);
    else {
      if(sauvegarde  == 1){
        do{
          annonce("Voulez vous sauvegardez ?",&jeu);
          sauve=affiche_choix_oui_non();
          if(sauve = 'o' ){
            sauvegarder_partie(&jeu,beginner,nom_save);
            sauve2 = 0;
          }
          else if(sauve = 'n'){
            sauve2 = 1;
          }
          else sauve2 = -1;
        }while(sauve2 < 0);
      }
    }
  }

  return 0;
}
