#include <msp430x54x.h>

unsigned int Rcount = 0;
unsigned int Lcount = 0;
unsigned int new_count = 0;
unsigned int old_count;
unsigned char led=0x00;
unsigned char PushDowKey = '0';
unsigned char Num[5][5]=
{'0','0','0','0','0',
'0','0','1','2','3',
'0','4','5','6','7',
'0','8','9','A','B',
'0','C','D','E','F'};

void delaymsec(int msec)
{
  for(int i=0;i<500;i++)
    for(int j=0;j<=msec;j++);
}
/*
///////___Port1.Interrupt___/////////////////
#pragma vector=PORT1_VECTOR
__interrupt void Port1(void)
{
  UCA3TXBUF = led;
}
*/
///////___UARTBUFFER.Interrupt___/////////////////
#pragma vector = USCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
{
  
  ///RX_Buffer is UCA3RXBUF
  ///TX_Buffer is UCA3TXBUF
  if((UCA3RXBUF >= 0x30 && UCA3RXBUF <= 0x39) || (UCA3RXBUF >= 0x41 && UCA3RXBUF <= 0x46))
  {
    while(!(UCA3IFG & UCTXIFG));
      UCA3TXBUF = PushDowKey;
    led = UCA3RXBUF-0x30;
  }
}
/*///////////////////////////////////////////
  UART 傳送格式為 Unicode
////////////////////////////////////////////
*/
///////___Main_Funtion()___/////////////////
void main( void )
{
  WDTCTL = WDTPW+WDTHOLD;
  P9DIR |=0x0F;
  
  delaymsec(200);
  
    ////////___UART___//////////////////           
        P11DIR |= 0x07;
        P11SEL |= 0x07;
        
        P7SEL |= 0x03;
        UCSCTL6 &= ~XT1DRIVE_3;
        UCSCTL6 |= XCAP_3;
        
        UCSCTL0 = 0x00;
        UCSCTL1 = DCORSEL_4;
        
        UCSCTL2 = 244;
        
        P10SEL |= 0x30;
        
        UCA3CTL1 = UCSWRST;
        UCA3CTL0 = UCMODE_0;
        
        UCA3CTL1 |= UCSSEL_2;
        UCA3BR0 = 0x87;         //鮑率=57600
        UCA3BR1 = 0x00;
        UCA3MCTL = 0xB6;
        UCA3CTL1 &= ~UCSWRST;
        UCA3IE |= UCRXIE;         //啟動接收中斷

        __bis_SR_register(GIE);  //開啟全域中斷
    ///////////////////////////////////
  while (1)
  {
    ///////___Keyboard___/////////////////
    P9OUT=0x01;
    for(int lc=1;lc<5;lc++){
      Rcount=(P9IN&0xF0);       //P9IN & 1111 0000
      if(Rcount>0){
        Lcount=lc;
        delaymsec(10);
        switch(Rcount){
          case 16: Rcount=1;
            break;
          case 32: Rcount=2;
            break;
          case 64: Rcount=3;
            break;
          case 128: Rcount=4;
            break; 
        }
       while(P9IN&0xF0);        //等待按鍵彈起
       PushDowKey = Num[Rcount][Lcount];
       new_count = 4*(Rcount-1)+(Lcount-1);
       while(!(UCA3IFG & UCTXIFG));
        UCA3TXBUF = PushDowKey;
       __no_operation();
      }
      P9OUT=0x01<<lc;
    }

    /////////___LED___/////////////////
        P6DIR |=0xFF;
        if(new_count != old_count){
          led=0x00;
          for(int i=0;i<new_count;i++)
              led+=0x01;
          old_count = new_count;
        }
        P6OUT = led;
        delaymsec(200);
    
    ///////___Interrupt___/////////////////
    /*
      P1REN |= 0x80;                        //P1.7致能內部上升電阻
      P1OUT |= 0x80;                        //P1.7為輸出
      P1IE  |= 0x80;                        //P1.7為中斷致能
      P1IES |= 0x80;                        //P1.7選擇下緣觸發
      P1IFG &= ~0x80;                       //P1.7清除旗標(目的是為了確保一開始的旗標絕對為0)
    */
    //////////////////////////////////////
  }
}
