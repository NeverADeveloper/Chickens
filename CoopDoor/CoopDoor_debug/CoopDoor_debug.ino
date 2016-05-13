/* 
  * Light-activated Door opener for ChickenCoop
  * by Andrew Westfold. Copyright (c) April 2016.
  *
  * Uses an LDR to measure light intensity, and reed switches to detect door open and closed positions
  * Drives a motor via a Motor control card to open/close the door
  * Has two manual override switches (momentary push switches) to open/close the door
  *
  * Connections
  *	Override buttons
  *		Open Door button
  *			VCC	- D2 - 10kOhm - GND
  *		Close Door button
  *		  	VCC	- D3 - 10kOhm - GND
  *	Door Position sensors
  * 		Door Open Reed switch
  *			VCC - D4 - 10kOhm - GND
  * 		Door Closed Reed switch		
  *			VCC - D5 - 10kOhm - GND
  *	LDR 
  *   		5V - LDR - A0 - 10kOhm - GND
  *	Motor Driver Board
  *		Batt +	Power In
  *		Batt -	Board GND and Arduino GND
  *		Board 5v	Arduino Vin
  *		Motor +	
  *		Motor -	
  *		enA		10
  *		in1		9
  *		in2		8 
*/

//Motor connections
int enA = 10;
int in1 = 9;
int in2 = 8;
//Override buttons
int openButn = 2;
int closeButn = 3;
//Door position sensors
int openSens = 4;
int closeSens = 5;
//LDR sensor
int ldrPin = A0;
//declare variables
volatile int lastState;
volatile int ldrValue = 0;
volatile int lightLevel = -1;
volatile int doorPosition = -2;
volatile int activateDoor = -3;
volatile int openButnState = 0;
volatile int closeButnState = 0;
const int openLevel = 800; //Tune to suit installation
const int closeLevel = 600; //Tune to suit installation
const int motorSpeed = 75; //Tune to suit installation
unsigned long minutesDelay = 5000; // how often should the loop run - 5000 = 5 seconds

void overrideOpen_ISR() // Override to allow manual door opening via interrupt
{
	openButnState = digitalRead(openButn);
	if (openButnState != 0)
	{
		//if the Open Door button is pressed, adjust the activateDoor value
		activateDoor = -1;
	}		
}

void overrideClose_ISR() // Override to allow manual door closing via interrupt
{
	closeButnState = digitalRead(closeButn);
	if (closeButnState != 0)
	{
		//if the Close Door button is pressed, adjust the activateDoor value
		activateDoor = 1;
	}	
}

void setup() //configure arduino pins and interrupts
{
	pinMode(openSens, INPUT); 
	pinMode(closeSens, INPUT);
	pinMode(openButn, INPUT); 
	pinMode(closeButn, INPUT);
	pinMode(ldrPin, INPUT);
	pinMode(enA, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(openButn), overrideOpen_ISR, RISING); //test that RISING acts as intended
	attachInterrupt(digitalPinToInterrupt(closeButn), overrideClose_ISR, RISING); //test that RISING acts as intended
	Serial.begin(9600); // Only needed for initial setup/debugging
}

void loop()
{
	unsigned long loopstart = millis();
	unsigned long nextEvent = loopstart + minutesDelay;   
	// Read the sensor values
	openSens = digitalRead(3);
	closeSens = digitalRead(4);
	ldrValue = analogRead(A0);
	//Use maths to work out which SwitchCase to use
	if (openSens == HIGH)
	{
		lastState = 0;
		doorPosition = -2;
	}
	else if (closeSens == HIGH)
	{
		lastState = 1;
		doorPosition = 2;
	}

	if (ldrValue < closeLevel)
	{
		lightLevel = -1;
	}   
	else if (ldrValue > openLevel)
	{
		lightLevel = 1;      
	}
	//Note that the value of activateDoor is also changed by the override interrupts
	activateDoor = (lightLevel + doorPosition);      
 	switch(activateDoor)
	{
		case -3:
			//switch the motor off
			digitalWrite(in1, LOW);
			digitalWrite(in2, LOW);
			Serial.println("Door is closed"); //debug/testing info
		break;
		case -1:
			//use the motor to open the door
			digitalWrite(in1, HIGH);
			digitalWrite(in2, LOW);
			analogWrite(enA, motorSpeed);
			Serial.println("Door is opening..."); //debug/testing info
		break;
		case 3:
			//switch motor off
			digitalWrite(in1, LOW);
			digitalWrite(in2, LOW);
			Serial.println("Door is open"); //debug/testing info
		break;
		case 1:
			//use motor to close door
			digitalWrite(in1, LOW);
			digitalWrite(in2, HIGH);
			analogWrite(enA, motorSpeed);
			Serial.println("Door is closing..."); //debug/testing info
		break;
	}
	//wait a time before going round the loop again. (Is this delay needed? who knows...)
	while(millis() < nextEvent);
        nextEvent += minutesDelay;
}

    Status API Training Shop Blog About 

    © 2016 GitHub, Inc. Terms Privacy Security Contact Help 

