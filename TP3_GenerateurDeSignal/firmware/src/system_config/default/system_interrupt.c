/*******************************************************************************
 System Interrupts File

  File Name:
    system_interrupt.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

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

#include "system/common/sys_common.h"
#include "app.h"
#include "system_definitions.h"
#include "Generateur.h"
#include "GesPec12.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************


void __ISR(_TIMER_1_VECTOR, ipl3AUTO) IntHandlerDrvTmrInstance0(void)
{
    BSP_LEDToggle(BSP_LED_1);
    //  LED1_W = !LED1_R;
    static uint16_t i = 0;
    
    ScanPec12(PEC12_A, PEC12_B, PEC12_PB);
    
    ScanS9(S_OK);// A changer
    
    i++;
    if(i >= 3000)
    {
        i = 2990;
        APP_UpdateState(APP_STATE_SERVICE_TASKS);
    }
    

//    //LED1_W = 1;  //Sert à prendre les mesures, mais peut être retiré pour le code final  
//    static uint16_t i;
//    static uint8_t compteur; 
//    
//    if (i > 3000)
//    {   
//        APP_UpdateState (APP_STATE_SERVICE_TASKS);
//    }
//    else
//    {
//        APP_UpdateState (APP_STATE_WAIT);
//        i++;
//    }
//    
//    if(compteur < 9)
//    {
//        compteur++;
//    }
//    else
//    {
//        compteur = 0;

//    }
//
//    
//    
//    ScanPec12(PEC12_A,PEC12_B,PEC12_PB);
//        //LED1_W = 0; //Sert à prendre les mesures, mais peut être retiré pour le code final 

    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_1);
}

/*void __ISR(_TIMER_3_VECTOR, ipl7AUTO) IntHandlerDrvTmrInstance1(void)
{

    LED1_W = !LED1_R;
    //LED1_W = 1;  //Sert à prendre les mesures, mais peut être retiré pour le code final  
    static uint16_t i;
    static uint8_t compteur; 
    
    if (i > 3000)
    {   
        APP_UpdateState (APP_STATE_SERVICE_TASKS);
    }
    else
    {
        APP_UpdateState (APP_STATE_WAIT);
        i++;
    }
    
    if(compteur < 9)
    {
        compteur++;
    }
    else
    {
        compteur = 0;
        LED2_W = !LED2_R;
    }

    
    
    ScanPec12(PEC12_A,PEC12_B,PEC12_PB);
    
    //LED1_W = 0; //Sert à prendre les mesures, mais peut être retiré pour le code final 
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_1);
}*/


void __ISR(_TIMER_3_VECTOR, ipl7AUTO) IntHandlerDrvTmrInstance1(void)
{
    LED0_W = 1;
    GENSIG_Execute(); 
    LED0_W = 0;
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
}
 /*******************************************************************************
 End of File
*/
