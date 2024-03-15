#ifndef MenuGen_h
#define MenuGen_h

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  03.02.2016
// Fichier MenuGen.h
// Gestion du menu  du générateur
// Traitement cyclique à 1 ms du Pec12


#include <stdbool.h>
#include <stdint.h>
#include "DefMenuGen.h"

//const char MenuFormes[4][21] = {"  Sinus", "  Triangle", "  DentDeScie", "  Carre"};

#define FREQU_MAX 2000
#define FREQU_MIN 20
#define AMPL_MAX 10000
#define AMPL_MIN 0
#define PAS_AMPL 100
#define OFFSET_MAX 5000
#define OFFSET_MIN -5000
#define PAS_OFFSET 100;


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


void MENU_Initialize(S_ParamGen *pParam);


void MENU_Execute(S_ParamGen *pParam);

void InitAfficheurMenu(S_ParamGen *pParam);
void AfficheMenu(S_ParamGen *pParam);
void CopieStructS_ParamGen(S_ParamGen *pParam1, S_ParamGen *pParam2);

#endif




  
   







