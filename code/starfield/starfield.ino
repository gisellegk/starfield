#include <Wire.h>
#include "LED_11x7_Matrix_IS31FL3731.h"

LED_11x7_Matrix_IS31FL3731 ledmatrix = LED_11x7_Matrix_IS31FL3731(); // create ledmatrix object

#define NUM_MATRIX_LEDS 77
#define MATRIX_X 7
#define MATRIX_Y 11
#define MATRIX_MAX 50
uint8_t b_matrix[MATRIX_X][MATRIX_Y];


void setup() {

  Serial.begin(9600);
  if (! ledmatrix.begin()) {
    Serial.println("IS31 not found");
    while (1);
  }
  Serial.println("IS31 Found!");

  randomSeed(analogRead(3));
  delay(100);
  matrix_fade_in();
}


void loop() {
  
  matrix_gen_random();
  matrix_show();
  delay(100);
}


void matrix_gen_random(){
  for(int x = 0; x < MATRIX_X; x++){
    for(int y = 0; y < MATRIX_Y; y++){
      b_matrix[x][y] = random(0,MATRIX_MAX);
    }
  }
}

void matrix_fade_in(){
  int numSteps = 60;
  matrix_gen_random();
  for(int steps = 1; steps <= numSteps; steps++){
    for(int x = 0; x < MATRIX_X; x++){
      for(int y = 0; y < MATRIX_Y; y++){
        ledmatrix.drawPixel(x, y, ((int)b_matrix[x][y])*steps/numSteps);
      }
    }
  }
  
}

void matrix_show(){
  for (uint8_t x=0; x<MATRIX_X; x++) {
    for (uint8_t y=0; y<MATRIX_Y; y++) {
      ledmatrix.drawPixel(x, y, b_matrix[x][y]);
    }
  } 
}



