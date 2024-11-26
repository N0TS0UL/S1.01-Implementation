/**
 * \brief Ce code affiche un snake à des coordonnées prérentrées et le fait avancer vers la droite, l'utilisateur utilise ZQSD pour le faire changer de direction, pour arreter le jeu il faut appuyer sur 'a'.
 * 
 * \author Korentin Charreteur 1B1
 */

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Constantes servant à faire un tableau à double entrées sous la forme {{x1, x2, x3,...},{y1, y2, y3,...}}
 * DIMENSION = 2 car le serpent se deplace dans un plan en deux dimension il n'y a donc que deux coordonées differentes à savoir pour situer le seprent
 * Initialisation de constante X et Y pour facilité la lecture :
 * laPosition[0][] sera pour l'axe x -> laPosition[X][]
 * laPosition[1][] sera pour l'axe y -> laPosition[Y][]
 * TAILLE_SNAKE lui est la taille des deux tableaux constituant le tableau à double entrées et represente les coordonnées des parties du corps du serpent
*/
#define TAILLE_SNAKE 10
#define DIMENSION 2
#define X 0
#define Y 1
/**Touche de sortie */
#define SORTIE 'a'
/**Differentes pause imposé, comme une pause pour lires les explication ou pour pas que le snake se deplace trop vite */
#define PAUSE_EXPLICATIONS 3000000
#define PAUSE_BOUCLE 90000
/**Definition de la position du snake en debut de jeu ici x = 20, y = 20 */
#define POS_X 20
#define POS_Y 20
/**Definition de constantes pour les differentes parties du corps */
#define TETE_SERPENT 'O'
#define CORP_SERPENT 'X'
/**Definition des differentes directions */
#define HAUT 'z'
#define BAS 's'
#define DROITE 'd'
#define GAUCHE 'q'
/**Bordure du jeu */
#define BORDURE_DROITE 100
#define BORDURE_GAUCHE 0
#define BORDURE_HAUT 0
#define BORDURE_BAS 30

void afficher(int, int, char);
void effacer(int, int);
void dessinerSerpent(int laPosition[DIMENSION][TAILLE_SNAKE]);
void progresser(int laPosition[DIMENSION][TAILLE_SNAKE], char);
void gotoXY(int, int);
int kbhit();
void disableEcho();
void enableEcho();

int main(){
	int laPosition[DIMENSION][TAILLE_SNAKE];
	char toucheTaper, ancienneTouche;

	printf("Pour arreter le serpent appuiyer sur 'a' à tout moment, pour vous deplacez utilisez les touches ZQSD.\n");
	usleep(PAUSE_EXPLICATIONS);

	/**Initialisation des premieres valeurs du tableau a double entrées */
	laPosition[X][0] = POS_X;
	laPosition[Y][0] = POS_Y;
	/**
	 * Calcule des premieres coordonnée qui servirons a afficher le serpent en entier dé le debut.
	 * Si l'on veut que le reste du corps du serpent sorte de la partie precedente (comme si il etait recrocviller sur une case) il faut enlever cette partie 
	 */
	for (int i = 1; i < TAILLE_SNAKE; i++)
	{
		laPosition[X][i] = (laPosition[X][0]-i);
		laPosition[Y][i] = laPosition[Y][0];
	}

	/**
	 * Le serpent se deplace dabord vers la droite sans action de la part de l'utilisateur le serpent continura vers la droite
	 * Si l'utilisateur decide de taper une ou plusieurs direction/caracteres avant la fin des explications le serpent ira quand meme vers la droite.
	 */
	while (kbhit())
	{
		getchar();
	}
	toucheTaper=DROITE;
	ancienneTouche=toucheTaper;

	/**
	 * Efface le terminal
	 * Desactive l'écrture des touches tapées au clavier sans en empécher la lecture
	 * Dessine le serpent pour la premiere
	 */
	system("clear");
	disableEcho();
	dessinerSerpent(laPosition);
	while (toucheTaper!=SORTIE)
	{
		usleep(PAUSE_BOUCLE);
		/**Verifie si une touche est appuyer, si oui alors la toucheTaper change */
		if (kbhit())
		{
			/**
			 * Retirer la latence:
			 * Permet d'eviter la latence en prenant toujour le dernier caracter taper que l'utilisateur reste appuyer sur une meme touche longtemps ou en tape une serie.
			 * Cepandant si l'utilisateur compte taper deux direction en meme temps (zq) et attand que le serpent qui aller initialement sur la droite fasse ce qu'il veut il ne le fera pas q etant la direction interdite de la droite
			while (kbhit())
			{
				toucheTaper = getchar();
			}
			*/

			/**Recupere le 1er caractere taper, ex : 'z''q''s''d' la touche taper sera 'z' il restera 'q''s''d' pour la prochaine boucle si l'utilisateur ne tape rien, si il saisie quelque chose les caracteres vont s'ajouter et crée une latance entre sa volonter et le programme */
			toucheTaper = getchar();

			/**
			 * Verification de si l'utilisateur essaye de faire le serpent se retourné sur lui meme ou tape une touche differente que les touches definits.
			 * Si le teste pour verifier si la touche est definit n'est pas mit en place alors la progression n'aurra aucune direction et le serpent s'arretera
			 */
			if (((ancienneTouche==DROITE) && (toucheTaper==GAUCHE)) 
			|| 	((ancienneTouche==GAUCHE) && (toucheTaper==DROITE))
			||	((ancienneTouche==HAUT) && (toucheTaper==BAS))
			||	((ancienneTouche==BAS) && (toucheTaper==HAUT))
			||  ((toucheTaper!=DROITE)
			&&   (toucheTaper!=GAUCHE)
			&&   (toucheTaper!=HAUT)
			&&   (toucheTaper!=BAS)
			&&	 (toucheTaper!=SORTIE)))
			{
				toucheTaper=ancienneTouche;
			}
		}

		/**Progression  puis vidage du buffer */
		progresser(laPosition, toucheTaper);
		fflush(stdout);
		
		/**Sauvergarde de la touche/direction utiliser durant cette boucle pour effectuer un teste de direction dans la prochaine boucle */
		ancienneTouche=toucheTaper;

		/**
		 * Teste si la touche tapé est le caractere de sortie si il s'agit de la condition de sortie :
		 * - Effacer le cmd
		 * - Redonner la capaciter d'afficher ce qui est taper dans le cmd
		 * */
		if (toucheTaper==SORTIE)
		{
			system("clear");
			enableEcho();
			printf("\n");
		}
	}
   return EXIT_SUCCESS;
}

/**Affiche le caractere qu'il ressoit en parametre a la position x,y en y deplaçant le curseur*/
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

	/**Affichage des coordonnée du serpent grace au tableau */
	for (int i = 1; i < TAILLE_SNAKE-1; i++)
	{
		/**Teste pour Afficher les parties de serpent que si elles sont compris dans les bordures */
		if ((laPosition[X][i]>BORDURE_GAUCHE) 
		 && (laPosition[Y][i]>BORDURE_HAUT)
		 && (laPosition[X][i]<BORDURE_DROITE)
		 && (laPosition[Y][i]<BORDURE_BAS))
		{
			x = laPosition[X][i];
			y = laPosition[Y][i];
			afficher(x, y, CORP_SERPENT);
		}
    }

	/**Affichage de la tete aux premieres coordonées du tableau si elle dans les bordures */
    if ((laPosition[X][0]>BORDURE_GAUCHE) 
	 && (laPosition[Y][0]>BORDURE_HAUT)
	 && (laPosition[X][0]<BORDURE_DROITE)
	 && (laPosition[Y][0]<BORDURE_BAS))
	{
		x = laPosition[X][0];
		y = laPosition[Y][0];
		afficher(x, y, TETE_SERPENT);
	}
}

/**Faire avancer le snake dans la direction reçu en parametre */
void progresser(int laPosition[DIMENSION][TAILLE_SNAKE], char direction){
	int suppX, suppY;
	/**Verifie si la patie a supprimer est encore visible pour ne pas faire bouger le curseur inutilement et effacer des choses non voulu */
	if ((laPosition[X][TAILLE_SNAKE-1]>BORDURE_GAUCHE) 
	 && (laPosition[Y][TAILLE_SNAKE-1]>BORDURE_HAUT)
	 && (laPosition[X][TAILLE_SNAKE-1]<BORDURE_DROITE)
	 && (laPosition[Y][TAILLE_SNAKE-1]<BORDURE_BAS))
	{
		/**Creation des coordonnée a effacer et appelle de la precedure pour les effacer*/
		suppX = laPosition[X][TAILLE_SNAKE-1];
		suppY = laPosition[Y][TAILLE_SNAKE-1];
		effacer(suppX, suppY);
	}
	
	/**Calcule des cordonées x et y pour tout le reste du corp en partant de la fin*/
	for (int i = 1; i < TAILLE_SNAKE; i++)
	{
		laPosition[X][(TAILLE_SNAKE-i)] = laPosition[X][(TAILLE_SNAKE-i-1)];
		laPosition[Y][(TAILLE_SNAKE-i)] = laPosition[Y][(TAILLE_SNAKE-i-1)];
	}

	/**Progression da la coordonnée reference du tableau suivant la direction donnée puis transmision du tableau a "dessinerSerpent" */
	if (direction==DROITE)
	{
		laPosition[X][0] = (laPosition[X][0]+1);
	}
	else if (direction==GAUCHE)
	{
		laPosition[X][0] = (laPosition[X][0]-1);
	}
	else if (direction==HAUT)
	{
		laPosition[Y][0] = (laPosition[Y][0]-1);
	}
	else if (direction==BAS)
	{
		laPosition[Y][0] = (laPosition[Y][0]+1);
	}

	/**Verification de si la touche de direction est differente de celles de SORTIE pour ne pas afficher le serpent une fois de trop */
	if (direction!=SORTIE)
	{
		dessinerSerpent(laPosition);
	}
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