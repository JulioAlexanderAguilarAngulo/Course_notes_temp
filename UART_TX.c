// Include Standard files
#include "include/AT91SAM7X-EK.h"
/* Global variables */
#define SPEED 		(MCKKHz/1000)
#define BAUDRATE        115200

unsigned int LedSpeed = SPEED *500;//00 ;

const int ledmask[NB_LED]= {LED1, LED2, LED3, LED4};

unsigned int mode =       AT91C_US_USMODE_NORMAL
                        | AT91C_US_CLKS_CLOCK
                        | AT91C_US_CHRL_8_BITS
                        | AT91C_US_PAR_NONE
                        | AT91C_US_NBSTOP_1_BIT
                        | AT91C_US_CHMODE_NORMAL;

char message[]="\x6d\x6f\x6e\x20\x70\x72\x6f\x66\x65\x73\x73\x65\x75\x72\x20\x73\x65\x20\x6d\x6f\x71\x75\x65\x20\x64\x65\x20 \x6d\x6f\x69 \x20 \x2e\x2e\x2e \x0D \x0A";
char imessage;

void wait ( void )
{
    unsigned int waiting_time ;
    for(waiting_time = 0; waiting_time < LedSpeed; waiting_time++) ;
}


int main()
{

int i;
imessage=0;    
    // d'abord, il faut activer l'horloge sur le PIO
       AT91C_BASE_PMC->PMC_PCER =  (1 << ID_PIO_LED);
    // ensuite, il faut activer l'horloge sur le module UART
       AT91C_BASE_PMC->PMC_PCER =  (1 << ID_UART);

    // Ensuite, on attribue les broche des LEDs au PIO, puis on les configure
    // en sortie.
	BASE_PIO_LED->PIO_PER = LED_MASK; // Set in PIO mode
	BASE_PIO_LED->PIO_OER = LED_MASK; // Configure in Output


    // on commence par initialiser les LED dans l'état éteint
    // on écrit dans le registre SODR, pour mettre à '1' car on est en
    // logique inversée
       BASE_PIO_LED->PIO_SODR = LED_MASK ;

    // il faut définir que les broches UART_RX_PIN et UART_TX_PIN  ne sont pas en mode PIO
       AT91C_BASE_PIOA-> PIO_PDR = UART_TX_PIN|UART_RX_PIN; // disable IO enable peripheral ctrl
       AT91C_BASE_PIOA-> PIO_ASR = UART_TX_PIN|UART_RX_PIN; // à compléter
    // puis les attribuer au module A
   //    AT91C_BASE_PIOA-> ...; // à compléter

    // cet accès valide les RX et TX.
       BASE_UART->US_CR= AT91C_US_RXEN | AT91C_US_TXEN;
    // il faut mettre le module USART0 (US0) en mode remote loopback
       BASE_UART->US_MR= mode; // à compléter

       BASE_UART->US_BRGR = (MCK / BAUDRATE) / 16;
       BASE_UART->US_CR = AT91C_US_TXEN;

    // boucle infinie
    for (;;)
    {
	for ( i=0 ; i < NB_LED-1 ; i++ )
        {
        BASE_PIO_LED->PIO_CODR = ledmask[i] ;
        wait();
        BASE_PIO_LED->PIO_SODR = ledmask[i] ;
        }

        for ( i=(NB_LED-1) ; i > 0 ; i-- )
        {
        BASE_PIO_LED->PIO_CODR = ledmask[i] ;
        wait();
        BASE_PIO_LED->PIO_SODR = ledmask[i] ;
        }

        BASE_UART->US_THR = message[imessage];//'A';
        while ((BASE_UART->US_CSR & AT91C_US_TXEMPTY) == 0){};        
        if (message[imessage]==0X0A)
          {
            imessage=0;
          }
        else {
        imessage++;
        }
    }
}
