#include <msp430x54x.h>
#include "FPS.h"

unsigned int Rcount = 0;
unsigned int Lcount = 0;
unsigned int new_count = 0;
unsigned int old_count;
unsigned char led = 0x00;
unsigned char PushDownKey = '0';
unsigned char Num[5][5] = {
  '0','0','0','0','0',
  '0','0','1','2','3',
  '0','4','5','6','7',
  '0','8','9','A','B',
  '0','C','D','E','F'
};

void delaymsec( int msec );

#pragma vector = USCI_A3_VECTOR
__interrupt void USCI_A3_ISR( void ) {
  unsigned int num;

  if ( UCA3RXBUF >= 0x30 && UCA3RXBUF <= 0x39 ) {
    while( !( UCA3IFG & UCTXIFG ) );
    UCA3TXBUF = PushDownKey;
    num = UCA3RXBUF - 48;
    old_count = num;
    new_count = num;
    led = num;
  }

  else if ( UCA3RXBUF >= 0x41 && UCA3RXBUF <= 0x46 ) {
    while( !( UCA3IFG & UCTXIFG ) );
    UCA3TXBUF = PushDownKey;
    num = UCA3RXBUF - 55;
    old_count = num;
    new_count = num;
    led = num;
  }

}

void main( void ) {
  WDTCTL = WDTPW + WDTHOLD;
  P9DIR |= 0x0F;
  delaymsec(200);
  P6DIR |= 0xFF;
  P7SEL |= 0x03;
  UCSCTL6 &= ~XT1DRIVE_3;
  UCSCTL6 |= XCAP_3;
  UCSCTL0 = 0x00;
  UCSCTL1 = DCORSEL_4;
  UCSCTL2 = 244;
  P10SEL |= 0x30;
  UCA3CTL1 = SWRT;
  UCA3CTL0 = UCMODE_0;
  UCA3CTL1 |= UCSSEL_2;
  UCA3BR0 = 0x87;
  UCA3BR1 = 0x00;
  UCA3MCTL = 0xB6;
  UCA3CTL1 &= ~SWRT;
  UCA3IE |= UCRXIE;  

  __bis_SR_register(GIE);  

  while ( true ) {
    P9OUT = 0x01;  
    for ( int lc = 1; lc < 5; lc++ ) {  
      Rcount = ( P9IN & 0xF0 );  
      if ( Rcount > 0 ) {
        Lcount = lc;  
        delaymsec(10);
        switch( Rcount ) {
          case 16: Rcount = 1;
            break;
          case 32: Rcount = 2;
            break;
          case 64: Rcount = 3;
            break;
          case 128: Rcount = 4;
            break; 
        }
        while( P9IN & 0xF0 );  
        PushDownKey = Num[Rcount][Lcount];  
        while( !( UCA3IFG & UCTXIFG ) );
        UCA3TXBUF = PushDownKey;  
        new_count = 4 * ( Rcount - 1 ) + ( Lcount - 1 );  
        __no_operation();  
      }
      P9OUT = 0x01 << lc;  
    }

    if ( new_count != old_count ) {  
      led = 0x00;  
      for ( int i = 0; i < new_count; i++ ) {
          led += 0x01;  
      }
      old_count = new_count;
    }

    P6OUT = led;
    delaymsec(200);
  }
}

void delaymsec(int msec) {
  for(int i=0;i<500;i++) {
    for(int j=0;j<=msec;j++);
  }
}