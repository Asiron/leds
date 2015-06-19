#define USE_OCTOWS2811

#include<OctoWS2811.h>
#include<FastLED.h>

#define NUM_LEDS_PER_STRIP 9
#define NUM_STRIPS 4
#define ACTUAL_STRIPS 2
#define PORT_SHIFT 2
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

// Pin layouts on the teensy 3:
// OctoWS2811: 2,14,7,8,6,20,21,5

void set_led(int i, int j, CRGB color);
void set_led_hsv(int i, int j, byte hue, byte sat, byte val);
void random_noise_anim();
void start_up();


void setup() {
  Serial.begin(9600);
  LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
  LEDS.setBrightness(255);
  // start_up();
}

void start_up()
{
  for (int i = 0; i < ACTUAL_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
      set_led(0, j, CRGB::Black);
      set_led(1, j, CRGB::Black);
    }
  }
  LEDS.show();

  for (int j = 0; j < NUM_LEDS_PER_STRIP - 2; j++) {
    set_led(0, j, CRGB::White);
    set_led(1, j, CRGB::White);
    LEDS.show();
    LEDS.delay(50 * NUM_LEDS_PER_STRIP - 50 * j);
  }

  for (int j = NUM_LEDS_PER_STRIP - 3; j < NUM_LEDS_PER_STRIP; j++) {
    LEDS.delay(100 * NUM_LEDS_PER_STRIP - 100 * j);
    set_led(0, j, CRGB::White);
    set_led(1, j, CRGB::White);
    LEDS.show();
  }

  LEDS.delay(500);


  for (int i = 0; i < ACTUAL_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
      set_led(i, j, CRGB::Green);
    }
  }

  LEDS.show();
  LEDS.delay(2000);
}

inline void set_led(int i, int j, CRGB color)
{
  leds[(i + PORT_SHIFT) * (NUM_LEDS_PER_STRIP) + j] = color;
}

void set_led_hsv(int i, int j, byte hue, byte sat, byte val)
{
  leds[(i + PORT_SHIFT) * (NUM_LEDS_PER_STRIP) + j] = CHSV(hue, sat, val);
}

void random_noise_anim()
{
}


void loop() {

  static uint8_t hue = 0;

  static uint8_t ind_i = 0;
  static uint8_t ind_j = 0;
  static int8_t  dir   = 1;

  static uint8_t index = 0;

  static uint8_t max_index = NUM_LEDS_PER_STRIP - 1;
  static uint8_t min_index = 0;

  static uint32_t current_delay_time = 10;

  for (int i = PORT_SHIFT; i < NUM_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
      leds[(i * (NUM_LEDS_PER_STRIP)) + j] = CRGB::Black;
    }
  }

  index += dir;

  /* control direction */

  //  uint8_t ran = rand();
  //  max_index = ran / 31;


  //current_delay_time = (20 * (NUM_LEDS_PER_STRIP)) - (20 * max_index);

  current_delay_time = 20;

  if (dir == 1 && index >= max_index ) {
    dir = -1;
    index = max_index - 1;

    uint8_t ran = rand();
    max_index = ran / 31;

    current_delay_time *= 3;
    current_delay_time += 10;


  } else if (dir == -1 && index <= min_index) {
    dir = 1;
    index = min_index;
  }

  index %= NUM_LEDS_PER_STRIP;

  hue = 0 + index * 3;


  for (int i = PORT_SHIFT; i < NUM_STRIPS; i++) {
    for (int j = 0; j < index + 1; j++) {

      uint8_t max_val = 255;
      uint8_t min_val = 100;
      int value = max_val - ((max_val - min_val) * 1000) / (index * 1000) * j;

      leds[(i * (NUM_LEDS_PER_STRIP)) + j] = CHSV(hue, 255, value);
    }
  }

  /*
    if (index == 0)
    {
      set_led_hsv(0, 0, hue, 255, 40);
      set_led_hsv(1, 0, hue, 255, 40);

    } else {
      for (int i = PORT_SHIFT; i < NUM_STRIPS; i++) {
        for (int j = 0; j < index + 1; j++) {

          uint8_t max_val = 255;
          uint8_t min_val = 100;
          int value = max_val - ((max_val - min_val) * 1000) / (index * 1000) * j;

          leds[(i * (NUM_LEDS_PER_STRIP)) + j] = CHSV(hue, 255, value);
        }
      }
    }
   */
  /*

  for (int i = 0; i < ACTUAL_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
      set_led_hsv(i, j, 255, 255, 0);
    }
  }

  static uint8_t hue_left, hue_right;

  if (ind_j == 7 and ind_i == 0) {
    hue_left = hue;
  } else if (ind_j == 7 and ind_i == 1) {
    hue_right = hue_left;
  }


  set_led_hsv(0, 8, hue_left, 255, 255);
  set_led_hsv(1, 8, hue_right, 255, 255);

  ind_j += dir;
  ind_j %= NUM_LEDS_PER_STRIP;

  if (dir == 1 && ind_j >= NUM_LEDS_PER_STRIP - 2) {
    dir = -1;
  }

  if (dir == -1 && ind_j <= 0) {
    dir = 1;
    ind_i = (ind_i) == 1 ? 0 : 1;
  }


  //  Serial.print(ind_i);
  //  Serial.print(" ");
  //  Serial.print(ind_j);
  //  Serial.print(" ");
  //  Serial.println(dir);

  set_led_hsv(ind_i, ind_j, 128, 255, 255);

  //  // Set the first n leds on each strip to show which strip it is
  //  for(int i = 0; i < NUM_STRIPS; i++) {
  //    for(int j = 0; j <= i; j++) {
  //      leds[(i*NUM_LEDS_PER_STRIP) + j] = CRGB::Red;
  //    }
  //  }

  hue += 30;


  */

  //hue += 10;


  LEDS.show();
  LEDS.delay(current_delay_time);
}

