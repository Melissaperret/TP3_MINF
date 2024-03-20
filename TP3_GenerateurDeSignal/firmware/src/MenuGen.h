#ifndef MenuGen_h
#define MenuGen_h

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  03.02.2016
// Fichier MenuGen.h
// Gestion du menu  du générateur
// Traitement cyclique à 1 ms du Pec12

// Librairie inclues
#include <stdbool.h>
#include <stdint.h>
#include "DefMenuGen.h"

#define FREQU_MAX 2000  // Fréwuence maximum
#define FREQU_MIN 20    // Fréquence minimum
#define AMPL_MAX 10000  // Amplitude maximum
#define AMPL_MIN 0      // Amplitude minimum
#define PAS_AMPL 100    // Pas d'amplitude
#define OFFSET_MAX 5000 // Offset maximum
#define OFFSET_MIN -5000    // Offset minimum
#define PAS_OFFSET 100; // Pas d'offset

// Énumération pour les état du menu
typedef enum 
{
    SEL_FORME = 0,
    SET_FORME,
    SEL_FREQU,
    SET_FREQU,
    SEL_AMPL,
    SET_AMPL,
    SEL_OFFSET,
    SET_OFFSET,
    SAVE
}MENU_STATE;

// Fonction d'execution du menu, appel cyclique depuis l'application
void MENU_Execute(S_ParamGen *pParam);
// Fonction d'affichage initiale du menu
void MENU_Initialize(S_ParamGen *pParam);
// Fonction d'affichage des valeurs du menu
void AfficheMenu(S_ParamGen *pParam);
// Fonction de gestion des états de setting des données
MENU_STATE GestSettingMenu(MENU_STATE menuStat, S_ParamGen *tempData, S_ParamGen *pParam);

#endif