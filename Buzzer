#include <reg51.h>
#include <stdio.h>
#include <string.h>
#include<intrins.h>
sbit BUZZER = P3^4;  // Buzzer pin

void delay_Long(void);
void main(void){
	BUZZER=0;
	BUZZER=1;
	BUZZER=0;
}
void delay_Long(void) {
    unsigned int i;
    for(i=0; i<30000; i++);
}
