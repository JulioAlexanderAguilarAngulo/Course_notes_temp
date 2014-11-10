// Include Standard files
#include "include/AT91SAM7X-EK.h" //click droit, open
#include <aic/aic.h>
#include <pit/pit.h>

/* Global variables */
#define SPEED 		(MCKKHz/1000) // (48000/1000)
#define PIT_PERIOD      5
#define maxval          100
#define minval          10

unsigned short duty;
unsigned int LedSpeed   = SPEED *10000 ;

const int ledmask[8]  = {LED1, LED2, LED3, LED4, LED3, LED2, LED1, 0x00 };
const int ledmask_2[8] = {LED1, LED1 | LED2, LED1 | LED2 | LED3, LED1 | LED2 | LED3 | LED4, LED2 | LED3 | LED4, LED3 | LED4, LED4, 0x00};
int tmpval, joyval;
char tick1;

void ConfigSyst(void);
/*
AT91C_BASE_PITC->PITC_PIMR = MCKKHz /( 16 * 250000) - 1;

//disabled interrupt
AT91C_BASE_AIC->AIC_IDR = 0xFFFFFFFF;
//clear all interrupt
AT91C_BASE_AIC->AIC_ICCR = 0xFFFFFFFF;

AT91C_BASE_AIC->AIC_SMR[AT91C_ID_SYS] |= AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL;
AT91C_BASE_AIC->AIC_SVR[AT91C_ID_SYS]  = (unsigned) ISR_PIT;

AT91C_BASE_AIC->
*/

void wait(void)
{
    unsigned int waiting_time ;
    for(waiting_time = 0; waiting_time < LedSpeed; waiting_time++) ;
    
}

void ISR_PIT (void)
{
 // This function is to be executed for each interrupt on PIT
// To be completed ...
    unsigned int dummy;

    // Read the PIT status register
    dummy = AT91C_BASE_PITC->PITC_PISR & AT91C_PITC_PITS;
    if (dummy != 0) 
    {     
    // On augmente le pas pour que les calculs ailles plus vite
    tick1+=5;
    
    // tick < valeur_d'échantillonnage
    if (tick1 < duty)
    {           
            // On allume la diode
            BASE_PIO_LED->PIO_CODR = LED2;
    }
    else
    {
            // Inverse
            BASE_PIO_LED->PIO_SODR = LED2;
    }
    if (tick1 == maxval)
    {
            // Réinit du tick
            tick1 = 0;
    }
    
        // Read the PIVR to acknowledge interrupt and get number of ticks
    dummy=  AT91C_BASE_PITC->PITC_PIVR;
    }
}

void main(void)
{
    // minval et maxval sont deux define judicieusement choisi (10 et 100)
    ConfigSyst();
    tick1=0;
    BASE_PIO_LED->PIO_CODR = LED2;
    short it = 0;
    int cpt  = 0;
    // L'interruption va donc commencée à 10 et non pas à 0
    duty     = minval;
    // unsigned int delay = SPEED * 10000;
    while(1)
    {
        // Les deux boucles vont de 1 à 6 de manière très judicieuse
        for (it=1 ; it <= 5 ; it++)
        {
            // L'incrémentation de ce compteur déclenche l'interruption
            cpt++;
            // On incrémente duty pour qu'au prochain tour de boucle l'allumage 
            // de la led par l'interruption soit plus rapide
            duty+=10;
            // Petit délais pour que ça soit plus fluide
            wait();
            // On recommence jusqu'à 6, à ce moment là duty vaudra normalement 
            // 100 (led allumée tout le temps)
            // Où alors si c'est moins la led sera ON/OFF suffisament vite pour
            // donner l'illusion qu'elle reste ON
        }
        for (it=1 ; it <= 5 ; it++)
        {
            // Opération inverse, la led est ON et on va l'éteindre 
            // progressivement
            // Toujours le compteur pour enclencher l'interruption
            cpt--;
            // Cette fois on décrémente duty, qui part de sa valeur max jusqu'à 
            // sa valeur min
            duty-=10;
            // Cute delay
            wait();       
        };
        // infinite loop to make LED1 blink.
      /*for (;;)
        {
            BASE_PIO_LED->PIO_SODR = LED1;
            wait();
            BASE_PIO_LED->PIO_CODR = LED1;
            wait();
        }*/ 
    }
}

void ConfigSyst(void){
    // Fisrt, activate PIO_LED
       AT91C_BASE_PMC->PMC_PCER =  (1 << ID_PIO_LED);

        //then set IOs
           BASE_PIO_LED->PIO_PER = LED_MASK; // Set in PIO mode
           BASE_PIO_LED->PIO_OER = LED_MASK; // Configure in Output
        // and switch off all LEDs
           BASE_PIO_LED->PIO_SODR = LED_MASK ;
           
/*The primary goal of the Peripheral Interval Timer (PIT) is to generate periodic
interrupts. This is
most often used to provide the base tick of an operating system. The PIT uses MCK
divided by
16 as its input clock, as well as a 20-bit counter. Each time the counter reaches a
programmable
value, an interrupt is generated, and a second counter increments. The latter makes
it possible
to never miss a tick, even when the system is overloaded.  The getting started
example uses the PIT to provide a 1 ms time base. Each time the PIT interrupt
is triggered, a 32-bit counter is incremented. A Wait function uses this counter to
provide a
precise way for an application to suspend itself for a specific amount of time.
*/

/**/           
        // PIT initialization
        // To be completed, expected PIT period is 0.25s
/*
Since the PIT is part of the System Controller, it is continuously clocked. As such,
there is no
need to enable its peripheral clock in the PMC.
The Mode Register contains the Periodic Interval Value (PIV) which indicates to the
PIT when to
reset the internal counter. It must be programmed to the number of ticks generated
by MCK/16         
           */        
// This is done with the following line of code:(250 ms tick)
 AT91C_BASE_PITC->PITC_PIMR = BOARD_MCK/100000 * PIT_PERIOD +8; //48 000 000 / 100 000 * 250000  +8
// Before starting the timer, the interrupt must be configured in the AIC. Please refer to Section


    // AIC initialization_ advanced interrupt controller
           
// Disable all interrupts
AT91C_BASE_AIC->AIC_IDCR = 0xFFFFFFFF;
// Clear all interrupts
AT91C_BASE_AIC->AIC_ICCR = 0xFFFFFFFF;
  
    // Configure interrupt on PIT
    AT91C_BASE_AIC->AIC_SMR[AT91C_ID_SYS] |= AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL; //Mode haut niveau (détection d'interruption)

    AT91C_BASE_AIC->AIC_SVR[AT91C_ID_SYS] = (unsigned) ISR_PIT; 

    AT91C_BASE_AIC->AIC_IECR = 1 << AT91C_ID_SYS;
    AT91C_BASE_PITC->PITC_PIMR |= AT91C_PITC_PITIEN;

    // Enable the pit
    AT91C_BASE_PITC->PITC_PIMR |= AT91C_PITC_PITEN;
    
        }
