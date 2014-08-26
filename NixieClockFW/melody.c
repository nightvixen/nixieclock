#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "notes.h"

void buz_freq(int freq);
void buz_mute()
{
DDRB &= ~_BV(PB1);
}


int buzzer_init(void);
void delay_ms(int n);

#define DJEN_LAENGE 71
const int	Djen [DJEN_LAENGE][2] ={
/*{S, CEL},
{A1, SEM},
{A1, SEM}, 
{A1, SEM },
{F1, QV },
{C2 , D_Cr},
{A1, SEM},
{F1, QV },
{C2 , D_Cr},
{A1, CEL },
{E2 , SEM },
{E2 , SEM },
{E2 , SEM },
{F2 , QV },
{C2 , D_Cr},
{Gis1 , SEM },
{F1, QV },
{C2 , D_Cr}, 
{A1, CEL },
{A2 , SEM },
{A1, QV},
{A1, D_Cr},
{A2 , SEM},
{Gis2, Cr},
{G2 , Cr },
{Fis2 , QV },
{F2 , QV},
{Fis2 , Cr}, 
{S, Cr },
{As , Cr},
{Dis2 , SEM },
{D2, Cr },
{Cis2, Cr},
{C2 , QV },
{Ais1, QV },
{C2 , Cr},
{S, Cr },
{F1, QV},
{Gis1 , SEM},
{F1, Qu },
{A1, QV},
{C2 , SEM },
{A1, Qu },
{C2 , QV},
{E2 , CEL},
{A2 , SEM },
{A1, QV},
{A1, D_Cr },
{A2 , SEM},
{Gis2, Cr},
{G2 , Cr},
{Fis2 , QV},
{F2 , QV },
{Fis2 , Cr},
{S, Cr},
{As , Cr},
{Dis2 , SEM},
{D2, Cr},
{Cis2, Cr},
{C2 , QV},
{Ais1, QV},
{C2 , Cr },
{S, Cr},
{F1, Cr },
{Gis1 , SEM},
{F1, Qu },
{C2 , QV },
{A1, SEM },
{F1, Qu},
{C1, QV},
{A1, CEL},*/
{S, CEL}, 
{A1, 500}, 
{A1, 500}, 
{A1, 500}, 
{F1, 350}, 
{C2, 150}, 
{A1, 500}, 
{F1, 350}, 
{C2, 150}, 
{A1, 1000}, 
{E2, 500}, 
{E2, 500}, 
{E2, 500}, 
{F2, 350}, 
{C2, 150}, 
{Gis1, 500}, 
{F1, 350}, 
{C2, 150}, 
{A1, 1000}, 
{A2, 500}, 
{A1, 350}, 
{A1, 150}, 
{A2, 500}, 
{Gis2, 250}, 
{G2, 250}, 
{Fis2, 125}, 
{F2, 125}, 
{Fis2, 250}, 
{S, 250}, 
{As, 250}, 
{Dis2, 500}, 
{D2, 250}, 
{Cis2, 250}, 
{C2, 125}, 
{Ais1, 125}, 
{C2, 250}, 
{S, 250}, 
{F1, 125}, 
{Gis1, 500}, 
{F1, 375}, 
{A1, 125}, 
{C2, 500}, 
{A1, 375}, 
{C2, 125}, 
{E2, 1000}, 
{A2, 500}, 
{A1, 350}, 
{A1, 150}, 
{A2, 500}, 
{Gis2, 250}, 
{G2, 250}, 
{Fis2, 125}, 
{F2, 125}, 
{Fis2, 250}, 
{S, 250}, 
{As, 250}, 
{Dis2, 500}, 
{D2, 250}, 
{Cis2, 250}, 
{C2, 125}, 
{Ais1, 125}, 
{C2, 250}, 
{S, 250}, 
{F1, 250}, 
{Gis1, 500}, 
{F1, 375}, 
{C2, 125}, 
{A1, 500}, 
{F1, 375}, 
{C1, 125}, 
{A1, 1000}, 

};



/*
int main(void)
{
  	buzzer_init();
  	DDRB = 0xFF;
	sei();
	
while(1){

	for (int i=0; i <DJEN_LAENGE; i++)
	{
	buz_freq(Djen[i][0]);
	delay_ms(Djen[i][1]*150);
	}


}


}
*/
