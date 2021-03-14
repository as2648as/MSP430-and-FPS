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
  // 若接收的值在 0-9 (ASCII)
  if ( UCA3RXBUF >= 0x30 && UCA3RXBUF <= 0x39 ) {
    // 等待狀態為可傳送
    while( !( UCA3IFG & UCTXIFG ) );
    // 將PushDownKey的值給予UCA3的UART傳輸
    UCA3TXBUF = PushDownKey;
    // 0 的 ASCII
    // DEC = 48, HEX = 0x30
    // 使用 48 相減後會得到輸入的數值
    num = UCA3RXBUF - 48;
    // 將暫存值改為解出值
    old_count = num;
    new_count = num;
    // 修改LED顯示
    led = num;
  }
  // 若接收的值為 A-F (ASCII)
  else if ( UCA3RXBUF >= 0x41 && UCA3RXBUF <= 0x46 ) {
    // 等待狀態為可傳送
    while( !( UCA3IFG & UCTXIFG ) );
    // 將PushDownKey的值給予UCA3的UART傳輸
    UCA3TXBUF = PushDownKey;
    // A 的 ASCII
    // DEC = 65, HEX = 0x41
    // 使用 55 相減後會得到輸入的數值
    num = UCA3RXBUF - 55;
    // 將暫存值改為解出值
    old_count = num;
    new_count = num;
    // 修改LED顯示
    led = num;
  }
}

void main( void ) {
  // 停止看門狗
  WDTCTL = WDTPW + WDTHOLD;

  // 將P9鍵盤腳位設定輸出入方向
  P9DIR |= 0x0F;
  
  // 等待200ms
  delaymsec(200);

  // LED 輸出設定
  P6DIR |= 0xFF;

  ///////////////////////////////////////////////////
  // 
  // 使用 UART 設定流程：
  // 1 將 UCSWRST 設定為 1, 讓USCI_Ax 處於重置狀態
  // 2 設定暫存器參數
  // 3 設定腳位
  // 4 將 UCSWRST 設定為 0
  // 5 啟動 RX/TX 中斷致能與 GIE 中斷
  //
  // 註：UART 傳送格式為 Unicode
  //
  ///////////////////////////////////////////////////

  // UART 前置設定
  P7SEL |= 0x03;
  UCSCTL6 &= ~XT1DRIVE_3;
  UCSCTL6 |= XCAP_3;

  UCSCTL0 = 0x00;
  UCSCTL1 = DCORSEL_4;
  UCSCTL2 = 244;

  // 設定P10 UART腳位
  P10SEL |= 0x30;
  UCA3CTL1 = SWRT;
  UCA3CTL0 = UCMODE_0;
  UCA3CTL1 |= UCSSEL_2;

  // 鮑率設定 (57600)
  UCA3BR0 = 0x87;
  UCA3BR1 = 0x00;
  UCA3MCTL = 0xB6;

  UCA3CTL1 &= ~SWRT;

  UCA3IE |= UCRXIE;  // 啟動接收中斷致能

  __bis_SR_register(GIE);  // 開啟全域中斷

  // UART 設定結束,
  // 開始 接收/讀取 按鍵迴圈
  while ( true ) {
    // 鍵盤掃描
    P9OUT = 0x01;  // 將P9輸出設定為 0000 0001
    for ( int lc = 1; lc < 5; lc++ ) {  // 註： lc、Lcount 為行, Rcount 為列
      Rcount = ( P9IN & 0xF0 );  // 此時按鍵輸入為 xxxx 0000
      // 若有按下按鍵, 則 Rcount 大於 0
      if ( Rcount > 0 ) {
        Lcount = lc;  // 設定行
        delaymsec(10);
        // 將鍵盤讀取值轉換為列
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
        while( P9IN & 0xF0 );  // 等待使用者放開按鍵
        PushDownKey = Num[Rcount][Lcount];  // 設定PushDownKey
        // 等待UART狀態為可傳送
        // 此處為檢察中斷是否發生
        // 若發生的話, 則迴圈將持續
        while( !( UCA3IFG & UCTXIFG ) );
        UCA3TXBUF = PushDownKey;  // 將PushDownKey的值給予UCA3的UART傳輸
        new_count = 4 * ( Rcount - 1 ) + ( Lcount - 1 );  // 計算該按鍵數值
        __no_operation();  // 等待
      }
      P9OUT = 0x01 << lc;  // 更換掃描腳位
    }

    // LED 設定
    if ( new_count != old_count ) {  // 若狀態被改變
      led = 0x00;  // LED 初值設定
      for ( int i = 0; i < new_count; i++ ) {
          led += 0x01;  // led 變數累加 count 數
      }
      // 更新狀態
      old_count = new_count;
    }
    // 將 LED 燈號輸出
    P6OUT = led;
    delaymsec(200);
  }  // while end
}  // main end

// delay 函式
void delaymsec(int msec) {
  for(int i=0;i<500;i++) {
    for(int j=0;j<=msec;j++);
  }
}