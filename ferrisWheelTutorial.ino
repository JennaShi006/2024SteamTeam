//Code tested physically.
#include <avr/interrupt.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // LCD address can be 0x3F also for a 16 chars and 2 line display

//Indicator Variables
const byte led_pin=13;
volatile bool light=false;

//pin definitions
const byte inc_pin=8;
const byte dec_pin=9;
const byte dir_pin=10;

//Motor/LM293D
const byte en_pin=6;
const byte f_pin=5;
const byte b_pin=4;
const byte steps=5;

//Indicator Variables
volatile bool allow=true;
volatile bool inc=true;
byte speed=0;
volatile bool fwd=true;
volatile bool start=false;
volatile bool dir_change=false;

//Printing to the LCD Screen
String pwm="  PWM";
char dir[] = {'B', 'F'};
char state[]={'S','R'};


void setup() {
  lcd.init();
  lcd.clear();//Clear Existing Text on the LCD        
  lcd.backlight();// Make sure backlight is on  
  // Print Main Message Template on the LCD
  lcd.setCursor(0,1);//Set cursor to character 0 on line 1
  lcd.print(speed);
  lcd.print(pwm);
  lcd.setCursor(0,0);
  lcd.print(dir[fwd]);
  lcd.print("  ");
  lcd.print(state[start]);
  
  //Selecting Pin Modes
  pinMode(led_pin,OUTPUT);
  pinMode(f_pin,OUTPUT);
  pinMode(b_pin,OUTPUT);
  pinMode(en_pin,OUTPUT);
  //Initializing Pins to Low
  digitalWrite(led_pin,LOW);
  digitalWrite(f_pin,LOW);
  digitalWrite(b_pin,LOW);
  digitalWrite(en_pin,LOW);
  
  //Holding Interrupt Pins to High
  pinMode(inc_pin,INPUT_PULLUP);
  pinMode(dec_pin,INPUT_PULLUP);
  pinMode(dir_pin,INPUT_PULLUP);
  
  //Pin Interrupts on Pins-8/9/10
  PCICR=0;
  PCICR|=(1<<PCIE0);//0 is PORT B,1 is PORT C and 2 is PORT D
  PCMSK0=0;
  PCMSK0|=(1<<PCINT0);//From 8 to 13-Enable Pin 8
  PCMSK0|=(1<<PCINT1);//Enable Pin9
  PCMSK0|=(1<<PCINT2);//Enable Pin10
  sei();
}

void loop() {
  digitalWrite(led_pin,light);   
//Increase Speed-dir_change prevents errors 
  if (allow==false && inc==true && dir_change==false){
    digitalWrite(led_pin,light);    
    if (speed>=115){//Limit to 120 PWM
      speed=120;
    }else if (speed==0){
      speed=75;//start from 75
    }else{
      speed=speed+steps;
    }
    lcd.setCursor(0,1);    
    lcd.print(speed);    
    delay(400);//Debounce the switch
    allow=true;
    light=false;     
  }
//Decrease Speed-dir_change prevents errors
  if (allow==false && inc==false && dir_change==false){
    digitalWrite(led_pin,light);
    if (speed<=75){//No ramping below 75
      speed=0;
    }else{
      speed=speed-steps;
    }
    lcd.setCursor(0,1);
    lcd.print("   ");    
    lcd.setCursor(0,1);
    lcd.print(speed);
    delay(400);//Debounce the switch
    allow=true;
    light=false;     
  }
  //Flip Direction
  if (dir_change==true && allow==false){
    digitalWrite(led_pin,light);
    lcd.setCursor(0,0);
    lcd.print(dir[fwd]);    
    delay(400);//Debounce the switch
    dir_change=false;//ready for next dir change    
    allow=true;
    light=false;         
  }

  //State Update- S vs R
  if(speed>0){
    start=true;
    lcd.setCursor(3,0);
    lcd.print(state[start]);
  }else{
    start=false;
    lcd.setCursor(3,0);
    lcd.print(state[start]);
  }
//disable interrupts before this call
  run_wheel(state[start],dir[fwd],speed);
//enable afterwords
}

ISR (PCINT0_vect){//ideally should toggle  
  if (bitRead(PINB,0)==0 && allow==true){    
    light=true;//light changes state
    allow=false;//allow goes to false-no repeat interrupts
    inc=true;
  }

  if (bitRead(PINB,1)==0 && allow==true){ 
    light=true;//light changes state
    allow=false;//allow goes to false-no repeat interrupts   
    inc=false;
  }

  if (bitRead(PINB,2)==0 && allow==true){    
    if(speed==0){
      light=true;      
      dir_change=true;
      allow=false;
      fwd=!fwd;
    }   
  }
}

void run_wheel(char check_state, char set_dir, byte set_speed){
  //if state is stop, set enable to 0
  //if state is run, use set dir to set H/L
  //analog write the speed to drive pin
  if(check_state=='S'){
    analogWrite(en_pin,set_speed);
    digitalWrite(f_pin, LOW);
    digitalWrite(b_pin, LOW);
  }
  
  if (check_state=='R' && set_dir=='F'){
    digitalWrite(f_pin, HIGH);
    digitalWrite(b_pin, LOW);
    analogWrite(en_pin,set_speed);
  }
  
  if (check_state=='R' && set_dir=='B'){
    digitalWrite(f_pin, LOW);
    digitalWrite(b_pin, HIGH);
    analogWrite(en_pin,set_speed);
  }
}
