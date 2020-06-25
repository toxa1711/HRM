#include "main.h"


#define PLL_M      16
#define PLL_N      336

/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      2

/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
#define PLL_Q      7

int SetupClock(){


	return 0;
}

int DelayMs(int data){
	for(int i = 0; i < data; i++){
		i++;
	}
	return 0;
}

int Delay(int data){
	for(int i = 0; i < data; i++){
		i++;
	}
	return 0;
}


void SB(uint8_t Data, int DR){ //Data = 1 Register = 0;
	if(DR == 1) {GPIO_SetBits(GPIOD, AOPin);}
	else GPIO_ResetBits(GPIOD, AOPin);

	SendViaSpi(Data);
}

void SW(uint16_t Data, int DR){ //Data = 1 Register = 0;
	if(DR == 1) {GPIO_SetBits(GPIOD, AOPin);}
	else GPIO_ResetBits(GPIOD, AOPin);

	SendViaSpi(Data >> 8);
	SendViaSpi(Data);
}

int SendViaSpi(unsigned char data){
	GPIO_ResetBits(GPIOD, CSPin);//DigitalWrite(CS_PIN, LOW);
		    int i;

		      for(i=0; i<8; i++){

		         if (data & 0x80){
		        	 GPIO_SetBits(GPIOD, DatPin);//DigitalWrite(DIN_PIN, 1); //SPI_SDI = 1;
		         }
		         else{
		        	 GPIO_ResetBits(GPIOD, DatPin);//DigitalWrite(DIN_PIN, 0); //SPI_SDI = 0;
		         }

		         GPIO_SetBits(GPIOD, ClkPin);//DigitalWrite(CLK_PIN, 1); //SPI_SCK = 1;

		         data <<= 1;
		         //DelayMs(1);

		         GPIO_ResetBits(GPIOD, ClkPin);//DigitalWrite(CLK_PIN, 0); //SPI_SCK = 0;
		      }
		      GPIO_SetBits(GPIOD, CSPin);//DigitalWrite(CS_PIN, HIGH);
	return 0;
}

int startup(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef G;

	G.GPIO_Pin = CSPin | ResPin | AOPin | DatPin | ClkPin | GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;
		G.GPIO_Mode = GPIO_Mode_OUT;
		G.GPIO_OType = GPIO_OType_PP;
		G.GPIO_PuPd = GPIO_PuPd_NOPULL;
		G.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOD, &G);

		GPIO_SetBits(GPIOD, GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12);


		GPIO_ResetBits(GPIOD, CSPin | ResPin | AOPin | DatPin | ClkPin);

		Delay(2000);

		GPIO_ResetBits(GPIOD, GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12);

		GPIO_ResetBits(GPIOD, ResPin);
		Delay(200);
		GPIO_SetBits(GPIOD, ResPin);
		Delay(200);
		GPIO_ResetBits(GPIOD, CSPin);

	return 0;
}

void SetAddr(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2){
	SB(0x2A, Reg);
	SB(0x00, Dat);
	SB(X1, Dat);
	SB(0x00, Dat);
	SB(X2, Dat);
	SB(0x2B, Reg);
	SB(0x00, Dat);
	SB(Y1, Dat);
	SB(0x00, Dat);
	SB(Y2, Dat);

	SB(0x2C, Reg);
}

void SetScrn(Colours8 Colour){
	uint8_t XCnt = 0;
	uint8_t YCnt = 0;

	SetAddr(0, 0, XPix-1, YPix-1);

	for(XCnt = 0; XCnt<XPix; XCnt++){
		for(YCnt = 0; YCnt<YPix; YCnt++){
			SW(Colour, Dat);
		}
	}
}

void ClrScrn(void){
	SetScrn(Black);
}

void WritePix(uint16_t X, uint16_t Y, Colours8 Colour){
	SetAddr(X, Y, X, Y);
	//PCol(Colour);
	SW(Colour, Dat);
}

void SleepMode(uint8_t Mode){
	if(Mode == Sleep) SB(0x10, Reg);
	else SB(0x11, Reg);
	Delay(120);
}

void InvMode(uint8_t Mode){
	if(Mode==0) {SB(0x20, Reg);}
	else SB(0x21, Reg);
}

int main(void)
{



	startup();

	SB(0x01, Reg); //Software reset
		SB(0x11, Reg); //Exit Sleep


		SB(0x26, Reg); //Set default gamma
		SB(0x04, Dat);

		SB(0xC0, Reg); //Set Power Control 1
		SB(0x1F, Dat);

		/*SB(0xC1, Reg); //Set Power Control 2
		SB(0x00, Dat);

		SB(0xC2, Reg); //Set Power Control 3
		SB(0x00, Dat);
		SB(0x07, Dat);

		SB(0xC3, Reg); //Set Power Control 4 (Idle mode)
		SB(0x00, Dat);
		SB(0x07, Dat);

		SB(0xC5, Reg); //Set VCom Control 1
		SB(0x24, Dat); // VComH = 3v
		SB(0xC8, Dat); // VComL = 0v*/

		SB(0x38, Reg); //Idle mode off
		//SB(0x39, Reg); //Enable idle mode

		SB(0x3A, Reg); //Set pixel mode
		SB(0x05, Dat);

		/*SB(0x36, Reg); //Set Memory access mode
		SB(0x08, Dat);*/

		SB(0x29, Reg); //Display on

		InvMode(0);



		/*InvMode(0);
				for(int i = 0;i < 100;i++){
					WritePix(i,i,White);
				}*/

		GPIO_SetBits(GPIOD, GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12);


		    RCC_ClocksTypeDef clocks;
		    	RCC_GetClocksFreq(&clocks);
	while(1){



		//SetScrn(Blue);

		//SetScrn(Red);

		for(int i = 0; i < 128; i++){
			WritePix(i,i,Red);
			WritePix(i-1,i-1,Blue);
		}
    	for(int i = 0;i < 128;i++){
    				WritePix(i,i,White);
    			}/*
		GPIO_ResetBits(GPIOD, GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12);
		for(int i = 0;i < 100;i++){
		    				WritePix(i,i,Red);
		    			}
		GPIO_SetBits(GPIOD, GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12);*/

		/*GPIO_ResetBits(GPIOD, GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12);
		Delay(16000000);
		GPIO_SetBits(GPIOD, GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12);
		Delay(clocks.SYSCLK_Frequency);*/

    }


}

