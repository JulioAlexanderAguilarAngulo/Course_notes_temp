// Include Standard files
// TP001a: led sequences (masks and access to tables)
// TP001b: led sequences (pointers)
#include "include/AT91SAM7X-EK.h"


/* Global variables */
#define SPEED 		(MCKKHz/1000)
#define PULL_UP
unsigned int LedSpeed = SPEED *1000 ;

const int ledmask[8]= {LED1, LED2, LED3, LED4, LED3, LED2, LED1, 0x00 };
const int delays[8]= {2,4,6,8,10,12,14,0x00 };
int tmpval;
char module,on_change,tick;
char ep,ea;

void Config_System(void);
void Delay_Soft(void);
void DelayN(int counts);

/***********************************************************************/

void main(void){
    int i;
      Config_System();
       i=0;tick=0;
       ep=0;ea=0;
  while(1){

    if(~(BASE_PIO_PUSHB->PIO_PDSR) & AT91B_PBUP)
      {
        ep=1; // freeze!!
      }
    else if(~(BASE_PIO_PUSHB->PIO_PDSR) & AT91B_PBRIGHT)
      {
        ep=2; // faster!!
      }
    else if(~(BASE_PIO_PUSHB->PIO_PDSR) & AT91B_PBLEFT)
      {
        ep=3; // slower!!
      }
    else if(~(BASE_PIO_PUSHB->PIO_PDSR) & AT91B_PBDOWN)
      {
        ep=4; // whatever!!
      }
    else
      {
        ep=0;
      }
//  now testing release condition
//  transitions:  X -> 0   0 -> X  (X>0)   
    if (ea==ep)
      {
      on_change=0;
      }
    else
      {
      on_change=1;
      if (ep>ea)
        {
            switch(ep)
          {
            case 0x01:
              module=0x01;
              module&=0x0F;
              break;
            case 0x02:
              {module++;
              if (module>=0x0F) 
              {module=0x0F;};}
              break;
            case 0x03:
              {module--;
              if (module<=0x01) 
              {module=1;};}
              break;
            case 0x04:
              module=0x05;
              module&=0x0F;
              break;
          }            
        }
      }
  
    Delay_Soft(); 
    BASE_PIO_LED->PIO_SODR ^= tmpval ;//toggle LEDs to initial state
    tmpval=ledmask[i];
    if (tick==module)
      {
        tick=0;
        i++;
        if (tmpval==0)
          {
          i=0;
          }
      }
          
          #ifdef PULL_UP        
            BASE_PIO_LED->PIO_CODR = tmpval ;
          #else
            BASE_PIO_LED->PIO_SODR = tmpval ;
          #endif       
            
    tick++;
    tick&=0x0F;    
    ea=ep;    
  }
}
void Config_System(void)
{
    // d'abord, il faut activer l'horloge sur les PIOs
    // PRC  page 181 datasheet  
    
      AT91C_BASE_PMC->PMC_PCER =  (1 << ID_PIO_LED);
      AT91C_BASE_PMC->PMC_PCER =  (1 << ID_PIO_PUSHB);

    // Ensuite, on attribue les broche des LEDs au PIO, puis on les configure
	BASE_PIO_LED->PIO_PER = LED_MASK; // Set in PIO mode
	BASE_PIO_LED->PIO_OER = LED_MASK; // Configure as Output

	BASE_PIO_PUSHB->PIO_PER = AT91B_PB_MASK; // Set in PIO mode
	BASE_PIO_PUSHB->PIO_PDSR = AT91B_PB_MASK; // Configure as Input

    // on commence par initialiser les LED dans l'état éteint
    // on écrit dans le registre SODR, pour mettre à '1' car on est en
    // logique inversée
        
#ifdef PULL_UP        
       BASE_PIO_LED->PIO_SODR = LED_MASK ;
#else
       BASE_PIO_LED->PIO_CODR = LED_MASK ;
#endif       
}

void Delay_Soft(void)
{
    unsigned int waiting_time ;
    for(waiting_time = 0; waiting_time < LedSpeed; waiting_time++) ;
}

void DelayN(int counts)
{
  while(counts > 0)
  {
    Delay_Soft();
    counts--;
  }
} //pin data status register => PDSR



