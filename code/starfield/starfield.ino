#include <Wire.h>
#include "LED_11x7_Matrix_IS31FL3731.h"
#include "LP5036.h"


LED_11x7_Matrix_IS31FL3731 ledmatrix = LED_11x7_Matrix_IS31FL3731(); // create ledmatrix object

#define NUM_MATRIX_LEDS 77
#define MATRIX_X 7
#define MATRIX_Y 11

#define MATRIX_B_MAX 60
uint8_t b_matrix[MATRIX_X][MATRIX_Y];
uint8_t b_matrix_target[MATRIX_X][MATRIX_Y];

#define FRAME_CTR_MAX 30
uint8_t frame_ctr;

#define KNOB_PIN 0
double knob_val; // 0-1

#define MOON_PIN 2
uint8_t moon;

#define NC_PIN 3

LP5036 LP5036; // instantiate LP5036 class

#define NUM_B_STARS 36
const uint8_t base_b_stars[NUM_B_STARS] = 
{
  93, 161, 16, 134, 5, 12,
  44, 21, 49, 27, 8, 2,
  12, 48, 28, 30, 21, 144,
  34, 12, 71, 85, 58, 37,
  98, 255, 28, 31, 128, 89, 
  0, 0, 0, 0, 0, 106 

};

uint8_t b_stars[NUM_B_STARS] = 
{
  93, 161, 16, 134, 5, 12,
  44, 21, 49, 27, 8, 2,
  12, 48, 28, 30, 21, 144,
  34, 12, 71, 85, 58, 37,
  98, 255, 28, 31, 128, 89, 
  0, 0, 0, 0, 0, 106

};

uint8_t t_stars[NUM_B_STARS];


void setup() {
  Serial.begin(9600);
  
  randomSeed(analogRead(NC_PIN));

  // Matrix Setup
  if (! ledmatrix.begin()) {
    Serial.println("IS31 not found");
    // while (1);
  }
  Serial.println("IS31 Found!");
  ledmatrix.clear();

  // LP5036 setup
  // i2c already init in ledmatrix.begin()
  LP5036.I2Cscan();

  LP5036.powerUp();

  LP5036.init(); // set PWM frequency and output current for all leds

  LP5036.setRunMode();
  LP5036.bankControlOff(); // control each rgb led individually to test function

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
  // delay(10);
}

void read_knob(){
  knob_val = analogRead(KNOB_PIN)/1023.0;
}

void update_moon(){
  // set brightness based on knob value
  moon = pow(200, knob_val) +1;
  analogWrite(MOON_PIN, moon);
}

void update_bright_stars(){
  uint8_t brightness = map(knob_val*100, 0,100, 20, 128);
  for(uint8_t ii = 0x00; ii < 12; ii++) {
    LP5036.setBrightness(ii, brightness);

    int d1 = t_stars[ii*3] - b_stars[ii*3];
    
    b_stars[ii*3] = b_stars[ii*3] + d1*frame_ctr/FRAME_CTR_MAX;
    LP5036.setColor(ii*3,   b_stars[ii*3]);
    // if(ii == 10) break;
    int d2 = t_stars[ii*3+1] - b_stars[ii*3+1];
    int d3 = t_stars[ii*3+2] - b_stars[ii*3+2];
    b_stars[ii*3+1] = b_stars[ii*3+1] + d2*frame_ctr/FRAME_CTR_MAX;
    b_stars[ii*3+2] = b_stars[ii*3+2] + d3*frame_ctr/FRAME_CTR_MAX;
    LP5036.setColor(ii*3+1, b_stars[ii*3+1]);
    LP5036.setColor(ii*3+2, b_stars[ii*3+2]);
  }

}

void bright_gen_target(){
  for(int i = 0; i < NUM_B_STARS; i++){
    if(i == 24 || i == 25) t_stars[i] = b_stars[i]; // no twinkle for planets
    else {
      t_stars[i] = base_b_stars[i]*(random(30,100)/100.0);
      if(t_stars[i] < 2) t_stars[i] = 2;
    }
  }
}



void matrix_gen_target(){
  for(int x = 0; x < MATRIX_X; x++){
    for(int y = 0; y < MATRIX_Y; y++){
      if(random(0,100) < (30+20*knob_val)){
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
        // ledmatrix.drawPixel(x, y, b_matrix[x][y]);
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



