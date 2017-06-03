#include<msp430g2553.h>
#include<legacymsp430.h>
#define SCL BIT6
#define SDA BIT7

 void init_I2C();
void Receive(unsigned int slave_address, unsigned char data[], unsigned int len);

unsigned int slave_address = 0x68;
unsigned char data[6];
unsigned int len=6;


int main(void){

	WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer
	BCSCTL1 = CALBC1_1MHZ;					//
	DCOCTL = CALDCO_1MHZ;						//
	TA0CCR0 = 100 -1;							//contador de 16 bits
	TA0CTL = TASSEL_2 + ID_0 + MC_1 + TAIE;//modo smclk + dividido por 1 + mod de contagem up + habilita interrupção
	init_I2C();
	_BIS_SR(GIE + LPM0_bits);
	return 0;
}

interrupt(TIMER0_A1_VECTOR) TA0_ISR1(void){
	Receive(slave_address, data, len);
  TA0CTL &= ~TAIFG;
}

void init_I2C()
 {
 	UCB0CTL1 |=	UCSWRST;                      // Enable SW reset
 	UCB0CTL0 = 	UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
 	UCB0CTL1 = 	UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
 	UCB0BR0  = 	10;                             // fSCL = SMCLK/10 = 100kHz
 	UCB0BR1  = 	0;
 	P1SEL    |= SCL + SDA;                     	// Assign I2C pins to USCI_B0
 	P1SEL2   |= SCL + SDA;                     	// Assign I2C pins to USCI_B0
 	UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
}


void Receive(unsigned int slave_address, unsigned char data[], unsigned int len)
 {
 	volatile unsigned int i;
 	UCB0I2CSA = slave_address;
 	while(UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
 	UCB0CTL1 &= ~UCTR ;                     // Clear UCTR
 	UCB0CTL1 |= UCTXSTT;                    // I2C start condition
 	while(UCB0CTL1 & UCTXSTT);             // Start condition sent?
 
 	for(i=0; i<len; i++)
 	{
 		if((len-i)==1)
 			UCB0CTL1 |= UCTXSTP;
 		while((IFG2 & UCB0RXIFG)==0);
 		data[i] = UCB0RXBUF;
 	}
 	while(UCB0CTL1 & UCTXSTP);
 }
