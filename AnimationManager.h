#ifndef ANIMATIONMANAGER_HEADER_GUARD
#define ANIMATIONMANAGER_HEADER_GUARD

#include <FastLED.h>

#include <cstdint>

#define NUM_LEDS_PER_STRIP 9
#define NUM_STRIPS 4
#define ACTUAL_STRIPS 2
#define PORT_SHIFT 2

#define ANIM_NUM 4

extern CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

struct NonBlockingWait {

  NonBlockingWait() : 
    NonBlockingWait(0) {};
  
  NonBlockingWait(int wait_time) : 
    is_waiting(false),
    first_entrance(0),
    waiting_interval(0),
    wait_time(wait_time) {};
  
  bool is_waiting      = false;
  int first_entrance   = 0;
  int waiting_interval = 0;
  int wait_time        = 0;
  
  bool wait()
  {
    if (is_waiting == false)
    {
      first_entrance   = millis();
      is_waiting       = true;
    }
    else
    {
      if (millis() - first_entrance > wait_time)
      {
        is_waiting = false;
        return true;
      }
    }
    return false;
  } 
};

typedef struct {
  int time_ms;
  CRGB color;
  int freq;
  int seed;
  int flags;
} AnimationContext;

void set_led(int i, int j, CRGB color);
void set_led_hsv(int i, int j, byte hue, byte sat, byte val);


void BootAnimation(AnimationContext* ctx);
void CommEstablishedAnimation(AnimationContext* ctx);
void RandomNoiseAnimation(AnimationContext* ctx);
void IdleAnimation(AnimationContext* ctx);

class AnimationManager
{
  public:

    typedef void (*AnimationFunc)(AnimationContext* ctx);

    enum class AnimationType {Boot = 0, CommEstablished = 1, RandomNoise = 2, Idle = 3};

    typedef struct {
      AnimationType type;
      AnimationFunc anim;
    } Animation;

    AnimationManager();
    ~AnimationManager() {};

    bool setAnimation(AnimationType anim_type, int time_ms = 1000, CRGB color = CRGB::Black, int freq = 1000, int seed = 0, int flags = 0);
    void displayCurrentAnimation();

  private:

    void cleanLEDs();

    Animation animations[ANIM_NUM] = {
      {AnimationType::Boot, BootAnimation},
      {AnimationType::CommEstablished, CommEstablishedAnimation},
      {AnimationType::RandomNoise, RandomNoiseAnimation},
      {AnimationType::Idle, IdleAnimation}
    };

    AnimationContext current_context;
    AnimationFunc current_animation;

    int anim_starting_time = 0;

    const int anim_count = ANIM_NUM;

};

#endif
