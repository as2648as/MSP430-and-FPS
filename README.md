# MSP430 + FPS 練習筆記  

MSP430 + 指紋模組的專題筆記，  
於 `2021.02.26` 開始記錄。  
紀錄人：`ZiLin`  

> # 快速導覽
> > [基礎腳位設定](https://github.com/as2648as/MSP430-and-FPS#%E8%BC%B8%E5%85%A5%E6%9A%AB%E5%AD%98%E5%99%A8-pxin)  
> > [UART 設定](https://github.com/as2648as/MSP430-and-FPS#UART%20%E8%A8%AD%E5%AE%9A)  

<br>

> # 基礎腳位設定  
> > ## 方向暫存器 PxDIR  
> > 選擇 I/O 的輸出入方向，  
> > 例如： `PxDIR = 0xF0`，  
> > `0xF0` 等於二進制的 `1111 0000`，  
> > 則表示 I/O 埠的：  
> > `Px.0` ~ `Px.3` 為輸入  
> > `Px.4` ~ `Px.7` 為輸出  
> 
> > ## 輸入暫存器 PxIN  
> > 若 `PxDIR` 的 I/O 埠為輸入時，  
> > `PxIN` 則可設定對 I/O 埠的儲存資料，  
> > 例如：`PxIN = 0xF0`，  
> > `0xF0` 等於二進制的 `1111 0000`，  
> > 則表示 I/O 埠的輸入資料：  
> > `Px.0` ~ `Px.3` 為LOW  
> > `Px.4` ~ `Px.7` 為HIGH  
> 
> > ## 輸出暫存器 PxOUT  
> > 若 `PxDIR` 的 I/O 埠為輸出時，  
> > `PxOUT` 則可設定對 I/O 埠的輸出資料，  
> > 例如：`PxOUT = 0xF0`，  
> > `0xF0` 等於二進制的 `1111 0000`，  
> > 則表示 I/O 埠的輸出資料：  
> > `Px.0` ~ `Px.3` 為LOW  
> > `Px.4` ~ `Px.7` 為HIGH  
> 
> > ## 功能選擇暫存器 PxSEL
> > 決定是否啟用接腳的複用功能，  
> > 完全理解後補充...  
> > 詳情可見實驗手冊 `4-1`。  
> 
> > ## 上拉、下拉電阻致能暫存器 PxREN
> > 當 `PxDIR` 暫存器設定為輸入時(`=0`)，  
> > `PxREN` 暫存器可設定為關閉內部電阻 (`=0`)，  
> > 或是設定為致能內部電阻(`=1`)，  
> > 若致能內部電阻(`=1`)啟用時，  
> > 可以在 `PxOUT` 暫存器設定：  
> >   1. (`=0`) 啟用下拉電阻  
> >   2. (`=1`) 啟用上拉電阻  
> >
> > 詳情可見實驗手冊 `4-3`。  

<br>

> # UART 設定  
> > ## UART 連接腳位  
> > |     -     |    0 |    1 |    2 |     3 |  
> > |:---------:| ----:| ----:| ----:| -----:|  
> > | UCA ? TXD | P3.4 | P5.6 | P9.4 | P10.4 |  
> > | UCA ? RXD | P3.5 | P5.7 | P9.5 | P10.5 |  
> 
> > ## 使用 `UART` 設定流程：  
> > 1. 將 `UCSWRST` 設定為 `1`, 讓 `USCI_Ax` 處於重置狀態  
> > 2. 設定暫存器參數  
> > 3. 設定腳位  
> > 4. 將 `UCSWRST` 設定為 0  
> > 5. 啟動 `RX/TX` 中斷致能與 `GIE` 中斷  
> >
> > 註：UART 傳送格式為 Unicode  
> 
> > ## UART 鮑率設定的關係
> > `UCSSELx` 設定時脈源後，  
> > `UCAxBR0`、`UCAxBR1` 與 `UCAxMCTL` 的關係：  
> > ```
> > 時脈(8MHz) / 鮑率(57600) = 135.642  
> > ```  
> > 將135填入`BRx`，  
> > `UCAxBR0` 為 `低位元`，`UCAxBR1` 為 `高位元`。  
> > 將 135 轉為 `16位元` 後得到 `0x87` 填入：
> > `UCAxBR0 = 0x87`, `UCAxBR1 = 0x00`