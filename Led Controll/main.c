#include "8052.h"

// Define 8 LEDs
#define led1  P1_0
#define led2  P1_1
#define led3  P1_2
#define led4  P1_3
#define led5  P1_4
#define led6  P1_5
#define led7  P1_6
#define led8  P1_7
int effect = 1;
int oldeffect = 1;
// Tre 0- 65536
void delay(unsigned int time)
{
	while(time--){
        if(oldeffect!=effect){
			//Neu ma hieu ung sau khac hieu ung truoc ngung delay de ngung hien thi
            oldeffect = effect;
            return;
        }
	}
}

//Effect1:
void RunEffect1(int count,int *reverse)
{
	P1 = 0xff;//Tat tat ca led
	//Dua vao so lan dem chon lan luot cac cap led sang
	switch(count%8)
	{
		case 0:
			P1 = 0xff;
			(*reverse) = ((*reverse)+1)%2;
			break;
		case 1:
			if(*reverse == 0)//led1 led2 sang
			{
				led1 = 0;
				led2 = 0;
			}
			else//led led8 sang cho truong hop con lai
			{
				led8 = 0;
				led7 = 0;
			}
			break;
		case 2:
			if(*reverse == 0)//Tuong tu voi truong hop led1 led2
			{
				led2 = 0;
				led3 = 0;
			}
			else//Tuong tu voi truong hop led1 led2
			{
				led7 = 0;
				led6 = 0;
			}
			break;
		case 3:
			if(*reverse == 0)//Tuong tu voi truong hop led1 led2
			{
				led3 = 0;
				led4 = 0;
			}
			else//Tuong tu voi truong hop led1 led2
			{
				led6 = 0;
				led5 = 0;
			}
			break;
		case 4:
			if(*reverse == 0)//Tuong tu voi truong hop led1 led2
			{
				led4 = 0;
				led5 = 0;
			}
			else//Tuong tu voi truong hop led1 led2
			{
				led5 = 0;
				led4 = 0;
			}
			break;
		case 5:
			if(*reverse == 0)//Tuong tu voi truong hop led1 led2
			{
				led5 = 0;
				led6 = 0;
			}
			else//Tuong tu voi truong hop led1 led2
			{
				led4 = 0;
				led3 = 0;
			}
			break;
		case 6:
			if(*reverse ==0)//Tuong tu voi truong hop led1 led2
			{
				led6 = 0;
				led7 = 0;
			}
			else//Tuong tu voi truong hop led1 led2
			{
				led3 = 0;
				led2 = 0;
			}
			break;
		case 7:
			if(*reverse == 0)//Tuong tu voi truong hop led1 led2
			{
				led7 = 0;
				led8 = 0;
			}
			else//Tuong tu voi truong hop led1 led2
			{
				led2 = 0;
				led1 = 0;
			}
			break;
	}
}

//Effect2:
void Display(){
    P1=~P1;//Dao nguoc cac bit cua cac led tu led1 toi led 8 de hien thi cho dung
    delay(10000);
    P1=~P1;//Dao nguoc lai de de tinh toan
}


void Effect2(int time){
    while(time--){
            if(effect!=2){//Ket thuc hieu ung neu xay ra ngat
                return;
            }
            P1 = 0;
            int j = 7,k = 0;
            for(; j>=0; j--){//Bat lat luot cac led roi don lai su dung cong don
                int OldValue = P1;
                for(k = 0; k <=j; k++){
                    P1 += (1<<k);//Cho led chay tu led1 -> led dang xet
                    Display();//Hien thi ra led
					if(effect!=2){//Ket thuc hieu ung neu xay ra ngat
						return;
					}
                    P1 = OldValue;//Khoi phuc lai gia tri cho P1
                }
                P1 += (1<<j);//Cong don cac led lai
            }
            j = 7;
//Lan luot tat cac led tu led8 -> led1 su dung dich bit
            for(;j>=0;j--){
                P1 = P1 >> 1;
                Display();
            }
            if(effect!=2){//Ket thuc hieu ung neu xay ra ngat
                return;
	    }
    }
}
void Effect1(int time){
		int count = 0;
		int reverse = 1;
		//Reset, turn all led of
		P1 = 0xff;
		delay(10000);
        if(effect!=1){//Ket thuc hieu ung neu xay ra ngat
            return;
        }
		//Repeat
		while(count<=((time*16)-1)&&effect==1)
		{
			RunEffect1(count,(&reverse));//Goi den ham thuc thi Effect1
			count++;
			delay(10000);
        if(effect!=1){//Ket thuc hieu ung neu xay ra ngat
            return;
        }
		}

}
//Effect3:
void Effect3(int time)
{
    if(effect!=3){//Ket thuc hieu ung neu xay ra ngat
        return;
    }
	while(time--){
		int count = 0;
		while(count!=9){//Su dung 9 th de bieu dien led moi lan tang truong hop len 1
			switch(count)
			{
				case 0:
					P1 = 0x7e;
					break;
				case 1:
					P1 = 0xbd;
					break;
				case 3:
					P1 = 0xdb;
					break;
				case 4:
					P1 = 0xe7;
					break;
				case 5:
					P1 = 0xdb;
					break;
				case 6:
					P1 = 0xbd;
					break;
				case 7:
					P1 = 0x7e;
					break;
				case 8:
					P1 = 0xff;
					break;
			}
			delay(10000);
            if(effect!=3){//Ket thuc hieu ung neu xay ra ngat
                return;
	    }
			count++;//Tang bien count len 1 de chuyen hieu ung cho effect 3
		}
	}
}
void init(){//Khoi tao
	P3_2 = 1;//Set chan 3.2 la dau vao
	P3_3 = 1;//Set chan 3.3 la dau vao
    EX0 = 1;//Mo ngat ngoai cho 3.2
    EX1 = 1;//Mo ngat ngoai cho 3.3
    IT1 = 1;//Ngat theo suong am
    IT0 = 1;//Ngat theo suong am
    EA = 1;//Mo ngat tong
	effect = 1;//Set hieu ung ban dau la effec1
	oldeffect = 1;//Set hieu ung ban dau la effec1
}
void main(void)
{
    init();
	while(1){//Chon hieu ung xay ra
        if(effect==1){
			delay(5000);//Tre 1 xiu de de quan sat chuyen hieu ung
            Effect1(10);//Lap lai hieu ung 10 lan
        }
        else if(effect==2){
			delay(5000);//Tre 1 xiu de de quan sat chuyen hieu ung
            Effect2(10);//Lap lai hieu ung 10 lan
        }
        else{
			delay(5000);//Tre 1 xiu de de quan sat chuyen hieu ung
            Effect3(10);//Lap lai hieu ung 10 lan
        }
	}
}

//Ham su ly ngat cho P3_2
void EXT0_Process() __interrupt IE0_VECTOR{
    EA=0;    //Cam ngat
	effect--;
	if(effect<1){//Neu hieu ung nho hon 1 thi khong cho chuyen hieu ung
        effect=1;
	}
    EA=1;    //Cho phep ngat
}



//Ham su ly ngat cho P3_3
void EXT1_Process() __interrupt IE1_VECTOR{
    EA=0;    //Cam ngat
	effect++;
	if(effect>3){//Neu hieu ung lon hon 3 thi khong cho chuyen hieu ung
        effect=3;
	}
    EA=1;    //Cho phep ngat
}
