/**
 * \brief Ce code affiche un snake à des coordonnées prérentrées et le fait avancer vers la droite, l'utilisateur utilise ZQSD pour le faire changer de direction, pour arreter le jeu il faut appuyer sur 'a', si le snake touche un mur il meure.
 * 
 * \author Korentin Charreteur 1B1
 */

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

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

/**Taille du terrain de jeu */
#define TAILLE_TERRAIN_X 80
#define TAILLE_TERRAIN_Y 40

/**
 * Bordure du jeu BORDURE_GAUCHE = X et BORDURE_HAUT = Y sont la positions gloabale du terrain de jeu, en modifiant les première coordonée en haut a gauche.
 * Gauche et Haute : definition manuel
 * Droite et Bas : definition auto
 */
#define BORDURE_GAUCHE 1
#define BORDURE_HAUT 1
#define BORDURE_DROITE TAILLE_TERRAIN_X + BORDURE_GAUCHE + 1
#define BORDURE_BAS TAILLE_TERRAIN_Y + BORDURE_HAUT + 1

/**Definition de la position du snake en debut de jeu */
#define POS_SNAKE_X BORDURE_GAUCHE + 40
#define POS_SNAKE_Y BORDURE_HAUT + 20

/**Explication des regle et du jeu */
#define EXPLICATIONS "Pour arreter le serpent appuiyer sur 'a' à tout moment, pour vous deplacez utilisez les touches ZQSD.\nDebut dans : "

/**Differentes pause imposé, comme une pause pour lires les explication ou pour pas que le snake se deplace trop vite */
#define NB_SECONDE_EXPLICATIONS 3
#define PAUSE_BOUCLE 300000
#define PAUSE_1_SECONDE 1000000

/**Definition de constantes pour les differentes parties du corps */
#define TETE_SERPENT 'O'
#define CORP_SERPENT 'X'

/**Charactere qui signifie une mort au toucher pour le serpent et taille de tout les pavés qui apparaitront ainsi que leur nombre */
#define MUR '#'

/**Definition des pavés/obstacles */
#define NOMBRE_PAVE 4
#define TAILLE_X_PAVE 5
#define TAILLE_Y_PAVE 5

/**Definition des differentes directions */
#define HAUT 'z'
#define BAS 's'
#define DROITE 'd'
#define GAUCHE 'q'

/**Ce qui servira a effacer et mettre du vide */
#define VIDE ' '

/**Pour laisser un espace minimum plus ou moins grand devant a l'apparition du serpent ou entre les bordure et les blocs*/
#define ESPACE_DEVANT_APPARITION 3
#define ESPACE_BORDURE 1

/**Calcule du nombre de pavé possible pour en cas de surplus de pavé crée une boucle infini quand toute les valeurs possible auront etait attribuer */
#define NB_PAVE_POSSIBLE (TAILLE_TERRAIN_X*TAILLE_TERRAIN_Y)-TAILLE_SNAKE-ESPACE_DEVANT_APPARITION-(((BORDURE_DROITE-BORDURE_GAUCHE)*ESPACE_BORDURE)*ESPACE_BORDURE)*2-(((BORDURE_BAS-BORDURE_HAUT-ESPACE_BORDURE*2)*ESPACE_BORDURE)*ESPACE_BORDURE)*2


/**Definition des differents types de tableaux utilisé */
typedef int t_positionSnake[DIMENSION][TAILLE_SNAKE];
typedef char t_plateau[BORDURE_DROITE][BORDURE_BAS];

void afficher(int, int, char);
void effacer(int, int);
void dessinerSerpent(t_positionSnake);
void progresser(t_positionSnake, char, bool *);
void initPlateau();
void dessinerPlateau();
void gotoXY(int, int);
int kbhit();
void disableEcho();
void enableEcho();

/**Definition du plateau pour qu'il soit visible et utilisable a toutes les procedures et fonctions */
t_plateau plateau;

int main(){
	t_positionSnake laPosition;
	char toucheTaper, ancienneTouche;
	bool collision = false;
	bool fin = true;

	/**Pour un aleatoir plus aleatoir sinon rand enverais a chaque fois la meme valeur */
	srand(time(NULL));

	printf("%s", EXPLICATIONS);

	for (int i = 0; i < NB_SECONDE_EXPLICATIONS; i++)
	{
		printf("%d ", NB_SECONDE_EXPLICATIONS-i);
		fflush(stdout);
		usleep(PAUSE_1_SECONDE);
	}

	/**Initialisation des premieres valeurs du tableau a double entrées */
	laPosition[X][0] = POS_SNAKE_X;
	laPosition[Y][0] = POS_SNAKE_Y;
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
	 */
	system("clear");
	disableEcho();

	/**Afficher le plateau et initialiser les pavé */
	initPlateau();

	/**Dessine le serpent pour la premiere */
	dessinerSerpent(laPosition);

	/**Condition infini jusqu'a ce qu'il y est une condition qui fasse changer fin en false */
	while (fin)
	{
		usleep(PAUSE_BOUCLE);
		/**Verifie si une touche est appuyer, si oui alors la toucheTaper change */
		if (kbhit())
		{
			/**
			 * Retirer la latence:
			 * Permet d'eviter la latence en prenant toujour le dernier caracter taper que l'utilisateur reste appuyer sur une meme touche longtemps ou en tape une serie.
			 * Cepandant si l'utilisateur compte taper deux direction en meme temps (zq) et attend que le serpent qui aller initialement sur la droite fasse ce qu'il veut il ne le fera pas q etant la direction interdite de la droite
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

		/**Progression, reception de si il y a collision ou non, puis vidage du buffer */
		progresser(laPosition, toucheTaper, &collision);
		fflush(stdout);
		
		
		/**Sauvergarde de la touche/direction utiliser durant cette boucle pour effectuer un teste de direction dans la prochaine boucle */
		ancienneTouche=toucheTaper;

		/**
		 * Teste si la touche tapé est le caractere de sortie si il s'agit de la condition de sortie et si il y a eu une collision:
		 * - Effacer le cmd
		 * - Redonner la capaciter d'afficher ce qui est taper dans le cmd
		 * - Changement de la condition de sortie de la boucle infinie en false
		 * */
		if ((toucheTaper==SORTIE) || collision)
		{
			system("clear");
			enableEcho();
			printf("\n");
			fin = false;
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
	afficher(x, y, VIDE);
}

/**Desiner le snake en utilisant les coordonées present dans le tableau a double entrés*/
void dessinerSerpent(t_positionSnake laPosition){
	int x, y;

	/**Affichage des coordonnée du serpent grace au tableau */
	for (int i = 1; i < TAILLE_SNAKE; i++)
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

/**Faire avancer le snake dans la direction reçu en parametre, tout en verifiant si il y a eu une collision*/
void progresser(t_positionSnake laPosition, char direction, bool *statut){
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
	
	/**Calcule des coordonées x et y pour tout le reste du corp en partant de la fin*/
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
	if ((direction!=SORTIE) && !(*statut))
	{
		dessinerSerpent(laPosition);
	}
	
	/**Verification d'une collision a un mur a l'aide du tableau plateau */
	if (plateau[laPosition[X][0]][laPosition[Y][0]]==MUR)
	{
		*statut=true;
	}

	/**Verification d'une collision a une partie du serpent */
	for (int i = 1; i < TAILLE_SNAKE; i++)
	{
		if ((laPosition[X][0] == laPosition[X][i])
		 && (laPosition[Y][0] == laPosition[Y][i]))
		{
			*statut=true;
		}
	}
}

/**Initialisation du tableau du plateau pour ensuite afficher chaque element tableau */
void initPlateau(){
	int paveX, paveY, nbPaveCree, paveACree;

	if (NOMBRE_PAVE > NB_PAVE_POSSIBLE)
	{
		paveACree = NB_PAVE_POSSIBLE;
	}
	else{
		paveACree = NOMBRE_PAVE;
	}

	int posiPave[DIMENSION][paveACree];

	/**Initialisation des bordures du plateau */
	for (int initX = 1; initX < BORDURE_DROITE; initX++)
	{
		for (int initY = 1; initY < BORDURE_BAS; initY++)
		{
			/**remplissage du tableau par du vide */
			plateau[initX][initY]=VIDE;

			/**Remplacement des bordures par des mur */
			if (((initY==BORDURE_HAUT) && (BORDURE_GAUCHE<=initX) && (initX<=BORDURE_DROITE))
			 || ((initY==BORDURE_BAS-1) && (BORDURE_GAUCHE<=initX) && (initX<=BORDURE_DROITE))
			 || ((initX==BORDURE_GAUCHE) && (BORDURE_HAUT<=initY) && (initY<=BORDURE_BAS))
			 || ((initX==BORDURE_DROITE-1) && (BORDURE_HAUT<=initY) && (initY<=BORDURE_BAS)))
			{
				plateau[initX][initY]=MUR;
			}	
		}
	}

	/**Initialisation du tableau des differentes positions des pavés */
	for (int i = 0; i < paveACree; i++)
	{
		posiPave[X][i] = 0;
		posiPave[Y][i] = 0;
	}

	nbPaveCree=0;

	while (nbPaveCree < paveACree)
	{
		/**Initialisation des differentes valeurs des pavé dans un tableau de maniere a ne pas avoir deux fois la meme valeur */
		for (int contPave = 0; contPave < paveACree; contPave++)
		{
			/**Obtention des valeurs aleatoir -TAILLE_?_PAVE pour laisser la place au serpent de passer entre le pave d'une certaine taille et les bordures*/
			paveX=rand()%(BORDURE_DROITE-TAILLE_X_PAVE-ESPACE_BORDURE);
			paveY=rand()%(BORDURE_BAS-TAILLE_Y_PAVE-ESPACE_BORDURE);

			/**Teste pour verifier si un x et y associer du tableau est egale au x et y associer aleatoir */
			for (int i = 0; i < paveACree; i++)
			{
				if (!((posiPave[X][i]==paveX) && (posiPave[Y][i]==paveY)))
				{
					posiPave[X][contPave] = paveX;
					posiPave[Y][contPave] = paveY;
				}
			}
			
			/**
			 * Verification des coordonée aleatoir des pavés de maniere a :
			 *  laisser un espace au serpent pour passer entre les pavés et les bordures
			 *  et ne pas faire apparaitre les pavés sur le serpent ni un mur directement apres la tete
			 */
			while ((paveX<=(BORDURE_GAUCHE+ESPACE_BORDURE))
				|| (paveY<=(BORDURE_HAUT+ESPACE_BORDURE))
				|| ((((POS_SNAKE_X-TAILLE_SNAKE-TAILLE_X_PAVE)<paveX-1) && (paveX<=(POS_SNAKE_X+ESPACE_DEVANT_APPARITION))) 
				&&  (((POS_SNAKE_Y-TAILLE_Y_PAVE)<paveY) && (paveY<=POS_SNAKE_Y))))
			{
				paveX=rand()%(BORDURE_DROITE-TAILLE_X_PAVE-ESPACE_BORDURE);
				paveY=rand()%(BORDURE_BAS-TAILLE_Y_PAVE-ESPACE_BORDURE);

				/**Teste pour verifier si un x et y associer du tableau est egale au x et y associer aleatoire */
				for (int i = 0; i < paveACree; i++)
				{
					if (!((posiPave[X][i]==paveX) && (posiPave[Y][i]==paveY)))
					{
						posiPave[X][contPave] = paveX;
						posiPave[Y][contPave] = paveY;
						nbPaveCree++;
					}
				}
			}
		}
	}

	/**Intialisation de toutes les valeurs d'un pavé en mur */
	for (int contPave = 0; contPave < paveACree; contPave++)
	{
		/**Initialisation des valeurs a utiliser dans des variable pour ne pas perdre celles du tableau */
		paveX = posiPave[X][contPave];
		paveY = posiPave[Y][contPave];

		/**Creation des pave au coordonées aleatoirs dans le tableau du plateau */
		for (int x = 0; x < TAILLE_X_PAVE; x++)
		{
			for (int y = 0; y < TAILLE_Y_PAVE; y++)
			{
				plateau[paveX][paveY] = MUR;
				paveY++;
			}

			/**paveY ayant etait modifié on lui redonne la valeur qu'il avait du tableau */
			paveY = posiPave[Y][contPave];
			paveX++;
		}
	}
	
	dessinerPlateau();
}

/**Afficher tout les elements du tableau plateau, le vide, les murs ... */
void dessinerPlateau(){
	for (int x = 0; x < BORDURE_DROITE; x++)
	{
		for (int y = 0; y < BORDURE_BAS; y++)
		{
			afficher(x, y, plateau[x][y]);
		}
	}
}

/**Placer le curseur au coordonnées x,y donner en parametre */
void gotoXY(int x, int y){ 
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