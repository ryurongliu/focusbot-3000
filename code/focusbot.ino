#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include "TickTwo.h"

//potentiometers
#define POT1 12
#define POT2 13

//start button
#define START 22

//string of lights
#define cheekLIGHTS 17
int numLights = 2;
Adafruit_NeoPixel cheekPixels(numLights, cheekLIGHTS, NEO_RGB + NEO_KHZ800);

#define browLIGHTS 32
int numbrowLights = 10;
Adafruit_NeoPixel browPixels(numbrowLights, browLIGHTS, NEO_RGB + NEO_KHZ800);

//light sensor things
#define LDR 36
int LDR_val = 0;
int LDR_threshold = 100;
bool currLight = false;
int alarm_tone = 1000;
int chirp_tone = 400;

//piezo
#define BUZZER_PIN 2
#define BUZZER_CHANNEL 3
bool buzzerAttached = false;

//servo
#define SERVOPIN 15
Servo myservo;

//function declarations
void blinkLight();
void alarmLight();
TickTwo timer(blinkLight, 500);
TickTwo alarmTimer(alarmLight, 250);
bool alarmLightState = true;

int curr_interval = 0;
int curr_increment = 5;
int curr_button = 1;


int timeVals[] = {60000, 300000, 600000, 1800000, 3600000, 1, 2, 3, 4, 5};
String timeDesc[] = {"1 min", "5 min", "10 min", "30 min", "1 hr", "1", "2", "3", "4", "5"};

bool timerGoing = false;

bool lightState = true;
int blinksPerBlinker = -1;
int currBlinker = -1;

void setup() {
  // put your setup code here, to run once:
  
  //set up buttons
  pinMode(START, INPUT_PULLUP);


  //set up lights
  cheekPixels.begin();
  cheekPixels.show();
  browPixels.begin();
  browPixels.show(); 

  //initialize light colors
  cheekPixels.setPixelColor(0, cheekPixels.Color(255, 0, 0)); //cheek 1
  cheekPixels.setPixelColor(1, cheekPixels.Color(255, 0, 0)); //cheek 2
  cheekPixels.show(); 


  //initialize serial
  Serial.begin(9600);

  //reset LEDs
  relightPots();

  //stop timer if necessary
  if (timerGoing){
    timer.stop();
    timerGoing = false;
  }


  myservo.setPeriodHertz(50);
  myservo.attach(SERVOPIN, 500, 2500);
  myservo.write(0);

  ledcSetup(BUZZER_CHANNEL, alarm_tone, 8); 
  
}






//callback function to blink LED
void blinkLight() {
  if (!lightState){
    //Serial.println("light on");
    browPixels.setPixelColor(currBlinker - 1, browPixels.Color(255, 255, 255));
    browPixels.show();
  }
  else{
    //Serial.println("light off");
    browPixels.setPixelColor(currBlinker - 1, browPixels.Color(0, 0, 0));
    browPixels.show();
  }
  lightState = !lightState;
  Serial.println(timer.counter());
}

void alarmLight(){
  if (!alarmLightState){
    cheekPixels.setPixelColor(0, cheekPixels.Color(0, 255, 0));
    cheekPixels.setPixelColor(1, cheekPixels.Color(0, 255, 0));
    cheekPixels.show();
  }
  else{
    cheekPixels.setPixelColor(0, cheekPixels.Color(0, 0, 0));
    cheekPixels.setPixelColor(1, cheekPixels.Color(0, 0, 0));
    cheekPixels.show();
  }
  alarmLightState = !alarmLightState;
}


//function to update lights based on potentiometer readings
void potLightUpdate(int currval, int readval){
  int diff = currval - readval;
    if (diff == -1){ //read greater than curr, add one light
      browPixels.setPixelColor(readval - 1, browPixels.Color(255, 255, 255));
      browPixels.show();
    }
    else if (diff == 1){ //reaad less thaan curr, remove one liight
      browPixels.setPixelColor(currval - 1, browPixels.Color(0, 0, 0));
      browPixels.show();
    }
}


//resets lights according to pots (in case of reset or if they were moved during timer)
void relightPots(){
  delay(1000);
  
  int pot1state = analogRead(POT1); //pot1
  int pot2state = analogRead(POT2); //pot2

  //pot LED selections
  int curr_interval = pot1state/819;
  int curr_increment = 5 + pot2state/819;

  //light increment LEDs
  int i = curr_increment;
  while (i > 5){
    browPixels.setPixelColor(i-1, browPixels.Color(255, 255, 255));
    i -= 1;
  }

  //light interval LEDs
  i = curr_interval;
  while (i > 0){
    browPixels.setPixelColor(i-1, browPixels.Color(255, 255, 255));
    i -= 1;
  }
  browPixels.show();
}


void chirpBuzzer(){
  ledcWriteTone(BUZZER_CHANNEL, chirp_tone);
  delay(250);
  ledcDetachPin(BUZZER_PIN);
  delay(250);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  ledcWriteTone(BUZZER_CHANNEL, chirp_tone);
  delay(250);
  ledcDetachPin(BUZZER_PIN);
  buzzerAttached = false;
}



void closeGate(){
  for (int i = 0; i < 175; i++){
    myservo.write(i);
    delay(20);
  }
}

void openGate(){
  for (int i = 175; i> 0; i--){
    myservo.write(i);
    delay(20);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  //when timer is not going...
  if (!timerGoing){

    //get input reads
    int STARTstate = digitalRead(START); //button
    int pot1state = analogRead(POT1); //pot1
    int pot2state = analogRead(POT2); //pot2

    //pot LED selections
    int read_interval = pot1state/819;
    int read_increment = 5 + pot2state/819;
  
    //update interval lights based on pot1 read
    if (read_interval != curr_interval){
      potLightUpdate(curr_interval, read_interval);
      curr_interval = read_interval;
    }
  
    //update increment lights based on pot2 read
    if (read_increment != curr_increment){
      potLightUpdate(curr_increment, read_increment);
      curr_increment = read_increment;
    }
  
    //start the timer on button press 
    if (STARTstate != curr_button and STARTstate == 0){ //if button is pressed
      Serial.println("button pressed");
      if (curr_interval > 0 and curr_increment > 5){ //if time has been set
        
        //number of milliseconds 
        int numMillis = timeVals[curr_interval - 1] * timeVals[curr_increment - 1];
       
        currBlinker = curr_increment; //pixel array address of first LED to blink
        int numBlinks = numMillis / 1000 * 2; //total number of blinks
        int numBlinkers = currBlinker - 5; //total number of blinkers
        blinksPerBlinker = numBlinks / numBlinkers;
        
        //debugging....
        int numMins = numMillis / 1000 / 60;
        Serial.print("Timer set to ");
        Serial.print(timeDesc[curr_interval - 1]);
        Serial.print(" x ");
        Serial.println(timeDesc[curr_increment - 1]);
        Serial.print("Starting timer for ");
        Serial.print(numBlinks / 2);
        Serial.println(" secs");
        Serial.print(numBlinks);
        Serial.println(" blinks");
        Serial.print("curr Blinker ");
        Serial.println(currBlinker);
        Serial.print("blinks per blinker ");
        Serial.println(blinksPerBlinker); 

        closeGate();
        //start timer
        timer.start(); 
        timerGoing = true;

        //set cheeks to red
        cheekPixels.setPixelColor(0, cheekPixels.Color(0, 255, 0));
        cheekPixels.setPixelColor(1, cheekPixels.Color(0, 255, 0));
        cheekPixels.show();
      }
      curr_button = STARTstate; //save current button state
    }
  }



  //if timer is going....
  if (timerGoing){
    //update timer
    timer.update();
    alarmTimer.update();


    
    bool newLight = false;
    //if light sensor senses light, scream
    LDR_val = analogRead(LDR);
    if (LDR_val > LDR_threshold){
      newLight = true;
    }
    if (currLight != newLight){
      currLight = newLight;
      if(currLight){
        Serial.println("OH NO LIGHT");
        ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
        ledcWriteTone(BUZZER_CHANNEL, alarm_tone);
        buzzerAttached = true;
        alarmTimer.start();
      }
      else{
        Serial.println("oh good no light");
        ledcDetachPin(BUZZER_PIN);
        buzzerAttached = false;
        alarmTimer.pause();
        cheekPixels.setPixelColor(0, cheekPixels.Color(0, 255, 0));
        cheekPixels.setPixelColor(1, cheekPixels.Color(0, 255, 0));
        cheekPixels.show();
      }
    }


    //if timer hits blinker limit
    if (timer.counter() == blinksPerBlinker){
      //if there are more LEDs left to blink
      if (currBlinker > 6){
        browPixels.setPixelColor(currBlinker-1, browPixels.Color(0, 0, 0));
        browPixels.show();
        currBlinker -= 1; //move on to the next LED
        lightState = true;
        timer.start(); //restart timer

        
        Serial.println("restarting timer");
        Serial.println("current blinker");
        Serial.println(currBlinker);
      }
      
      //else if all LEDs are done blinking
      else{
        //stop timer
        timer.stop();
        timerGoing = false;
        Serial.println("timer stopped");

        alarmTimer.stop();

        openGate();
        
        //set all timer leds off
        for (int i =1; i < 11; i++){
          browPixels.setPixelColor(i-1, browPixels.Color(0, 0, 0));
        }
        
        //set cheeks to green 
        cheekPixels.setPixelColor(0, cheekPixels.Color(255, 0, 0));
        cheekPixels.setPixelColor(1, cheekPixels.Color(255, 0, 0));
        cheekPixels.show();

        //chirp buzzer
        if (!buzzerAttached){
          ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
        }
        chirpBuzzer();
        //relight pot leds
        relightPots();
        curr_button = 1; //reset button state for next timer
      }
    }
  }
  delay(100);
}
