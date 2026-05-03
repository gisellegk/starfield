#include <Wire.h>
#include "LED_11x7_Matrix_IS31FL3731.h"

LED_11x7_Matrix_IS31FL3731 ledmatrix = LED_11x7_Matrix_IS31FL3731(); // create ledmatrix object

#define NUM_MATRIX_LEDS 77
#define MATRIX_X 7
#define MATRIX_Y 11

#define MATRIX_ABS_MAX 60
uint8_t b_matrix[MATRIX_X][MATRIX_Y];
uint8_t b_matrix_target[MATRIX_X][MATRIX_Y];

#define MATRIX_STEPS_MAX 50
uint8_t matrix_step;

#define KNOB_PIN 0
double knob;

#define NC_PIN 3

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(NC_PIN));

  // init knob value 0-1
  knob_handler();

  // Matrix Setup
  if (! ledmatrix.begin()) {
    Serial.println("IS31 not found");
    while (1);
  }
  Serial.println("IS31 Found!");
  ledmatrix.clear();
  matrix_step = 0;
  
  // LP5036 setup

  // Moon setup


  delay(100);
}


void loop() {
  knob_handler();
  matrix_frame_handler();
  brights_frame_handler();
  moon_handler();
  delay(100);
}

void knob_handler(){
  knob = analogRead(KNOB_PIN)/1023.0;
}

void moon_handler(){
  // set brightness based on knob value
}

void brights_frame_handler(){
  
}


void matrix_frame_handler(){
  matrix_update_frame();
  matrix_step++;
  if(matrix_step >= MATRIX_STEPS_MAX) {
    matrix_gen_random();
    matrix_step = 0;
  }


}

void matrix_gen_random(){
  for(int x = 0; x < MATRIX_X; x++){
    for(int y = 0; y < MATRIX_Y; y++){
      if(random(0,100) < (int)(30*knob)){
        b_matrix_target[x][y] = random(0,(int)(MATRIX_ABS_MAX*knob)); 
      } else {
        b_matrix_target[x][y] = 0;
      }

    }
  }
}

void matrix_update_frame(){
  for(int x = 0; x < MATRIX_X; x++){
      for(int y = 0; y < MATRIX_Y; y++){
        int difference = b_matrix[x][y] - b_matrix_target[x][y];
        b_matrix[x][y] = b_matrix[x][y] - difference*matrix_step/MATRIX_STEPS_MAX;
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



