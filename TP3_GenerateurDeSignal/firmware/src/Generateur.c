// Canevas manipulation GenSig avec menu
// P.M�lissa  09/02/2015
// Fichier Generateur.C
// Gestion  du g�n�rateur

// Pr�vu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber


#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "system_config.h"
#include "Mc32NVMUtil.h"
#include "Mc32DriverLcd.h"



// Variables globales
S_ParamGen valeursParamGen;  //Structure interm�diaire des param�tres du g�n�rateur
int32_t tableauValeursSignal[MAX_ECH];


//-------------------------------
// Fonction d'initialisation du g�n�rateur
// Auteur: JAR, MPT
// Date: 23.02.2024
// Entr�es: Pointeurs: Structure S_paramGen: pParam
// Sortie: -
//-------------------------------
void  GENSIG_Initialize(S_ParamGen *pParam)
{
    //R�cup�ration des donn�es sauvegard�es au d�marrage pr�c�dent
    //On fait un pointeur de 32bits 
    NVM_ReadBlock((uint32_t*)&valeursParamGen, sizeof(S_ParamGen));
    
    //Test pour v�rifier si MaGIC = MAGIC 
    if(valeursParamGen.Magic == MAGIC)
    {
        // Sauvegarde des valeurs r�cup�r�es
        *pParam = valeursParamGen;
        lcd_gotoxy(1,4);
        printf_lcd("Donnees restaurees");
    }
    else 
    {
        lcd_gotoxy(1,4);
        printf_lcd("Donnees par defaut");
        
        // Initialisation des valeurs du g�n�rateur
        pParam->Amplitude = 10000;
        pParam->Forme = SignalSinus;
        pParam->Frequence = 20;
        pParam->Magic = MAGIC;
        pParam->Offset = 0; 
    }
}
  


//-------------------------------
// Mise � jour de la periode d'�chantillonage
// Auteur: JAR, MPT
// Date: 23.02.2024
// Entr�es: Pointeurs: Structure S_paramGen: pParam
// Sortie: -
//-------------------------------
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    static uint16_t compteurTimer3;
   
    compteurTimer3 = (((TRANSFORMATION_VALEUR_TIMER3 / pParam->Frequence) -1) + 0.5);
    
    PLIB_TMR_Period16BitSet(TMR_ID_3, compteurTimer3);
}


//-------------------------------
// Mise � jour du signal (forme, amplitude, offset)
// Auteur: JAR, MPT
// Date: 23.02.2024
// Entr�es: Pointeurs: Structure S_paramGen: pParam
// Sortie: -
//-------------------------------
void    GENSIG_UpdateSignal(S_ParamGen *pParam)
{
    uint8_t nbEchantillon;
    uint16_t amplitude;
    int16_t offset;
   
    // Tableau des �chantillons de sinus
    const uint16_t tableauCourbeSinus[100] = {
     53, 56, 59, 62, 65, 68, 71, 74, 77, 79,
     82, 84, 86, 89, 90, 92, 94, 95, 96, 98,
     98, 99, 100, 100, 100, 100, 100, 99, 98,
     98, 96, 95, 94, 92, 90, 89, 86, 84, 82,
     79, 77, 74, 71, 68, 65, 62, 59, 56, 53,
     50, 47, 44, 41, 38, 35, 32, 29, 26, 23,
     21, 18, 16, 14, 11, 10, 8, 6, 5, 4, 2,
     2, 1, 0, 0, 0, 0, 0, 1, 2, 2, 4, 5, 6,
     8, 10, 11, 14, 16, 18, 21, 23, 26, 29,
     32, 35, 38, 41, 44, 47, 50};

    amplitude = pParam ->Amplitude / 100;
    offset = pParam->Offset * -0.5;//on fait fois -1 car le - est pour inverser le signal. Sinon le sinus monte quand on mettait -5000 au lieu de descendre 
    
    // Boucle pour le calcul de tous les �chantillons
    for (nbEchantillon = 0; nbEchantillon < MAX_ECH ; nbEchantillon++)
    {
        switch(pParam->Forme)
        {
            case SignalSinus:
            {
                // Calcul du sinus
                tableauValeursSignal[nbEchantillon] = VAL_DIVISION_TENSION + offset + (tableauCourbeSinus[nbEchantillon]-50) * amplitude;
            }
                break; 

            case SignalTriangle:
            {
                // Calcul du triangle
                if ((MAX_ECH * 0.5) > nbEchantillon)  //on fait pas directement divis�, car �a peut �tre couteux pour le uC
                    {
                        tableauValeursSignal[nbEchantillon] = VAL_DIVISION_TENSION + offset + (amplitude *(2 * (nbEchantillon -25))); //amplitude en mV
                    }
                else 
                    {

                        tableauValeursSignal[nbEchantillon] = VAL_DIVISION_TENSION + offset + (amplitude * (100 - 2 * (nbEchantillon-25)));
                    }
            }   
                break ; 
            case SignalDentDeScie:
            {
                // Calcul de la dent de scie
                tableauValeursSignal[nbEchantillon] = VAL_DIVISION_TENSION + (offset + (((nbEchantillon-50) * amplitude)));    
            }
                break; 

            case SignalCarre: 
            {
                // Calcul du carr�
                if ((MAX_ECH * 0.5) > nbEchantillon)  //*0.5 = division par 2
                    {
                        tableauValeursSignal[nbEchantillon] = VAL_DIVISION_TENSION + ((amplitude * 0.5 * MAX_ECH)+ offset);
                    }
                 else 
                    {
                        tableauValeursSignal[nbEchantillon] = VAL_DIVISION_TENSION - ((amplitude * 0.5 * MAX_ECH)- offset);
                    }
            }
                break; 

            default:
                break;
        }
    
        // Gestion de l'�cr�tage
        if(tableauValeursSignal[nbEchantillon] > MAX_AMPLITUDE)
        {
            tableauValeursSignal[nbEchantillon] = MAX_AMPLITUDE;
        }
        else if (tableauValeursSignal[nbEchantillon] < 0) 
        {
            tableauValeursSignal[nbEchantillon] = 0;
        }
        // Transforme les valeur en volt
        tableauValeursSignal[nbEchantillon] = ((VAL_MAX_PAS * tableauValeursSignal[nbEchantillon])/10000);  
    }
}


//-------------------------------
// Execution du g�n�rateur en envoient les valeurs calcul�es au dac
// Auteur: JAR, MPT
// Date: 23.02.2024
// Entr�es: -
// Sortie: -
//-------------------------------
void  GENSIG_Execute(void)
{
   static uint16_t EchNb = 0;

   // Envoi la valeur du signal au dac
   SPI_WriteToDac(0, tableauValeursSignal[EchNb]);      // sur canal 0
   EchNb++; // Passe � l'�chantillon suivant
   EchNb = EchNb % MAX_ECH; // G�re les d�bordements du nombre d'�chantillon
}
