#include "AnimationManager.h"

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

inline void set_led(int i, int j, CRGB color)
{
  leds[(i + PORT_SHIFT) * (NUM_LEDS_PER_STRIP) + j] = color;
}

inline void set_led_hsv(int i, int j, byte hue, byte sat, byte val)
{
  leds[(i + PORT_SHIFT) * (NUM_LEDS_PER_STRIP) + j] = CHSV(hue, sat, val);
}

AnimationManager::AnimationManager()
{
  setAnimation(AnimationType::Idle, 0);
}

bool AnimationManager::setAnimation(AnimationType anim_type, int time_ms, CRGB color, int freq, int seed, int flags)
{
  if (time_ms == 0)
  {
    current_context.time_ms = INT32_MAX;
  }
  else
  {
    current_context.time_ms = time_ms;
  }
  current_context.color   = color;
  current_context.freq    = freq;
  current_context.seed    = seed;
  current_context.flags   = flags;
  
  int8_t index = static_cast<int8_t>(anim_type);
  
  if (index >= anim_count) {
    return false;
  }
  current_animation = animations[index].anim;
  cleanLEDs();
  anim_starting_time = millis();
  return true;
}

void AnimationManager::displayCurrentAnimation()
{
  if (current_animation != NULL)
  {
    if (millis() - anim_starting_time > current_context.time_ms)
    {
      setAnimation(AnimationType::Idle, 0);
    }
    current_animation(&current_context);
  }
}

void AnimationManager::cleanLEDs()
{
  for (int j = 0; j < NUM_LEDS_PER_STRIP; j++)
  {
    set_led(0, j, CRGB::Black);
    set_led(1, j, CRGB::Black);
  }
  LEDS.show();
}

void BootAnimation(AnimationContext* ctx)
{  
  for (int j = 0; j < NUM_LEDS_PER_STRIP; j++) {
    set_led(0, j, CRGB::Black);
    set_led(1, j, CRGB::Black);
  }
            
  LEDS.show();

  for (int j = 0; j < NUM_LEDS_PER_STRIP - 2; j++) {
    set_led(0, j, CRGB::White);
    set_led(1, j, CRGB::White);
    LEDS.show();
    LEDS.delay(50 * NUM_LEDS_PER_STRIP - 50 * j);
  }

  for (int j = NUM_LEDS_PER_STRIP - 3; j < NUM_LEDS_PER_STRIP; j++)
  {
    LEDS.delay(100 * NUM_LEDS_PER_STRIP - 100 * j);
    set_led(0, j, CRGB::White);
    set_led(1, j, CRGB::White);
    LEDS.show();
  }

  LEDS.delay(500);

  for (int j = 0; j < NUM_LEDS_PER_STRIP; j++)
  {
    set_led(0, j, CRGB::Green);
    set_led(1, j, CRGB::Green);
  }

  LEDS.show();
  LEDS.delay(2000);
}

void CommEstablishedAnimation(AnimationContext* ctx)
{
  static int on = 0;
  
  static NonBlockingWait timer;

  timer.wait_time = 1000000 / ctx->freq;

  if (timer.wait() == false)
  {
    return;
  }
  
  on ^= 1;
  if (on == 1) {
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++)
    {
      set_led(0, j, ctx->color);
      set_led(1, j, ctx->color);
    }
  }
  else {
    for (int j = 0; j < NUM_LEDS_PER_STRIP; j++)
    {
      set_led(0, j, CRGB::Black);
      set_led(1, j, CRGB::Black);
    }
  } 
  LEDS.show();  
}

void RandomNoiseAnimation(AnimationContext* ctx)
{ 
  static uint8_t hue = 0;
  static int8_t  dir   = 1;
  static uint8_t index = 0;

  static uint8_t max_index = NUM_LEDS_PER_STRIP - 1;
  static uint8_t min_index = 0;

  static uint32_t current_delay_time = 10;

  static NonBlockingWait timer;

  if (timer.wait() == false)
  {
    return;
  }

  for (int j = 0; j < NUM_LEDS_PER_STRIP; j++)
  {
    set_led(0, j, CRGB::Black);
    set_led(1, j, CRGB::Black);
  }

  index += dir;

  current_delay_time = 20;

  if (dir == 1 && index >= max_index )
  {
    dir = -1;
    index = max_index - 1;

    uint8_t ran = rand();
    max_index = ran / 31;

    current_delay_time *= 3;
    current_delay_time += 10;
  }
  else if (dir == -1 && index <= min_index)
  {
    dir = 1;
    index = min_index;
  }

  index %= NUM_LEDS_PER_STRIP;
  
  hue = ctx->color.raw[0] + index * 3;
  //hue = 0 + index * 3;

  for (int j = 0; j < index + 1; j++)
  {
    uint8_t max_val = 255;
    uint8_t min_val = 100;
    int value = max_val - ((max_val - min_val) * 1000) / (index * 1000) * j;

    set_led_hsv(0,j,hue,255,value);
    set_led_hsv(1,j,hue,255,value);
  }
  
  timer.wait_time = current_delay_time;
  LEDS.show();
}

void IdleAnimation(AnimationContext* ctx)
{

}
