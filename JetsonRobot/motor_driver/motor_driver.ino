//www.elegoo.com

//    The direction of the car's movement
//  ENA   ENB   IN1   IN2   IN3   IN4   Description  
//  HIGH  HIGH  HIGH  LOW   LOW   HIGH  Car is runing forward
//  HIGH  HIGH  LOW   HIGH  HIGH  LOW   Car is runing back
//  HIGH  HIGH  LOW   HIGH  LOW   HIGH  Car is turning left
//  HIGH  HIGH  HIGH  LOW   HIGH  LOW   Car is turning right
//  HIGH  HIGH  LOW   LOW   LOW   LOW   Car is stoped
//  HIGH  HIGH  HIGH  HIGH  HIGH  HIGH  Car is stoped
//  LOW   LOW   N/A   N/A   N/A   N/A   Car is stoped
#include <string.h>

//define L298n module IO Pin
#define ENA 5 //Left enable
#define ENB 6 //Right enable
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

void forward(){ 
	digitalWrite(ENA,HIGH); //enable L298n A channel
	digitalWrite(ENB,HIGH); //enable L298n B channel
	digitalWrite(IN1,LOW); //set IN1 hight level
	digitalWrite(IN2,HIGH);  //set IN2 low level
	digitalWrite(IN3,HIGH);  //set IN3 low level
	digitalWrite(IN4,LOW); //set IN4 hight level
//	Serial.println("Forward");//send message to serial monitor`
}

void back(){
	digitalWrite(ENA,HIGH);
	digitalWrite(ENB,HIGH);
	digitalWrite(IN1,HIGH);
	digitalWrite(IN2,LOW);
	digitalWrite(IN3,LOW);
	digitalWrite(IN4,HIGH);
//	Serial.println("Back");
}

void right(){
	digitalWrite(ENA,HIGH);
	digitalWrite(ENB,HIGH);
	digitalWrite(IN1,HIGH);
	digitalWrite(IN2,LOW);
	digitalWrite(IN3,HIGH);
	digitalWrite(IN4,LOW); 
//	Serial.println("Left");
}

void left(){
	digitalWrite(ENA,HIGH);
	digitalWrite(ENB,HIGH);
	digitalWrite(IN1,LOW);
	digitalWrite(IN2,HIGH);
	digitalWrite(IN3,LOW);
	digitalWrite(IN4,HIGH);
//	Serial.println("Right");
}

void stop(){
	digitalWrite(ENA,LOW);
	digitalWrite(ENB,LOW);
	digitalWrite(IN1,LOW);
	digitalWrite(IN2,LOW);
	digitalWrite(IN3,LOW);
	digitalWrite(IN4,LOW);
//	Serial.println("Stop");
}

void leftWheel(int speed, bool isForward){
	analogWrite(ENA, speed);
	if (isForward){
		digitalWrite(IN1,LOW); //set IN1 hight level
		digitalWrite(IN2,HIGH);  //set IN2 low level
	}
	else {
		digitalWrite(IN1,HIGH); //set IN1 hight level
		digitalWrite(IN2,LOW);  //set IN2 low level
	}
}

//before execute loop() function, 
//setup() function will execute first and only execute once
void setup() {
  Serial.begin(9600);//open serial and set the baudrate
  pinMode(IN1,OUTPUT);//before useing io pin, pin mode must be set first 
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
}

//Repeat execution
char controlInput = 0;
enum direction {Forward, Backward, Left, Right, Stop};
direction dir = Stop;

void loop() {
//	leftWheel(125, true);
	if (Serial.available() > 0){
		controlInput = Serial.read();		
		switch(controlInput){
			case 'w':
				dir = Forward;
				break;
			case 's':
				dir = Backward;
				break;
			case 'a':
				dir = Left;
				break;
			case 'd':
				dir = Right;
				break;
			case 'q':
				dir = Stop;
				break;
		}	
	}
	switch(dir){
		case Forward:
			forward();
			break;
		case Backward:
			back();
			break;
		case Left:
			left();
			break;
		case Right:
			right();
			break;
		case Stop:
			stop();
			break;
		default:
			stop();
			break;
	}
}
