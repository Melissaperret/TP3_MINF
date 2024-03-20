#ifndef Generateur_h
#define Generateur_h

// TP3 MenuGen 2016
// C. HUBER  03.02.2016
// Fichier Generateur.h
// Prototypes des fonctions du générateur  de signal

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "DefMenuGen.h"

//Constantes
#define MAX_ECH 100 // Nombres d'échantillon
#define VAL_MAX_PAS 65535   // Nombre de pas maximum de convertion
#define VAL_DIVISION_TENSION 5000   // Moitier de l'amplitude maximum
#define TRANSFORMATION_VALEUR_TIMER3 800000 // Valeur de fréquence du timer 3
#define MAX_AMPLITUDE 10000 // Amplitude maximum

//extern const uint16_t tableauCourbeSinus[100]; //A ne pas déclarer dans le .h pour ce projet car seulement utiliser dans Generateur.c et pas différents fichiers. 

// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam);

// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam);

// Mise à jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam);

// Execution du générateur en envoient les valeurs calculées au dac
void  GENSIG_Execute(void);


#endif