int i;
int  adcX;
int  adcY =0;
int  adcZ;
char x1[3];
char y1[3];
char z1[3];
int  S1;
int  S2;
int  S3;
char SS1[3];
char SS2[3];
char SS3[3];
int delayCntr=0;
int TMRO=0;
//////////////ultrasonic///////////////////////////////
sbit TRIG at RC3_bit;

sbit TRIG_Direction at TRISC3_bit;

sbit ECHO at RC2_bit;

sbit ECHO_Direction at TRISC2_bit;
///////////////////////////////////////////////////////
int dist=0,cnt ;

unsigned char txt[15];
//////////////////////////////////////////////////////////////////////////
#define KP 1.0    // ????? ???????
#define KI 0.1    // ????? ???????
#define KD 0.01   // ????? ???????

int setpoint = 0; // ????? ???????? ??? ???? X
int error, integral, derivative, last_error;
int motor_speed;

void PID_Controller() {
    int current_position = 55;


    error = setpoint - current_position;


    integral += error;


    derivative = error - last_error;


    motor_speed = KP * error + KI * integral + KD * derivative;


    last_error = error;


    //Update_Motor_Speed(motor_speed);
}
////////////////////////////////////////
void ADCInit(void){
     ADCON1 = 0xC0; // right justified, all channels are analog
     TRISE= 0x0F; // Port E is input
     ADCON0 = 0x41; //prescale 16, channel3, dont start conversion, power on ATD
}

   unsigned int ADC_read0(void){
ADCON0 = 0x41;
ADCON0 = ADCON0 | 0x04;     //AN0
     while((ADCON0 & 0x04)); // wait until the GO/DONE bit is reset, the ATD reading is ready
return ((ADRESH<<8)|(ADRESL));
 }

void mymsDelay(unsigned int d){
delayCntr=0;
OPTION_REG=0x87;
TMR0=247;
INTCON=INTCON | 0x20;
while(delayCntr<d);
INTCON=INTCON & 0xDF;
}


void interrupt() {

     if (INTCON.INTF == 1){         // detect rising edge

       if (OPTION_REG.INTEDG ==1){

           T1CON.TMR1ON = 1;      // put on timer1

           OPTION_REG.INTEDG = 0; // set interrupt on falling edge

           }

       else if (OPTION_REG.INTEDG == 0){ // detect falling edge

             T1CON.TMR1ON = 0;           // put off timer1

             dist =((65536*cnt)+(TMR1L | (TMR1H<<8)))*0.0034;

             TMR1L=0;   // clear timer1L

             TMR1H=0;   // clear timer1H

             cnt=0;   // clear cnt variable

             OPTION_REG.INTEDG = 1; // set interrupt on rising edge

             }

       INTCON.INTF = 0;



       }

       else if (PIR1.TMR1IF == 1){   // timer1 overflow

       cnt++;

       PIR1.TMR1IF =0;

         }
if ( INTCON & 0x04) {
TMRO=247;
delayCntr++;
}
}


void forward(){
    PORTB=PORTB|0x02;
    PORTB=PORTB & ~0x08;
    if((PORTB & 0x02) == 0x02 & (PORTB & 0x08) == 0){
      for(i=0;i<200;i++){
        PORTB=PORTB|0x04;
        PORTB=PORTB|0x10;
        mymsDelay(700);
        PORTB=PORTB & ~0x04;
        PORTB=PORTB & ~0x10;
        mymsDelay(700);

      }

    }

}

void back(){
    PORTB=PORTB & ~0x02;
    PORTB=PORTB|0x08;
    if((PORTB & 0x02) == 0 & (PORTB & 0x08) == 0x08){
      for(i=0;i<200;i++){
        PORTB=PORTB|0x04;
        PORTB=PORTB|0x10;
        mymsDelay(700);
        PORTB=PORTB & ~0x04;
        PORTB=PORTB & ~0x10;
        mymsDelay(700);
      }

    }
}
void stop(){
  PORTB=PORTB & ~0x04;
  PORTB=PORTB & ~0x10;
  PORTB=PORTB & ~0x02;
  PORTB=PORTB & ~0x08;

}
void balance() {

     if (adcY >= 340 ){
          UART1_Write_Text("forward");
          UART1_Write(13);     // Start a new line
          UART1_Write(10);
          forward();
        }
        if (adcY <= 310 ){
          UART1_Write_Text("backworld");
          UART1_Write(13);     // Start a new line
          UART1_Write(10);
          back();
        }
        if (adcY > 310  && adcY < 340 ){
          UART1_Write_Text("Stop");
          UART1_Write(13);     // Start a new line
          UART1_Write(10);
          stop();
        }

}

void main(){
 PID_Controller();
  UART1_Init(9200); // Initialize USART module
  ADCInit();

  OPTION_REG.INTEDG = 1; //INTERRUPT ON RISING EDGE
  T1CON = 0x09;     // timer control register
  INTCON = 0xD0;   // interrupt control register
  PIE1.TMR1IE=1;   // timer1 interrupt enable bit
  T1CON.TMR1ON = 0; // timer1 0ff initially
  trisb0_bit=1;
  trisc=0;
  portc=0;
  TRISB=TRISB & ~0x04;//step
  PORTB=PORTB & ~0x04;
  TRISB=TRISB & ~0x10;
  PORTB=PORTB & ~0x10;
  PORTB=PORTB|0x01;
  TRISB=TRISB | 0x02;
  mymsDelay(20);
  TRISB=TRISB & ~0x02;//Diriction
  PORTB=PORTB & ~0x02;
  TRISB=TRISB & ~0x08;
  PORTB=PORTB & ~0x08;
  mymsDelay(20);
  i=0;/*
  UART1_Write_Text("PIC16F877A UART example");
  UART1_Write(13);     // Start a new line
  UART1_Write(10);   */
  while (1){
        Inttostr(dist,txt);

        Ltrim(txt);

          /*
       UART1_Write_Text(txt);
       UART1_Write(13);
       UART1_Write(10);
             */

        TRIG=1;

        mymsDelay(10);

        TRIG=0;

        mymsDelay(50);
        adcX = ADC_Read(0);
        adcY = ADC_Read(1);
        S1 = ADC_Read(5);
        S2 = ADC_Read(6);
        S3 = ADC_Read(7);

        /*IntToStr(adcY, y1);
       UART1_Write_Text(y1);

        UART1_Write(13);
       UART1_Write(10);*/
        if  ((PORTB & 0x01) == 0x01){
           if (dist<15){
             back();
           }else{
              balance();
           }
        }
        if  ((PORTB & 0x02) == 0x02){
          if(S2>900){
            forward();
          }else{
             balance();
          }
        }else{


            balance();

        }



  }
}
