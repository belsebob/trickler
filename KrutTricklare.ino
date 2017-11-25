//
// at bottom IR values are:
//bottomIRSensor
//13
//topIRSensor
//1023


/*----( Initialize stepper motor )----*/

#include <Stepper.h>

//---( Number of steps per revolution of INTERNAL motor in 4-step mode )---
#define STEPS_PER_MOTOR_REVOLUTION  32//32   

//---( Steps per OUTPUT SHAFT of gear reduction )---
//#define STEPS_PER_OUTPUT_REVOLUTION 80  //2048  
#define STEPS_PER_OUTPUT_REVOLUTION 40  //2048   

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to

//The pin connections need to be 4 pins connected
// to Motor Driver In1, In2, In3, In4  and then the pins entered
// here in the sequence 1-3-2-4 for proper sequencing
Stepper small_stepper(STEPS_PER_MOTOR_REVOLUTION, 4, 6, 5, 7);


/*-----( Declare other variables )-----*/

int  Steps2Take; 
int topIRSensor; 
int bottomIRSensor;
int buttonInputValue; //cable signal input value
int debugFullTimerValue;
const int  buttonPin = 3; //button signal reading
const int bigMotorSignalPin =11; //output signal 12v motor
long bigMotorCalibratedtimer =0;
int buttonState= 0;
bool go = false;

/*-----( Settings )-----*/
//values are estimated recommendations
const int fullSpeed = 255; //Big motor full speed
const int slowSpeed = 60; //40-100 Big motor low speed 
const int bigMotorGoDelayTime = 100; //50 Big motor runs for this time in slowSpeed setting 
const int bigMotorNoGoDelayTime = 0; //0- 200 Big motor stands still for this time in slowSpeed setting ----funkar ej??
const long bigmotorSafetyDeductionTime = 200; // 100-500  subtracts this in milliseconds from sample load timer


/*----( SETUP: RUNS ONCE )----*/
void setup()   
{
  pinMode(bigMotorSignalPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  topIRSensor = analogRead(A7);
  bottomIRSensor = analogRead(A6); 
    
  Serial.begin(9600);
  Pause();

}

/*----( Main Loop: RUNS CONSTANTLY )----*/ 

void loop()   
{ 
//buttonState = digitalRead(buttonPin);

if (go==true)

{
if (bigMotorCalibratedtimer ==0) //Om tid för stora motor är 0 -kalibrera denna
{CalibrateBigmotor(); //Kalibrerar tiden för stora motorn en gång
}
else
Trickle();
}


Serial.println("outside in loop");
buttonState = digitalRead(buttonPin);
if (buttonState==HIGH)
{

go = !go;
delay(400);

}}


/*----( Calibrate - Runs once )----*/

void  CalibrateBigmotor()
{
 unsigned long timer1 = 0;
 unsigned long timer2 =0;
 
 while (go ==true)
 {
 if (timer1 ==0 )
 {timer1 = millis();} //sets timer1 one time at startup

  
  bottomIRSensor = analogRead(A6);  //läser undre IRSensor
  analogWrite(bigMotorSignalPin, fullSpeed);
  
   Steps2Take  =  STEPS_PER_OUTPUT_REVOLUTION ;  // Rotate CW 1 turn
  small_stepper.setSpeed(800);   
  small_stepper.step(Steps2Take); 

  if (bottomIRSensor<700)
{

timer2 = millis()-timer1;

debugFullTimerValue = timer2; ///debug**********

  analogWrite(bigMotorSignalPin, 0);
  bigMotorCalibratedtimer = (timer2-bigmotorSafetyDeductionTime);


  
 Serial.println("bigMotorCalibratedtimer");
  Serial.println(bigMotorCalibratedtimer);

Serial.println("timer1");
  Serial.println(timer1);
  Serial.println("timer2");
  Serial.println(timer2);

   Serial.println(millis());
delay(300);

Serial.println("timer1");
  Serial.println(timer1);

   Serial.println(millis());


Serial.println("topIRSensor");
Serial.println("bottomIRSensor");
Serial.println(topIRSensor);
Serial.println(bottomIRSensor);
  
  
  go = false;
}

Serial.println("in calibrate");
buttonState = digitalRead(buttonPin); 
 if (buttonState==HIGH)
{
go = false;
delay(400);
 Pause();
}
 
 }

  }

/*----( Pause when done, waits for new button signal )----*/

  void Pause()

{
while( go==false)

    {

  analogWrite(bigMotorSignalPin, 0);//pause big motor, stepper is automatically paused
  Serial.println("Pause");

topIRSensor = analogRead(A7);
bottomIRSensor = analogRead(A6);
 Serial.println("topIRSensor");
 Serial.println(topIRSensor);
      Serial.println("bottomIRSensor");
 Serial.println(bottomIRSensor);

 
     buttonState = digitalRead(buttonPin); 
 if (buttonState==HIGH)
{
go = true;
delay(400);
 
}}
    
  }



/*----( Trickle )----*/

void Trickle() 
{
  
 if (go ==true)
 {

Serial.println("in trickle");

long bigmotorCurrentTimer =0; //timer for each load

topIRSensor = analogRead(A7);
bottomIRSensor = analogRead(A6);

long resetTimer = millis(); //millis at this moment

if (topIRSensor >900  && bottomIRSensor > 700 ) //if beam is at bottom
//if (topIRSensor > 1000 && bottomIRSensor < 50 ) //if beam is at bottom ///values if proximity sensors are used

{
//while current timer is smaller than calibrated big motor is a GO at FULL speed
while (bigmotorCurrentTimer < bigMotorCalibratedtimer  ) 

{
analogWrite(bigMotorSignalPin, fullSpeed);
 Steps2Take  =  STEPS_PER_OUTPUT_REVOLUTION ;  // Rotate CW 1 turn
  small_stepper.setSpeed(800);   
  small_stepper.step(Steps2Take); 

bigmotorCurrentTimer = millis()-resetTimer; //subtracts resetTimer value from millis to get timer
bottomIRSensor = analogRead(A6);

if (bottomIRSensor < 650)//if beam is not at bottom

go = false;
//{bigMotorCalibratedtimer =bigMotorCalibratedtimer-300; } //automaticlly lowers bigMotorCalibratedtimer if load overshoots

Serial.println("debugFullTimerValue");
Serial.println(debugFullTimerValue);
Serial.println("bigMotorCalibratedtimer");
Serial.println(bigMotorCalibratedtimer);
Serial.println("bigmotorCurrentTimer");
Serial.println(bigmotorCurrentTimer);



Serial.println("in trickle");

 
}
}

analogWrite(bigMotorSignalPin, 0);  
//delay(1000); // testing speeding things up 161029

topIRSensor = analogRead(A7);
bottomIRSensor = analogRead(A6);



//final check, beam is balanced at 7
while (topIRSensor>30) 
{


//While bottomIRSensor > 700   big motor is a GO at SLOW speed
while (bottomIRSensor > 700 )
{ 
  bottomIRSensor = analogRead(A6);
  
       Serial.println(bigmotorCurrentTimer);
       Serial.println("bigmotorCurrentTimer > bigMotorCalibratedtimer");
       Serial.println(bigMotorCalibratedtimer);
   
  analogWrite(bigMotorSignalPin, slowSpeed);
  


  long currTime = millis(); //millis at this moment 
  long motorRunTime =0;

    while( currTime + bigMotorGoDelayTime > motorRunTime  )
  {
   motorRunTime= millis();
   if( !bottomIRSensor > 700)
   analogWrite(bigMotorSignalPin, 0); 
  
  }  
 analogWrite(bigMotorSignalPin, 0);

 
   Steps2Take  =  STEPS_PER_OUTPUT_REVOLUTION ;  // Rotate CW 1 turn
  small_stepper.setSpeed(800);   
  small_stepper.step(Steps2Take); 
  
  //analogWrite(bigMotorSignalPin, 0);
  
//  delay(bigMotorNoGoDelayTime);


unsigned long bigMotorStopTimer;
int bigmotorStopTime = 0;
bigMotorStopTimer = millis();

while ((millis()-bigMotorStopTimer)<bigmotorStopTime){
  
   Steps2Take  =  STEPS_PER_OUTPUT_REVOLUTION ;  // Rotate CW 1 turn
  small_stepper.setSpeed(800);   
  small_stepper.step(Steps2Take);
}
 
}


topIRSensor = analogRead(A7);
bottomIRSensor = analogRead(A6);

if(bottomIRSensor > 700)
{ analogWrite(bigMotorSignalPin, 0);}

  Steps2Take  =  STEPS_PER_OUTPUT_REVOLUTION ;  // Rotate CW 1 turn
  small_stepper.setSpeed(800);   
  small_stepper.step(Steps2Take);  
//  }

Serial.println(topIRSensor);  


buttonState = digitalRead(buttonPin);
if (buttonState==HIGH)
{
go = false;
delay(400);
Pause();
  }
 }

  analogWrite(bigMotorSignalPin, 0);
  go=false;
 
 }

}
  
  
  
  
  
