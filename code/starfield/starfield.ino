#include <Wire.h>
#include "LED_11x7_Matrix_IS31FL3731.h"

LED_11x7_Matrix_IS31FL3731 ledmatrix = LED_11x7_Matrix_IS31FL3731(); // create ledmatrix object

#define NUM_MATRIX_LEDS 77
#define MATRIX_X 7
#define MATRIX_Y 11

#define MATRIX_B_MAX 60
uint8_t b_matrix[MATRIX_X][MATRIX_Y];
uint8_t b_matrix_target[MATRIX_X][MATRIX_Y];

#define FRAME_CTR_MAX 50
uint8_t frame_ctr;

#define KNOB_PIN 0
double knob_val; // 0-1

#define MOON_PIN 2
uint8_t moon;

#define NC_PIN 3

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(NC_PIN));

  // Matrix Setup
  if (! ledmatrix.begin()) {
    Serial.println("IS31 not found");
    while (1);
  }
  Serial.println("IS31 Found!");
  ledmatrix.clear();

  // LP5036 setup


  // fade all in  
  frame_ctr = 0;
  moon = 0;
  read_knob();
  matrix_gen_target();
  bright_gen_target();

  uint8_t moon_target = pow(254, knob_val) +1;

  while(frame_ctr < FRAME_CTR_MAX){
    moon += moon_target/FRAME_CTR_MAX;
    analogWrite(MOON_PIN, moon);
    update_matrix_stars();
    update_bright_stars();
    frame_ctr++;
    delay(10);
  }

  // prep for loop
  matrix_gen_target();
  bright_gen_target();
  frame_ctr = 0;
}


void loop() {
  
  update_matrix_stars();
  update_bright_stars();
  update_moon();

  read_knob();
  frame_ctr++;
  if(frame_ctr >= FRAME_CTR_MAX){
    matrix_gen_target();
    bright_gen_target();
    frame_ctr = 0;
  }
  delay(10);
}

void read_knob(){
  knob_val = analogRead(KNOB_PIN)/1023.0;
}

void update_moon(){
  // set brightness based on knob value
  moon = pow(254, knob_val) +1;
  analogWrite(MOON_PIN, moon);
}

void update_bright_stars(){
  
}

void bright_gen_target(){

}



void matrix_gen_target(){
  for(int x = 0; x < MATRIX_X; x++){
    for(int y = 0; y < MATRIX_Y; y++){
      if(random(0,100) < 30){
        b_matrix_target[x][y] = random(0,MATRIX_B_MAX); 
      } else {
        b_matrix_target[x][y] = 0;
      }

    }
  }
}

void update_matrix_stars(){
  for(int x = 0; x < MATRIX_X; x++){
      for(int y = 0; y < MATRIX_Y; y++){
        int difference = b_matrix_target[x][y]* (pow(2,knob_val)-.8) - b_matrix[x][y];
        b_matrix[x][y] = b_matrix[x][y] + difference*frame_ctr/FRAME_CTR_MAX;
      }
    }
    matrix_show();
}

void matrix_show(){
  for (uint8_t x=0; x<MATRIX_X; x++) {
    for (uint8_t y=0; y<MATRIX_Y; y++) {
      ledmatrix.drawPixel(x, y, b_matrix[x][y]);
    }
  } 
}



