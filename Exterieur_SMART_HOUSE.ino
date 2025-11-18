// Call library
#include <Servo.h>


// ______________________Define objects__________________________
Servo servodoor;
Servo servotracking;


// ______________________Define pins________________________
// garage
// the second ultra sonic sensor's pins
#define TRIG_2 4
#define ECHO_2 5
// Servo motor for garage door (PWM pin )
#define SERVO_DOOR 9

// Smart lighting
// the first ultra sonic sensor's pins
#define TRIG_1 2
#define ECHO_1 3
// LDR's pin
#define LDR_obscurity A0
// LED
#define LED_1 6
#define LED_2 7

//  photovoltaic module
//LDR's pins
#define LDR_LEFT A1
#define LDR_RIGHT A2
// Servo motor for tracking light source (PWM pin )
#define SERVO_TRACKING 10


// ______________________Define threshold______________________
#define distance_threshold_light 50
#define distance_threshold_garage 20
#define obscurity_threshold 500
#define led_delay 1000

// to ensure LED don't switch off immediatly 
unsigned long led_timer = 0;  
bool leds_on = false;


// ______________________Define personnalised fonctions__________________________
// Measure Distance 
long distance_measure (int trig_pin, int echo_pin){
  digitalWrite(trig_pin,LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin,LOW);
  long duration = pulseIn(echo_pin, HIGH);
  long distance_cm = duration * 0.034 / 2;
  return distance_cm;
}


// tracking light source
int track (int ldr_left, int ldr_right, int current_pos){
  int left=analogRead (ldr_left);
  int right=analogRead(ldr_right);
  int pos = current_pos;
    if (left+10<right){ 
     pos-=1;
     } else if (left-10>right){ pos+=1;}
    // to ensure the servo's position stays between 0 and 180°
  pos = constrain(pos, 0, 180);
  // smooth the mouvement of the servo motor
  int smoothed_pos = current_pos + (pos - current_pos) / 4; 
  return smoothed_pos;
}

// openning the garage door
int open_door (long distance, int threshold){
 if (distance<=threshold){
   return 90;
  } else return 0;
}


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(TRIG_1,OUTPUT);
pinMode(TRIG_2,OUTPUT);
pinMode(ECHO_1,INPUT);
pinMode(ECHO_2,INPUT);

servodoor.attach(SERVO_DOOR);
servotracking.attach(SERVO_TRACKING);

servodoor.write(0);
servotracking.write(90);

pinMode(LED_1,OUTPUT);
pinMode(LED_2,OUTPUT);
}


void loop() {
  // put your main code here, to run repeatedly:
 int obscurity=analogRead(LDR_obscurity);
 int LEFT=analogRead(LDR_LEFT);
 int RIGHT=analogRead(LDR_RIGHT);
 long distance_1=distance_measure (TRIG_1, ECHO_1);
 long distance_2=distance_measure (TRIG_2, ECHO_2);
  
  // switch on and off the LED
  if (distance_threshold_light>=distance_1 && obscurity>=obscurity_threshold){
   digitalWrite(LED_1,HIGH);
   digitalWrite(LED_2,HIGH);
   leds_on = true;
   led_timer = millis(); 
  }else if (leds_on && millis() - led_timer < led_delay){
    digitalWrite(LED_1, HIGH);
    digitalWrite(LED_2, HIGH);
  } else {
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
    leds_on = false;
  }

 int position=open_door (distance_2, distance_threshold_garage);


servodoor.write(position);
static int panel_pos = 90;
panel_pos = track(LEFT, RIGHT, panel_pos);
servotracking.write(panel_pos);

delay(100); 
}
