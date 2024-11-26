#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>

#define TAILLE_SERPENT 1 //taill du serpent
#define SORTIE = 'a' //touche pour mettre fin au jeu

    const int TIME = 1;

    typedef int TabSerpent[TAILLE_SERPENT];

void dessinerSerpent(int [], int []);
void afficher(int, int, char);
void effacer(int, int);
void gotoXY(int, int);
void progresser(int [], int []);
//bool testJeuFini(int [], int [], bool);

int main(){
    TabSerpent lesX, lesY;
    int x, y;
    //bool jeu_fini;
    //jeu_fini = 0;

    system("clear");// efface le terminal

    /** demande les corrdoner x et y pour la tete du serpent */
    printf("entrer valeur x");
    scanf("%d", &x);
    while((x<1)||(x>40)){
        printf("valeur incorect\n");
        printf("entrer valeur x\n");
        scanf("%d", &x);
    }
    printf("entrer valeur Y");
    scanf("%d", &y);
    while((y<1)||(y>40)){
        printf("valeur incorect\n");
        printf("entrer valeur y\n");
        scanf("%d", &y);
    }
    system("clear");// efface le terminal

    /** inisalise les valeur des tableau x et y*/
    lesX[0]= x;
    lesY[0]= y;
    //printf("x = %d, i = 0 y = %d\n", x,y);
    for (size_t i = 1; i < TAILLE_SERPENT; i++){
        lesX[i]= lesX[0]-i;
        lesY[i]= lesY[0];
        //printf("x = %d, i = %ld, y = %d\n", lesX[i],i,lesY[i]);
    }
    //system("clear");// efface le terminal


        for (size_t i = 0; i < TAILLE_SERPENT; i++){
        //printf("x = %d, i = %ld, y = %d\n", lesX[i],i,lesY[i]);
        for (size_t i = 0; i < 10; i++)
        {
            dessinerSerpent(lesX, lesY);
            //sleep(TIME);
            progresser(lesX,lesY);
        }
        
        dessinerSerpent(lesX, lesY);
    }
        
        printf("\n\n");
}

/**********************************************************/
/* Partie 1 afficher le serpent dans la console */
/**********************************************************/

/** dessin le serpent grace au tableur des x et y */
void dessinerSerpent(int lesX[], int lesY[]){
    const char TETE_SERPENT ='O';
    const char CORP_SERPENT ='X';
    int x, y;

//printf("E x = %d, y = %d\n", x,y);
    /** calcule les cordonée x y pour la tete */
    x= lesX[0];
    y= lesY[0];
    afficher( x, y, TETE_SERPENT);
//printf("T x = %d, y = %d\n", x,y);
    

    /** calcule les cordonée x y pour tout le reste du corp */
    for (size_t i = 1; i < TAILLE_SERPENT; i++){
        x= lesX[i];
        y= lesY[i];
        afficher( x, y, CORP_SERPENT);
        //printf("C x = %d, i= %ld, y = %d\n", x,i,y);
    }
    
    
}

/** affiche un caractère au corrdoner x y*/
void afficher(int x, int y, char c){
    gotoXY( x, y);
    printf("%c", c);
}



/** aplique le curseur au cordoner x y */
void gotoXY(int x, int y){ 
    printf("\033[%d;%df", y, x);
}


/**********************************************************/
/* Partie 2 mettre a jour les donnée du serpent */
/**********************************************************/


/* calcule les prochaine cordonnée du serpent*/
void progresser(int lesX[], int lesY[]){
    
    for (size_t i = TAILLE_SERPENT-1; i >1 ; i--){
        effacer(lesX[i],lesY[i]);
        lesX[i] = lesX[i-1];

        //printf(" x = %d, i= %ld, y = %d\n", lesX[i],i,lesY[i]);
    }
    effacer(lesX[0],lesY[0]);
    lesX[0] = lesX[0] +1;
    //printf(" x = %d,  y = %d\n", lesX[0],lesY[0]);
}

/** effacer le bout du serpent */
void effacer(int x, int y){
    const char VIDE =' ';
    gotoXY(x, y);
    printf("%c", VIDE);
}

/**********************************************************/
/* Partie 3 verifier si le jeu est fini */
/**********************************************************/

//bool testJeuFini(int lesX[], int lesY[], bool fin_jeu){
    
  //  return fin_jeu;
//}