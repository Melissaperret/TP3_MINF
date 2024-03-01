// Canevas manipulation GenSig avec menu
// P.Mélissa  09/02/2015
// Fichier Generateur.C
// Gestion  du générateur

// Prévu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber


#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "system_config.h"


// T.P. 2016 100 echantillons
#define MAX_ECH 100
#define VAL_MAX_PAS 65535
#define VAL_MID_PAS 32767


const uint16_t tableauCourbeSinus[100] = {
 53, 56, 59, 62, 65, 68, 71, 74, 77, 79,
 82, 84, 86, 89, 90, 92, 94, 95, 96, 98,
 98, 99, 100, 100, 100, 100, 100, 99, 98,
 98, 96, 95, 94, 92, 90, 89, 86, 84, 82,
 79, 77, 74, 71, 68, 65, 62, 59, 56, 53,
 50, 47, 44, 41, 38, 35, 32, 29, 26, 23,
 21, 18, 16, 14, 11, 10, 8, 6, 5, 4, 2,
 2, 1, 0, 0, 0, 0, 0, 1, 2, 2, 4, 5, 6,
 8, 10, 11, 14, 16, 18, 21, 23, 26, 29,};

S_ParamGen valeursParamGen;  //Structure intermédiaire 

uint16_t tableauValeursSignal[MAX_ECH];

// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{
    //Récupération des données sauvegardées au démarrage précédant
    //On fait un pointeur de 32bits 
    NVM_ReadBlock((uint32_t*)&valeursParamGen, sizeof(S_ParamGen));
    
    //Test pour vérifier si MaGIC = MAGIC 
    if(valeursParamGen.Magic == MAGIC)
    {
        *pParam = valeursParamGen;
    }
    else 
    {
        lcd_gotoxy(1,4);
        printf_lcd("Donnees par defaut");
        
        pParam->Amplitude = 0;    
        pParam->Forme = SignalCarre; 
        pParam->Frequence = 20;
        pParam->Magic = MAGIC;
        pParam->Offset = 0;
    }
        GENSIG_UpdatePeriode(&valeursParamGen);
}
  

// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    PLIB_TMR_Period16BitSet(TMR_ID_3, pParam->Frequence);
}

// Mise à jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam)
{
    *pParam = valeursParamGen;
    pParam->Amplitude = 300 ;
    pParam->Forme = SignalSinus ; 
    pParam->Offset = 0 ;
    pParam->Frequence = 8000 ;
  
    uint8_t i;
    
    switch(pParam->Forme)
    {
        case SignalSinus:
        {
            //Met dans une case du tableau, les valuers calculées ci-dessous 
            tableauValeursSignal[i] = VAL_MID_PAS + pParam ->Offset + (tableauCourbeSinus[i]-50) * pParam ->Amplitude;
        }
            break; 
            
        case SignalTriangle:
        {
             if ((MAX_ECH /2) > i)
                {
                    tableauValeursSignal[i] = VAL_MID_PAS + pParam ->Offset + (pParam ->Amplitude *(2 * (i -25)))  ;
                    
                }else 
                {

                    tableauValeursSignal[i] = VAL_MID_PAS + pParam ->Offset + (pParam ->Amplitude * (100 - 2 * (i-25))) ;
                }
        }   
            break ; 
            
            
        case SignalDentDeScie:
        {
            tableauValeursSignal[i] = VAL_MID_PAS + (pParam ->Offset + (((i-50) * pParam ->Amplitude)));    
        }
            break; 
            
            
        case SignalCarre: 
        {
             if ((MAX_ECH /2) > i)
                {
                    tableauValeursSignal[i] = (VAL_MID_PAS + (pParam ->Offset + (pParam ->Amplitude / 2 *MAX_ECH))) ;
                    
                }else 
                {

                    tableauValeursSignal[i] = (VAL_MID_PAS - (pParam ->Offset + (pParam ->Amplitude / 2 *MAX_ECH))) ;
                }
        }
            break; 

        default:
            break;
    }
}


// Execution du générateur
// Fonction appelée dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC à modifier
void  GENSIG_Execute(void)
{
   static uint16_t EchNb = 0;
   const uint16_t Step = 65535 / MAX_ECH;

   SPI_WriteToDac(0, tableauValeursSignal[EchNb]);      // sur canal 0
   EchNb++;
   EchNb = EchNb % MAX_ECH;
}
