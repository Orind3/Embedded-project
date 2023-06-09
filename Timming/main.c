#include <mcs51/8052.h>
#define TH0_50ms 0x3C//Voi TH va TL co gia tri nhu tren ket hop voi tan so cua AT89S52 la 12Mhz
#define TL0_50ms 0xB0//se tao ra khoang thoi gian giua 2 lan kich hoat ngat la 50ms



//global var
unsigned int second=0;
unsigned int const mask[10]={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
//Gia tri nay tuong ung voi gia tri 0,1,2,3,4,5,6,7,8,9 tren led 7 thanh


//Ham tao do tre nay de de dang quan sat hieu ung cua led 7 thanh
void delay(int interval)
{
    int i,j;
    for(i=0;i<100;i++)
    {
        for(j=0;j<interval;j++);
    }
}

//Hien thi gia tri value len led 7 thanh
void output_7seg(unsigned int value)
{
    if(value < 10){
        P1 = mask[value];
    }
}

//Set thoi gian cua bo dem nguoc la 0s va bo dem chuong la 1s)
unsigned int timmer = 0;
unsigned int buzzertimer = 1;
//Timer 0 interrupt handler
void TMR0_Process() __interrupt TF0_VECTOR{
    TF0  = 0;             //Xoa co bao tran
    TR0  = 0;             //Tam dung bo dinh thoi
    TH0  = TH0_50ms;    //Set thanh ghi TH va TL ve gia tri ban dau
    TL0  = TL0_50ms;
    if(timmer>0){
        timmer--;//Giam bien timmer di 1
    }
    if(buzzertimer>0){
        buzzertimer--;//Giam bien buzzertimmer di 1
    }
    if(buzzertimer<=20*5){//Het thoi gian timmer chuong keu 3 lan
        if(buzzertimer%20==0){
            P3_6 = !P3_6;
        }
    }
    TR0  = 1;//Bat lai bo dem thoi gian
    if(buzzertimer==0){
        TR0 = 0;//Ket thuc bo dem thoi gian
    }
}



//Ngat ngoai btn 3_2
void EXT0_Process() __interrupt IE0_VECTOR{
    EA=0;    //Cam ngat
    if(TR0 == 0){
        TR0 = 1;//Bat bo dem thoi gian
    }
    else{
        TR0 = 0;//Tat bo dem thoi gian
    }
    EA=1;    //Cho phep ngat
}



//EXT 1 interrupt handler
void EXT1_Process() __interrupt IE1_VECTOR{
    TR0 = 0;
    EA=0;    //Cam ngat
    timmer = 20 * 5 * 60;//Chinh bo dem thoi gian 5 phut
    buzzertimer = timmer + 20 * 5;//Setup thoi gian chuong bao
    EA=1;    //Cho phep ngat
}



//Hien thi 4 chu so phutphut.giaygiay
void display_number(unsigned int value){
    unsigned int temp;
    temp = value%60;
    P2 = 0b00001000;//Su dung P2_3 tuong ung voi led so 4 de hien thi giay
    output_7seg(temp%10);//Hien thi giay tren led 7 thanh
    delay(1);//Tre 1 thoi gian de co the quan sat tren led 7 thanh
    temp = temp/10;
    P2 = 0b00000100;//Su dung P2_2 tuong ung voi led so 3 de hien thi giay
    output_7seg(temp);//Hien thi giay tren led 7 thanh
    delay(1);//Tre 1 thoi gian de co the quan sat tren led 7 thanh
    temp = value/60;
    P2 = 0b00000010;//Su dung P2_1 tuong ung voi led so 2 de hien thi phut
    output_7seg(temp%10);
    P1 = P1 & 0b01111111;//Bat dau cham ngan cach giua phut va giay
    delay(1);//Tre 1 thoi gian de co the quan sat tren led 7 thanh
    temp = temp/10;
    P2 = 0b00000001;//Su dung P2_0 tuong ung voi led so 1 de hien thi phut
    output_7seg(temp);
    delay(1);//Tre 1 thoi gian de co the quan sat tren led 7 thanh
}

//system initialization
void init(){
    P3_6 = 0;             //Tat loa
    P3_2 = 1;             //Thiet lap chan P3_2 lam chan vao
    P3_3 = 1;             //Thiet lap chan P3_3 lam chan vao
    //Setup cho ngat ngoai so 0
    EX0  = 1;             //Cho phep ngat ngoai 0
    IT0  = 1;            //Ngat theo suon
    //Setup cho ngat ngoai so 1
    EX1  = 1;             //Cho phep ngat ngoai 1
    IT1  = 1;            //Ngat theo suon
    EA   = 1;            //global interrupt enable


    //timer 0
    TMOD = TMOD & 0xF0;
    TMOD = TMOD | 0x01;
    TH0  = TH0_50ms;        //Khoi tao T0
    TL0  = TL0_50ms;        //Tuong duong 15536
    TF0  = 0;           //Xoa co tran timer 0
    TR0  = 0;           //Khoi dong timer 0
    ET0  = 1;           //cho phep ngat timer 0
}

unsigned int end = 0;

void main(){
    init();//Goi ham khoi toa
    while(1){
        second = timmer/20;//Vi timmer = 50ms => second = timmer/20;
        display_number(second);//Hien thi seccond tren led 7 thanh
    }
}
