// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "Color.h"

//#include <Fonts/TomThumb.h>
#include "Dialog_plain_5.h"

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 2
#define WIDTH 30

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(30, 5, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

uint16_t myRemapFn(uint16_t x, uint16_t y) {
  uint16_t i;
  if (y % 2 == 0) {
    i = WIDTH * y + WIDTH - x - 1;
  } else {
    i = WIDTH * y + x;
  }

  if (i == WIDTH * 2 - 1) {
    return -1;
  } else if (i > WIDTH * 2 - 1) {
    return i - 1;
  } else {
    return i;
  }
  
}


const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

void setup() {
  matrix.setRemapFunction(myRemapFn);
  matrix.begin();
//  matrix.setTextWrap(false);
  matrix.setBrightness(40);
//  matrix.setTextColor(colors[0]);
//  matrix.setFont(&Dialog_plain_5);
}

int x    = matrix.width();
int pass = 0;

// The offset makes the colors scroll across the screen.
int offset = 0;

void loop() {
//  matrix.fillScreen(0);
//  matrix.setCursor(0, 5);
//  for (int i = 0; i < 5; i++) {
//    for (int j = 0; j < 30; j++) {
//       matrix.drawPixel(j, i, matrix.Color(255, 0, 0)); 
//       matrix.show();
//       delay(100);
//    }
//  }
  //matrix.print(F("Good Job!"));
//  if(--x < -36) {
//    x = matrix.width();
//    if(++pass >= 3) pass = 0;
//    matrix.setTextColor(colors[pass]);
//  }
//  matrix.show();
//  delay(1000);


  int max = matrix.height() * matrix.width();

    for (int x = 0; x < matrix.width(); x++) {
        for (int y = 0; y < matrix.height(); y++) {
            // pos is our (x,y) position expressed in a linear 0..max
            // line, with offset continually offsetting the starting
            // position. It's used to create a hue.
            int pos = (offset + (x * matrix.width() + y)) % max;

            // Create (hue,saturation,brightness) tuple with:
            // - position mapped to 0..360 range
            // - saturation full (1.0)
            // - brightness full (1.0)
            HSB hsb(pos * (360.0 / max), 1.0, 1.0);

            matrix.drawPixel(x, y, hsb.to_color());
        }
    }

    // Push the updated matrix to the NeoPixel display.
    matrix.show();
    delay(0);

    // Keep offset in the range 0..max.
    offset++;
    
    if (offset >= max) {
        offset = 0;
    }
}
