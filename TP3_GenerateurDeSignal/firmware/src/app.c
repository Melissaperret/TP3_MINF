/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
     Pour Tp3 Menu et generateur de signal .

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "Mc32gestSpiDac.h"
#include "MC32DriverLcd.h"
#include "MenuGen.h"
#include "GesPec12.h"
#include "Generateur.h"
#include "Mc32Debounce.h"

// Descripteur des sinaux
S_SwitchDescriptor DescrS9;

// Structure pour les traitements de S9
S_Pec12_Descriptor S9;

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;
S_ParamGen LocalParamGen;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback funtions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            LED2_W = 0; 
            
            lcd_init();
            lcd_bl_on();
            
            // Init SPI DAC
            SPI_InitLTC2604();
            
            // Initialisation PEC12
            Pec12Init();

            // Initialisation S9
            S9Init();
            
            // Initialisation du generateur
            GENSIG_Initialize(&LocalParamGen);   

            lcd_gotoxy(1,1);
            printf_lcd("TP3 GenSig 23-24");
            lcd_gotoxy(1,2); 
            printf_lcd("Melissa Perret");
            lcd_gotoxy(1,3); 
            printf_lcd("Jeremy Affolter");
            
            
            GENSIG_UpdateSignal(&LocalParamGen);
            GENSIG_UpdatePeriode(&LocalParamGen);
            
            // Active les timers 
            DRV_TMR0_Start();
            DRV_TMR1_Start();
            
            LED2_W = 1;
            
            APP_UpdateState(APP_STATE_WAIT);    // Passer à l'état d'attente
            break;
        }
        case APP_STATE_WAIT :
        {
            // nothing to do
            break;
        }

       case APP_STATE_SERVICE_TASKS:
            // Toggle de la led 2
            BSP_LEDToggle(BSP_LED_2);

            // Scan des boutons
            
            // Execution du menu
            MENU_Execute(&LocalParamGen);
            APP_UpdateState(APP_STATE_WAIT);    // Passer à l'état d'attente
         break;

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

void APP_UpdateState ( APP_STATES NewState )
{
    appData.state = NewState;   // Change l'état de la machine d'état
}

//-------------------------------
// Gestion du bouton S9
// Auteur: JAR, MPT
// Date: 01.03.2024
// Entrées: bool ValS9
// Sortie: -
//-------------------------------
void ScanS9(bool ValS9)
{
   // Traitement antirebond
   DoDebounce (&DescrS9, ValS9);
   
   // Compte le temps d'appui
   if(ValS9 == 0)
   {
       S9.PressDuration++;
   }
   
   // Lors d'un relachement
   if(DebounceIsReleased(&DescrS9))
   {
       // Test de la durée d'appui
       if(S9.PressDuration >= PRESSION_LONGUE)
       {
           // Appui long
           S9.ESC = 1;
           S9.OK = 0;
       }
       else
       {
           // Appui court
           S9.OK = 1;
           S9.ESC = 0;
       }
       S9.PressDuration = 0;    // Remise à 0 du temps d'appui
   }
   // Clear les flag d'appui et de relachement du bouton
   DebounceClearPressed(&DescrS9);
   DebounceClearReleased(&DescrS9);
}

//       S9IsOK         true indique action OK
bool S9IsOK    (void) {
   return (S9.OK);
}

//       S9IsESC        true indique action ESC
bool S9IsESC    (void) {
   return (S9.ESC);
}

//       S9ClearOK      annule indication action OK
void S9ClearOK   (void) {
   S9.OK = 0;
}

//       S9ClearESC     annule indication action ESC
void S9ClearESC   (void) {
   S9.ESC = 0;
}

void S9Init (void)
{
   // Initialisation du descripteur de S9
   DebounceInit(&DescrS9);
   
   // Init de la structure S9
    S9.Inc = 0;             // Événement incrément  
    S9.Dec = 0;             // Événement décrément 
    S9.OK = 0;              // Événement action OK
    S9.ESC = 0;             // Événement action ESC
    S9.NoActivity = 0;      // Indication d'activité
    S9.PressDuration = 0;   // Pour durée pression du P.B.
    S9.InactivityDuration = 0; // Durée inactivité
  
 } // Pec12Init

/*******************************************************************************
 End of File
 */

