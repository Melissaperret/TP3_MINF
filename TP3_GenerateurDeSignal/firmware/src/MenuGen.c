// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Modifié par: JAR et MPT le 01.03.2024
// Fichier MenuGen.c
// Gestion du menu  du générateur
// Traitement cyclique à  10 ms


// Librairie inclues
#include <stdint.h>                   
#include <stdbool.h>
#include "MenuGen.h"
#include "Mc32DriverLcd.h"
#include "app.h"
#include "GesPec12.h"
#include "Mc32NVMUtil.h"
#include <math.h>
#include "Generateur.h"


// Constante pour l'affichage des formes
const char MenuFormes[4][21] = {"Sinus", "Triangle", "DentDeScie", "Carre"};

//-------------------------------
// Fonction d'execution du menu, appel cyclique depuis l'application
// Auteur: JAR, MPT
// Date: 01.03.2024
// Entrées: Pointeurs: Structure S_paramGen: pParam
// Sortie: -
//-------------------------------
void MENU_Execute(S_ParamGen *pParam)
{
    static MENU_STATE menuStat = SEL_FORME; // Variable de l'état du menu
    static MENU_STATE oldMenuStat = SET_FORME;  // Variable de l'ancien état du menu
    static S_ParamGen tempData; // Structure de sauvegarde temporaire des paramètres
    static uint8_t i = 0;   // Variable indiquant si on est au premier passage
    static uint8_t y = 0;   // Variable de comptage de la boucle d'attente
    
    // Initialisation lors de la première exécution
    if(!i)
    {
        // Initialisation du menu
        MENU_Initialize(pParam);
        // Copie de la structure des paramètres dans la structure temporaire
        tempData = *pParam;
        i++;
    }
    
    if(menuStat != SAVE)
    {
        if(menuStat % 2) //si menu est en mode setting 
        {
            // Affiche les valeurs dans le menu
            AfficheMenu(&tempData);
            // Gestion des état de setting du menu
            menuStat = GestSettingMenu(menuStat, &tempData, pParam);
        }
        else
        {
            // Tests du pec12
            if(Pec12IsOK())
            {
                // Passer à l'état de setting
                menuStat ++;
            }
            else if(Pec12IsPlus())
            {
                // Passer au prochaine état de sélection
                menuStat += 2;
                if(menuStat >= SAVE)
                {
                    menuStat = SEL_FORME;
                }
            }
            else if(Pec12IsMinus())
            {
                // Passer à l'état précédent de selection
                menuStat -= 2;
                if(menuStat > SAVE)
                {
                    menuStat = SEL_OFFSET;
                }
            }
        }
        // Appui sur S9
        if(S9IsOK()||S9IsESC())
        {
            // Indiquer une activité
            Pec12ClearInactivity();
            // Clear de S9
            S9ClearESC();    
            S9ClearOK();
            // Passer à l'état de sauvegarde
            menuStat = SAVE;
        }
    }
    
    // Enlever l'astérix
    if((abs(oldMenuStat - menuStat)) >= 2)
    {
        lcd_gotoxy(1,1); 
        printf_lcd(" ");
        lcd_gotoxy(1,2); 
        printf_lcd(" ");
        lcd_gotoxy(1,3); 
        printf_lcd(" ");
        lcd_gotoxy(1,4); 
        printf_lcd(" ");
    }
    
    // Sauvegarde de l'état du menu
    oldMenuStat = menuStat;
    
    // Affiche le ? ou l'* qui correspond à l'état
    switch(menuStat)
    {
        case SEL_FORME:
            lcd_gotoxy(1,1); 
            printf_lcd("*");
            break;
            
        case SET_FORME:
            lcd_gotoxy(1,1); 
            printf_lcd("?");
            break;
            
        case SEL_FREQU:
            lcd_gotoxy(1,2); 
            printf_lcd("*");
            break;
            
        case SET_FREQU:
            lcd_gotoxy(1,2); 
            printf_lcd("?");
            break;
            
        case SEL_AMPL:
            lcd_gotoxy(1,3); 
            printf_lcd("*");
            break;
            
        case SET_AMPL:
            lcd_gotoxy(1,3); 
            printf_lcd("?");
            break;
            
        case SEL_OFFSET:
            lcd_gotoxy(1,4); 
            printf_lcd("*");
            break;
            
        case SET_OFFSET:
            lcd_gotoxy(1,4); 
            printf_lcd("?");
            break;
            
        // État de sauvegarde dans la flash
        case SAVE:
            // Appui long
            if(S9IsESC())
            {
                y++;
                Pec12ClearInactivity(); // Activité
                //Sauvegarde
                NVM_WriteBlock((uint32_t*)pParam, sizeof(S_ParamGen));
                lcd_gotoxy(1,2);
                printf_lcd("    Sauvegarde OK   ");
                lcd_ClearLine(3);
            }
            // Toutes autres actions
            else if(S9IsOK() || Pec12IsESC() || Pec12IsMinus() || Pec12IsOK() || Pec12IsPlus())
            {
                y++;
                Pec12ClearInactivity(); // Activité
                //Sauvegarde annulée
                lcd_gotoxy(1,2);
                printf_lcd(" Sauvegarde ANNULEE ");
                lcd_ClearLine(3);
            }
            
            // Boucle d'attente de 2s
            if(y > 0)
            {
                y++;
                if (y > 200)
                {
                    // Réinitialiser l'affichage du menu
                    MENU_Initialize(pParam);
                    y = 0;  // Remise à zéros de la variable d'attente
                    menuStat = SEL_FORME;   // Repaser au premier état
                }
            }
            else
            {
                // Afficher la question de la sauvegarde
                lcd_ClearLine(1);
                lcd_ClearLine(4);
                lcd_gotoxy(1,2);
                printf_lcd("    Sauvegarde ?    ");
                lcd_gotoxy(1,3);
                printf_lcd("    (Appui long)    ");
            }
            break;
            
        default:
            break;
    }  
    
    // Clear de S9 et du Pec12
    S9ClearESC();    
    S9ClearOK();
    Pec12ClearESC();    
    Pec12ClearOK();
    Pec12ClearPlus();
    Pec12ClearMinus();
}

//-------------------------------
// Fonction d'affichage initiale du menu
// Auteur: JAR, MPT
// Date: 08.03.2024
// Entrées: Pointeurs: Structure S_paramGen: pParam
// Sortie: -
//-------------------------------
void MENU_Initialize(S_ParamGen *pParam)
{
    lcd_gotoxy(2,1); 
    printf_lcd("Forme = %10s  ", MenuFormes[pParam->Forme]);
    lcd_gotoxy(2,2); 
    printf_lcd("Freq [Hz] =  %4d   ", pParam->Frequence);
    lcd_gotoxy(2,3); 
    printf_lcd("Ampl [mV] = %5d   ", pParam->Amplitude);
    lcd_gotoxy(2,4); 
    printf_lcd("Offset [mV] = %5d ", pParam->Offset);
}

//-------------------------------
// Fonction d'affichage des valeurs du menu
// Auteur: JAR, MPT
// Date: 08.03.2024
// Entrées: Pointeurs: Structure S_paramGen: pParam
// Sortie: -
//-------------------------------
void AfficheMenu(S_ParamGen *pParam)
{
    lcd_gotoxy(10,1);
    printf_lcd("%10s", MenuFormes[pParam->Forme]);
    lcd_gotoxy(15,2);
    printf_lcd("%4d", pParam->Frequence);
    lcd_gotoxy(14,3);
    printf_lcd("%5d", pParam->Amplitude);
    lcd_gotoxy(16,4);
    printf_lcd("%5d", pParam->Offset);
}
    
//-------------------------------
// Fonction de gestion des états de setting des données 
// Auteur: JAR, MPT
// Date: 18.03.2024
// Entrées: Structure MENU_STATE menuState Pointeurs: Structure S_paramGen: pParam1, pParam2
// Sortie: Structure MENU_STATE
//-------------------------------
MENU_STATE GestSettingMenu(MENU_STATE menuStat, S_ParamGen *tempData, S_ParamGen *pParam)
{
    // Sauvegarde des paramètres modifiés
   if(Pec12IsOK())
    {
        // Copie de la structure temporaire dans la structure des paramètres 
        *pParam = *tempData;
        menuStat --;    // Revenir à l'état de selection
        AfficheMenu(tempData);  // Affichage des valeurs
        // Mise à jour du signal
        GENSIG_UpdateSignal(pParam);
        GENSIG_UpdatePeriode(pParam);
    }
   // Non Sauvegarde des paramètres modifiés
    else if(Pec12IsESC())
    {
        // Copie de la structure des paramètres dans la structure temporaire
        *tempData = *pParam;
        menuStat --;    // Revenir à l'état de selection
        AfficheMenu(pParam);    // Affichage des valeurs
    }
    // Si le Pec12 est tourné à droite
    else if(Pec12IsPlus())
    {
        switch(menuStat)
        {
            case SET_FORME:
                // Passer à la forme suivante à part si on est au maximum
                if(tempData->Forme < SignalCarre)
                {
                    tempData->Forme++;
                }
                else
                {
                    tempData->Forme = SignalCarre;
                }
                break;

            case SET_FREQU:
                // Augmenter la fréquence d'un pas à part si il y a un rebouclement
                if(tempData->Frequence < FREQU_MAX)
                {
                    tempData->Frequence += FREQU_MIN;
                }
                else
                {
                    tempData->Frequence = FREQU_MIN;
                }
                break;

            case SET_AMPL:
                // Augmenter l'amplitude d'un pas à part si il y a un rebouclement
                if(tempData->Amplitude < AMPL_MAX)
                {
                    tempData->Amplitude += PAS_AMPL;
                }
                else
                {
                    tempData->Amplitude = AMPL_MIN;
                }
                break;

            case SET_OFFSET:
                // Augmenter l'offset d'un pas à part si on est au maximum
                if(tempData->Offset < OFFSET_MAX)
                {
                    tempData->Offset += PAS_OFFSET;
                }
                else
                {
                    tempData->Offset = OFFSET_MAX;
                }
                break;

            default:
                break;
        }
    }
   // Si le Pec12 est tourné à gauche
    else if(Pec12IsMinus())
    {
        switch(menuStat)
        {
            case SET_FORME:
                // Passer à la forme précédente à part si on est au minimum
                if(tempData->Forme > SignalSinus)
                {
                    tempData->Forme--;
                }
                else
                {
                    tempData->Forme = SignalSinus;
                }
                break;

            case SET_FREQU:
                // Diminue la fréquence d'un pas à part si il y a un rebouclement
                if(tempData->Frequence > FREQU_MIN)
                {
                    tempData->Frequence -= FREQU_MIN;
                }
                else
                {
                    tempData->Frequence = FREQU_MAX;
                }
                break;

            case SET_AMPL:
                // Diminue l'amplitude d'un pas à part si il y a un rebouclement
                if(tempData->Amplitude > AMPL_MIN)
                {
                    tempData->Amplitude -= PAS_AMPL;
                }
                else
                {
                    tempData->Amplitude = AMPL_MAX;
                }
                break;

            case SET_OFFSET:
                // Diminue l'offset d'un pas à part si on est au minimum
                if(tempData->Offset > OFFSET_MIN)
                {
                    tempData->Offset -= PAS_OFFSET;
                }
                else
                {
                    tempData->Offset = OFFSET_MIN;
                }
                break;

            default:
                break;
        }
    } 
   return menuStat; // Retourne l'état du menu
}