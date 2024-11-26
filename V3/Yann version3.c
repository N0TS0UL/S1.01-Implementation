/**
* \page Général 
* 
* \author TANNEAU YANN
* \version 3.0
* \date 2024
*
* Ce programme est un Snake
* en prototype
*/

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/**Constantes servants à faire un tableau à double entrées sous la forme {[x1, x2, x3,...], [y1, y2, y3,...]}
 * laPosition[0][] sera pour l'axe x
 * laPosition[1][] sera pour l'axe y
*/
#define SNAKE_TAILLE 10
#define SNAKE_DIMENSION 2


#define PAVES_TAILLE 5
#define PAVES_DIMENSION 5

/*Touche de sortie */
#define SORTIE 'a'

/*Touche de deplacement */
#define DEPLACEMENT_HAUT 'z'
#define DEPLACEMENT_BAS 's'
#define DEPLACEMENT_DROIT 'd'
#define DEPLACEMENT_GAUCHE 'q'

/** definition de l'air de jeu */
#define AIRE_DE_JEU_X 80
#define AIRE_DE_JEU_Y 40
#define MUR_SIMBOLE '#'
#define VIDE ' '

// Pause entre deux mouvement en micro secondes
const int PAUSE= 500000;
// Charactère qui représente la tete du serpent
const char TETE_SERPENT ='O';
// Charactère qui représente le corps du serpent
const char CORPS_SERPENT ='X';
// Activation du mode affichage pas à pas
const bool DEBUG = false;

// Position de la tete en X et Y
const int POSITION_INITIAL_X = 40;
const int POSITION_INITIAL_Y = 20;

void disableEcho();
void enableEcho();
void initPalteau(char tableauDeJeu[AIRE_DE_JEU_Y][AIRE_DE_JEU_X]);
void dessinerPlateau(char tableauDeJeu[AIRE_DE_JEU_Y][AIRE_DE_JEU_X]);
void dessinerSerpent(int laPosition[SNAKE_DIMENSION][SNAKE_TAILLE]);
void afficher(int, int, char);
void effacer(int, int);
void progresser(int laPosition[SNAKE_DIMENSION][SNAKE_TAILLE], char, bool* , char tableauDeJeu[AIRE_DE_JEU_Y][AIRE_DE_JEU_X]);
void gotoXY(int, int);
int kbhit(void);

/**
* \fn int main()
* \brief Programme principal.
* \return Code de sortie du programme (0 : sortie normale).
* 
* Le programme principal dessine un serpent en mouvement tant que l'utilisateur 
* n'a pas taper sur la touche de fin.
*/
int main(){
	int laPosition[SNAKE_DIMENSION][SNAKE_TAILLE];
	char tableauDeJeu[AIRE_DE_JEU_Y][AIRE_DE_JEU_X];
	bool finDuJeu = false;
	char direction, directionInterdit;
	char pause;
	char toucheTaper = DEPLACEMENT_DROIT;


	disableEcho();
	initPalteau(tableauDeJeu);
	//initialisation des valeurs du tableau
	laPosition[0][0] = POSITION_INITIAL_X;
	laPosition[1][0] = POSITION_INITIAL_Y;
	for (int i = 1; i < SNAKE_TAILLE; i++){
		laPosition[0][i] = (laPosition[0][0]-i);
		laPosition[1][i] = laPosition[1][0];
	}

	// Affichage du serpent aux coordonnées de départ
	dessinerSerpent(laPosition);

	while (!finDuJeu) {

		if (DEBUG) {
			// Mode debug : on attend que l'utilisateur tape sur une touche ENTREE
			// pour passer à la prochaine étape
			scanf("%c", &pause);
		} else {
			// Pause entre chaque mouvement
			usleep(PAUSE);
		}

		if (toucheTaper!=directionInterdit){
				switch (toucheTaper){
					case DEPLACEMENT_HAUT:
						direction = DEPLACEMENT_HAUT;
						directionInterdit = DEPLACEMENT_BAS;
					break;

					case DEPLACEMENT_BAS:
						direction = DEPLACEMENT_BAS;
						directionInterdit = DEPLACEMENT_HAUT;
						break;

					case DEPLACEMENT_DROIT:
						direction = DEPLACEMENT_DROIT;
						directionInterdit = DEPLACEMENT_GAUCHE;
					break;

						case DEPLACEMENT_GAUCHE:
						direction = DEPLACEMENT_GAUCHE;
						directionInterdit = DEPLACEMENT_DROIT;
						break;

					default:
						break;
				}
		}
		// Calcul les prochaines coordonnées et y affichage le serpent
		progresser(laPosition, direction, &finDuJeu, tableauDeJeu);

		//Vérification q'une touche est appuyée
		if (kbhit()) {
			//Récupère la valeur de la touche appuyée
			toucheTaper = getchar();
			}
			
			//Teste de la touche pour verifier si il s'agit de la condition de sortie
			if (toucheTaper==SORTIE){
				finDuJeu=true;
			}
		
	}
	enableEcho();
	//system("clear");
	gotoXY(1,AIRE_DE_JEU_Y+1);
    return EXIT_SUCCESS;
}

/**********************************************************/
/* Partie 0 parametre et inisalisation */
/**********************************************************/
void disableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}



void enableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void initPalteau(char tableauDeJeu[AIRE_DE_JEU_Y][AIRE_DE_JEU_X]){
	int x, y;

	for (int y = 0; y < AIRE_DE_JEU_Y; y++)
	{
		for (int x = 0; x < AIRE_DE_JEU_X; x++)
		{
			tableauDeJeu[y][x]=VIDE;
		}
		
	}

	y=0;
	for (int i = 0; i < AIRE_DE_JEU_X; i++){
		tableauDeJeu[y][i]= MUR_SIMBOLE;

	}
	y=AIRE_DE_JEU_Y-1;
	for (int i = 0; i < AIRE_DE_JEU_X; i++){
		tableauDeJeu[y][i]= MUR_SIMBOLE;

	}
	x=0;
	for (int i = 0; i < AIRE_DE_JEU_X; i++){
		tableauDeJeu[i][x]= MUR_SIMBOLE;

	}
	x=AIRE_DE_JEU_X-1;
	for (int i = 0; i < AIRE_DE_JEU_X; i++){
		tableauDeJeu[i][x]= MUR_SIMBOLE;

	}
	
	system("clear");
	dessinerPlateau(tableauDeJeu);
}

void dessinerPlateau(char tableauDeJeu[AIRE_DE_JEU_Y][AIRE_DE_JEU_X]){
	for (int y = 0; y < AIRE_DE_JEU_Y; y++)
	{
		for (int x = 0; x < AIRE_DE_JEU_X; x++)
		{
			if(tableauDeJeu[y][x]!=VIDE){
				afficher(x+1, y+1, tableauDeJeu[y][x]);
			}
		}
		
	}
	
}
/**********************************************************/
/* Partie 1 afficher le serpent dans la console */
/**********************************************************/

/**
* \fn void dessinerSerpent(int laPosition[SNAKE_DIMENSION][SNAKE_TAILLE])
* \brief Desisner le snake
* \param laPosition : tableau des cordonée du serpent.
*
* dessine la tete et le corp du serpent grace au cordonnée du tableau laPosition
*/
void dessinerSerpent(int laPosition[SNAKE_DIMENSION][SNAKE_TAILLE]){
	int x, y;

	for (int i = 1; i < SNAKE_TAILLE; i++){
		/**affichage des coordonnée du serpent grace au tableau */
        x = laPosition[0][i];
		y = laPosition[1][i];
        afficher(x, y, CORPS_SERPENT);//affiche le corps
    }

	/** calcule les coordonée x et y pour la tete */
    x = laPosition[0][0];
	y = laPosition[1][0];
	afficher(x, y, TETE_SERPENT);//affiche la tete
	// vide le buffer
	fflush(stdout);
}

/**
* \fn void afficher(int x, int y, char c)
* \brief affiche un caratère
* \param x : absice du terminal
* \param y : ordonnée du terminal
* \param c : caractère qui sera afficher
*
* affiche le caratère c au cordonnée X Y dans le terminal
*/
void afficher(int x, int y, char c){
	//if	(((x>=0)&&(y>=0)&&(x<=AIRE_DE_JEU_FIN_X)&&(y<=AIRE_DE_JEU_FIN_Y))){
	gotoXY(x, y);
	printf("%c", c);
	//}
}

/**
* \fn void effacer(int x, int y)
* \brief efface un caractère
* \param x : absice du terminal
* \param y : ordonnée du terminal
*
* efface le caratère au cordonnée X Y dans le terminal
*/
void effacer(int x, int y){
	afficher(x, y, VIDE);
}

/**
* \fn void gotoXY(int x, int y)
* \brief place le curseur
* \param x : absice du terminal
* \param y : ordonnée du terminal
* 
* place le curseur au cordonnée X Y dans le terminal
*/
void gotoXY(int x, int y){ 
    printf("\033[%d;%df", y, x);
}

/**********************************************************/
/* Partie 2 mettre a jour les données du serpent          */
/**********************************************************/

/**
* \fn void progresser(int laPosition[SNAKE_DIMENSION][SNAKE_TAILLE])
* \brief met a jour les coordonnées du serpent
* \param laPosition : tableau des corrdonnées du serpent.
* 
* met à jour les coordonnées du serpent et l'affiche.
*/
void progresser(int laPosition[SNAKE_DIMENSION][SNAKE_TAILLE], char direction, bool *finDuJeu, char tableauDeJeu[AIRE_DE_JEU_Y][AIRE_DE_JEU_X]){
	int suppX, suppY, deplaX, deplaY, coorX, coorY;
	/**Creation des coordonnées à éffacer et appelle de la procédure pour les effacer*/
	suppX = laPosition[0][SNAKE_TAILLE-1];
	suppY = laPosition[1][SNAKE_TAILLE-1];
	effacer(suppX, suppY);

		switch (direction){

		case DEPLACEMENT_HAUT:
			deplaX=0;
			deplaY=-1;
			break;

		case DEPLACEMENT_BAS:
			deplaX=0;
			deplaY=1;
			break;

		case DEPLACEMENT_DROIT:
			deplaX=1;
			deplaY=0;
			break;

		case DEPLACEMENT_GAUCHE:
			deplaX=-1;
			deplaY=0;
			break;

		default:
			break;
		}
	

	/**Progression puis dessinage */
	for (int i = SNAKE_TAILLE -1; 0 < i ; i--){
		laPosition[0][i] = laPosition[0][i-1];
		laPosition[1][i] = laPosition[1][i-1];
	}
	laPosition[0][0] = (laPosition[0][0]+deplaX);
	laPosition[1][0] = (laPosition[1][0]+deplaY);
	dessinerSerpent(laPosition);

	for (int i = 1; i < SNAKE_TAILLE; i++)
	{
		if ((laPosition[0][0]==laPosition[0][i])&&(laPosition[1][0]==laPosition[1][i]))
		{
			*finDuJeu = true;
		}
		
	}
	coorX=laPosition[0][0];
	coorY=laPosition[1][0];
	if (tableauDeJeu[coorY][coorX]==MUR_SIMBOLE)
	{
		*finDuJeu = true;
	}
	
}

/**********************************************************/
/* Partie 3 verifier si le jeu est fini                   */
/**********************************************************/

/**Obtenir les touches que l'utilisateur tape au clavier en boucle sans arreter le code*/
/**
* \fn int kbhit(void)
* \brief Indique si une touche a été tapée
* \return le caratère tapée
*
* la fonction retourne : 1 si un caractere est present, 0 si pas de caractere present
* et getchar() est le caratère tapée
*/
int kbhit(void){
	
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	} 
	return unCaractere;
}
