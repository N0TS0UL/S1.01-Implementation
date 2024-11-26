/**
 * \brief Ce code affiche un snake à des coordonnées donner par l'utilisateur et le fait avancer. Pour arreter le snake il suffit d'appuyer sur la touche 'a'.
 * 
 * \author Korentin Charreteur 1B1
 */



#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>

/**Constantes servant à faire un tableau à double entré sous la forme {{x1, y1},{x2, y2}...}
 * DIMENSION = 2 car le serpent se deplace dans un plan en deux dimension il n'y a donc que deux coordonées dofferentes a savoir pour situer le seprent
 * laPosition[0][] sera pour l'axe x
 * laPosition[1][] sera pour l'axe y
 * TAILLE_SNAKE lui represente toutes les coordonnées des parties du corps du serpent
*/
#define TAILLE_SNAKE 10
#define DIMENSION 2
/*Touche de sortie */
#define SORTIE 'a'
#define PAUSE 100000

const char TETE_SERPENT ='O';
const char CORP_SERPENT ='X';

void afficher(int, int, char);
void effacer(int, int);
void dessinerSerpent(int laPosition[DIMENSION][TAILLE_SNAKE]);
void progresser(int laPosition[DIMENSION][TAILLE_SNAKE]);
void gotoXY(int, int);
int kbhit(void);

int main(){
	int posX, posY, fin;
	int laPosition[DIMENSION][TAILLE_SNAKE];
	char toucheSortie;
	fin=0;

	printf("Pour arreter le serpent appuiyer sur 'a' à tout moment.\n");
	printf("Selectionnez les coordonées X de votre sperpent entre 1 et 40 : ");
	scanf("%d", &posX);
	while ((1>posX)||(posX>40))
	{
		printf("Erreur valeur x non correcte\n");
		printf("Selectionnez les coordonées X de votre sperpent entre 1 et 40 : ");
		scanf("%d", &posX);
	}

	printf("Selectionnez les coordonées Y de votre sperpent entre 1 et 40 : ");
	scanf("%d", &posY);
	while ((1>posY)||(posY>40))
	{
		printf("Erreur valeur y non correcte\n");
		printf("Selectionnez les coordonées Y de votre sperpent entre 1 et 40 : ");
		scanf("%d", &posY);
	}

	/**Initialisation des premieres valeurs du tableau a double entrés */
	laPosition[0][0] = posX;
	laPosition[1][0] = posY;

	system("clear");
	dessinerSerpent(laPosition);
	while (fin!=1)
	{
		usleep(PAUSE);
		progresser(laPosition);
		
		/**Verification de si une touche est appuyer
		 * Si une touche est appyer alors prendre ça valeur 
		 * Puis teste de la touche pour verifier si il s'agit de la condition de sortie
		*/
		if (kbhit())
		{
			toucheSortie = getchar();
			if (toucheSortie==SORTIE)
			{
				system("clear");
				fin=1;
			}
		}
	}
   return EXIT_SUCCESS;
}

/**Afficher le caractere qu'il ressoit en parametre a la position x,y en y deplaçant le curseur*/
void afficher(int x, int y, char c){
	gotoXY(x, y);
	printf("%c", c);
}

/**Efface le caractere qui se trouve au coordonées x,y donnés en parametre */
void effacer(int x, int y){
	char vide;
	vide=' ';
	afficher(x, y, vide);
}

/**Desiner le snake en utilisant les coordonées present dans le tableau a double entrés*/
void dessinerSerpent(int laPosition[DIMENSION][TAILLE_SNAKE]){
	int x, y;
    /** calcule les cordonée x et y pour la tete puis affichage */
    x = laPosition[0][0];
	y = laPosition[1][0];
    afficher(x, y, TETE_SERPENT);

    /** calcule des cordonées x et y pour tout le reste du corp */
    for (int i = 1; i < TAILLE_SNAKE; i++){
		laPosition[0][i] = (laPosition[0][0]-i);
		laPosition[1][i] = laPosition[1][0];
	}
	/**affichage des coordonnée du serpent grace au tableau */
	for (int i = 1; i < TAILLE_SNAKE; i++)
	{
		if ((laPosition[0][i]>0) && (laPosition[1][i]>0))
		{
			x = laPosition[0][i];
			y = laPosition[1][i];
        	afficher(x, y, CORP_SERPENT);
		}
    }
}

/**Faire avancer le snake sur la droite*/
void progresser(int laPosition[DIMENSION][TAILLE_SNAKE]){
	int suppX, suppY;
	/**Creation des coordonnée a effacer et appelle de la precedure pour les effacer*/
	suppX = laPosition[0][TAILLE_SNAKE-1];
	suppY = laPosition[1][TAILLE_SNAKE-1];
	effacer(suppX, suppY);
	/**Progression da la coordonnée reference du tableau puis transmision du tableau a "dessinerSerpent" */
	laPosition[0][0] = (laPosition[0][0]+1);
	dessinerSerpent(laPosition);
}

/**Placer le curseur au coordonnées x,y donner en parametre */
void gotoXY(int x, int y) { 
    printf("\033[%d;%df", y, x);
}

/**Obtenir les touche que l'utilisateur tape au clavier en boucle sans arreter le code*/
int kbhit(void){
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere present
	
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
