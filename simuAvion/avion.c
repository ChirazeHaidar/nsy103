#include <math.h>

#include "avion.h"

// caract�ristiques du d�placement de l'avion
struct deplacement dep;

// coordonn�es spatiales de l'avion
struct coordonnees coord;

// num�ro de vol de l'avion : code sur 5 caract�res
char numero_vol[6];

/********************************
 ***  3 fonctions � impl�menter
 ********************************/

int ouvrir_communication()
{
  // fonction � impl�menter qui permet d'entrer en communication via TCP
  // avec le gestionnaire de vols
  return 1;
}

void fermer_communication()
{ 
  // fonction � impl�menter qui permet de fermer la communication
  // avec le gestionnaire de vols
}
	
void envoyer_caracteristiques()
{
  // fonction � impl�menter qui envoie l'ensemble des caract�ristiques
  // courantes de l'avion au gestionnaire de vols
}

/********************************
 ***  Fonctions g�rant le d�placement de l'avion : ne pas modifier
 ********************************/

// initialise al�atoirement les param�tres initiaux de l'avion
void initialiser_avion()
{
  // initialisation al�atoire du compteur al�atoire
  int seed;
  time(&seed);
  srandom(seed);

  // intialisation des param�tres de l'avion
  coord.x = 1000 + random() % 1000;
  coord.y = 1000 + random() % 1000;
  coord.altitude = 900 + random() % 100;
  
  dep.cap = random() % 360;
  dep.vitesse = 600 + random() % 200;
  
  // initialisation du numero de l'avion : chaine de 5 caract�res 
  // form�e de 2 lettres puis 3 chiffres
  numero_vol[0] = (random() % 26) + 'A';
  numero_vol[1] = (random() % 26) + 'A';
  sprintf (&numero_vol[2], "%03d", (random() % 999) + 1);
  numero_vol[5] = 0;
}

// modifie la valeur de l'avion avec la valeur pass�e en param�tre
void changer_vitesse(int vitesse)
{
  if (vitesse < 0)
    dep.vitesse = 0;
  else if (vitesse > VITMAX) 
    dep.vitesse = VITMAX;
  else dep.vitesse = vitesse;
}

// modifie le cap de l'avion avec la valeur pass�e en param�tre
void changer_cap(int cap)
{
  if ((cap >= 0) && (cap < 360))
    dep.cap = cap;
}

// modifie l'altitude de l'avion avec la valeur pass�e en param�tre
void changer_altitude(int alt)
{
  if (alt < 0) 
    coord.altitude = 0;
  else if (alt > ALTMAX) 
    coord.altitude = ALTMAX;
  else coord.altitude = alt;
}

// affiche les caract�ristiques courantes de l'avion
void afficher_donnees()
{
  printf("Avion %s -> localisation : (%d,%d), altitude : %d, vitesse : %d, cap : %d\n",
	 numero_vol, coord.x, coord.y, coord.altitude, dep.vitesse, dep.cap);
}

// recalcule la localisation de l'avion en fonction de sa vitesse et de son cap
void calcul_deplacement()
{
  float cosinus, sinus;
  float dep_x, dep_y;
  int nb;
  
  if (dep.vitesse < VITMIN)
    {
      printf("Vitesse trop faible : crash de l'avion\n");
      fermer_communication();
      exit(2);
    }
  if (coord.altitude == 0)
    {
      printf("L'avion s'est ecrase au sol\n");
      fermer_communication();
      exit(3);
    }
  
  cosinus = cos(dep.cap * 2 * M_PI / 360);
  sinus = sin(dep.cap * 2 * M_PI / 360);
  
  dep_x = cos(dep.cap * 2 * M_PI / 360) * dep.vitesse * 10 / VITMIN;
  dep_y = sin(dep.cap * 2 * M_PI / 360) * dep.vitesse * 10 / VITMIN;

  // on se d�place d'au moins une case quels que soient le cap et la vitesse
  // sauf si cap est un des angles droit
  if ((dep_x > 0) && (dep_x < 1)) dep_x = 1;
  if ((dep_x < 0) && (dep_x > -1)) dep_x = -1;
  
  if ((dep_y > 0) && (dep_y < 1)) dep_y = 1;
  if ((dep_y < 0) && (dep_y > -1)) dep_y = -1;
  
  //printf(" x : %f y : %f\n", dep_x, dep_y);
  
  coord.x = coord.x + (int)dep_x;
  coord.y = coord.y + (int)dep_y;
  
  afficher_donnees();
}

// fonction principale : g�re l'ex�cution de l'avion au fil du temps
void se_deplacer()
{
  while(1)
    {
      sleep(PAUSE);
      calcul_deplacement();
      envoyer_caracteristiques();
    }
}

int main()
{  
  // on initialise l'avion
  initialiser_avion();  

  afficher_donnees();
  // on quitte si on arrive � pas contacter le gestionnaire de vols
  if (!ouvrir_communication())
    {
      printf("Impossible de contacter le gestionnaire de vols\n");
      exit(1);
    }

  // on se d�place une fois toutes les initialisations faites
  se_deplacer();   
}
  
