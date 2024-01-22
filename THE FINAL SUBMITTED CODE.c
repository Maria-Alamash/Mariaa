int i;
int blan=218; // Set Point for accelerometer
int   adcX;  ///This for X axis in accelerometer accelerometer value
int   adcY ; ///This for Y axis in accelerometer accelerometer value
int   adcZ;  ///This for Z axis in accelerometer accelerometer value



int  S1;  // Right LDR
int  S2;  // Mid LDR
int  S3;  // Left LDR

int delayCntr=0; //For delay function
int TMRO=0;


///////////////////////////////////////////////////////
int dist=0;
int dist1=0;
int dist2=0;

//////////////////////////////////////
//Delay function


void mymsDelay(unsigned int d){
delayCntr=0;
OPTION_REG=0x87;
TMR0=247;
INTCON=INTCON | 0x20;
while(delayCntr<d);
INTCON=INTCON & 0xDF;
}


////////////////////////////////////////
int UltraSonicBack() {

    int dist=0;
TMR1H = 0;                  //Sets the Initial Value of Timer
TMR1L = 0;                  //Sets the Initial Value of Timer

RC0_BIT = 1;               //TRIGGER HIGH SEND THE PULSE
mymsDelay(0.01);               //10uS Delay
RC0_BIT = 0;               //TRIGGER LOW STOP PULSE

while(!RC1_BIT);           //Waiting for Echo

T1CON.F0 = 1;               //Timer Starts
while(RC1_BIT);            //Waiting for Echo goes LOW
T1CON.F0 = 0;               //Timer Stops

dist = (TMR1L | (TMR1H<<8));   //Reads Timer Value
dist = dist/58.82;                //Converts Time to Distance
return dist;

}


int UltrasonicFront() {

    int dist=0;
TMR1H = 0;                  //Sets the Initial Value of Timer
TMR1L = 0;                  //Sets the Initial Value of Timer

RC2_BIT = 1;               //TRIGGER HIGH SEND THE PULSE
mymsDelay(0.01);               //10uS Delay
RC2_BIT = 0;               //TRIGGER LOW STOP PULSE

while(!RC3_BIT);           //Waiting for Echo

T1CON.F0 = 1;               //Timer Starts
while(RC3_BIT);            //Waiting for Echo goes LOW
T1CON.F0 = 0;               //Timer Stops

dist = (TMR1L | (TMR1H<<8));   //Reads Timer Value
dist = dist/58.82;                //Converts Time to Distance
return dist;

}

void forward(){
     PORTB=PORTB|0x02;
     PORTB=PORTB & ~0x08;


    if((PORTB & 0x02) == 0x02 & (PORTB & 0x08) == 0){
      for(i=0;i<200;i++){
        PORTB=PORTB|0x04;
        PORTB=PORTB|0x10;
        //Delay_us(speed);
        mymsDelay(1);
        PORTB=PORTB & ~0x04;
        PORTB=PORTB & ~0x10;
       // Delay_us(speed);
        mymsDelay(1);
      }

    }

}

void back(){
    PORTB=PORTB & ~0x02;
    PORTB=PORTB|0x08;
    if(PORTB.F1==0 & PORTB.F3==1){
      for(i=0;i<200;i++){
        PORTB=PORTB|0x04;
        PORTB=PORTB|0x10;
        //Delay_us(speed);
        mymsDelay(1);
        PORTB=PORTB & ~0x04;
        PORTB=PORTB & ~0x10;
        //Delay_us(speed);
        mymsDelay(1);

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

     if (adcY >= blan+20 ){

          forward();
        }
        if (adcY <= blan-20 ){

          back();
        }
        if (adcY > blan+20  && adcY < blan-20 ){

          stop();
        }

}

void ADCInit(void){
    ADCON1 = 0x0F; // right justified, all channels are analog (AN0-AN7, ANE0-ANE2)
    TRISA = 0x0F;  // Port A is input for AN0-AN3
    TRISE = 0x07;  // Port E is input for ANE0-ANE2
    ADCON0 = 0x41; // prescale 16, channel 0 (AN0), don't start conversion, power on ATD
}
  unsigned int ADC_read0(void){
    ADCON0 = 0x41; // Select channel 0 (AN0)
    ADCON0 = ADCON0 | 0x04; // Start conversion
    while((ADCON0 & 0x04)); // Wait for conversion to complete
    return ((ADRESH << 8) | ADRESL);
}

unsigned int ADC_read1(void){
    ADCON0 = 0x42; // Select channel 1 (AN1)
    ADCON0 = ADCON0 | 0x04; // Start conversion
    while((ADCON0 & 0x04)); // Wait for conversion to complete
    return ((ADRESH << 8) | ADRESL);
}

unsigned int ADC_read2(void){
    ADCON0 = 0x43; // Select channel 2 (AN2)
    ADCON0 = ADCON0 | 0x04; // Start conversion
    while((ADCON0 & 0x04)); // Wait for conversion to complete
    return ((ADRESH << 8) | ADRESL);
}

// Similarly, for Port E pins
unsigned int ADC_read3(void){
    ADCON0 = 0x44; // Select channel 3 (ANE0)
    ADCON0 = ADCON0 | 0x04; // Start conversion
    while((ADCON0 & 0x04)); // Wait for conversion to complete
    return ((ADRESH << 8) | ADRESL);
}

unsigned int ADC_read4(void){
    ADCON0 = 0x45; // Select channel 4 (ANE1)
    ADCON0 = ADCON0 | 0x04; // Start conversion
    while((ADCON0 & 0x04)); // Wait for conversion to complete
    return ((ADRESH << 8) | ADRESL);
}

unsigned int ADC_read5(void){
    ADCON0 = 0x46; // Select channel 5 (ANE2)
    ADCON0 = ADCON0 | 0x04; // Start conversion
    while((ADCON0 & 0x04)); // Wait for conversion to complete
    return ((ADRESH << 8) | ADRESL);
}

void main(){


  ADCInit();
  dist1=0;
  dist2=0;
  dist1=UltraSonicBack();
  dist2=UltrasonicFront();
  
TRISB = TRISB | 0b00100000; // Set RB5 as input


  TRISB=TRISB & ~0x04;//step
  PORTB=PORTB & ~0x04;
  TRISB=TRISB & ~0x10;
  PORTB=PORTB & ~0x10;

  mymsDelay(20);
  TRISB=TRISB & ~0x02;//Diriction
  PORTB=PORTB & ~0x02;
  TRISB=TRISB & ~0x08;
  PORTB=PORTB & ~0x08;
  ////////////////////
  TRISC=0b00001001;
  PORTC=0x00;
  T1CON = 0x10;                 //Initialize Timer Module
  TRISD=0b00000011;
  PORTD=0x00;
  mymsDelay(20);
  i=0;
  
  
  while (1){

        mymsDelay(0.02);




        S1 = ADC_read3();
        S2 =ADC_read4();
        S3 = ADC_read5();
        adcX ==ADC_read0();
        adcY = ADC_read1();
        adcZ = ADC_read2();
        
        
        if ((PORTB & 0x20) == 0x20){       /// This function to reset the set point to make Balance on it by bush buttom

           blan = adcY;

        }
        
        
              ///////////First Mode /////////////
        if ((PORTD & 0x01) == 0x01){
           if (dist1<15){
             back();
           }else{
              balance();
           }
            if (dist2<15){
             back();
           }else{
              balance();
           }
           ////
        }
        ///////////Second Mode /////////////
        if ((PORTD & 0x02) == 0x02){//////LDR mode
          if(S2>900){  //Mid LDR
           forward();
          }
          else{
             balance();
          }
          



        if(S1>900){ ////Right LDR
            forward();
          }else{
             balance();
          }


        
        if(S3>900){//// Left LDR
            forward();
          }else{
             balance();
          } 

        } else{


            balance();

        }
    }

}
