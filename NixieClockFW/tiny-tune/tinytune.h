#ifndef __TINYTUNE__
#define __TINYTUNE__

#define NOTE_ON 0
#define NOTE_OFF 1
#define ROW_ADV 3
#define SET_VOL 4
#define SET_ENV 5
#define SET_GLIDE_SPEED 6
#define PORTAMENTO 7
#define NOTE_ON_FULL_VOL 8

#include <inttypes.h>
#include <avr/pgmspace.h>

void initTinyTune();

typedef char bool;

typedef uint8_t* PROGMEM songdata;

void initVoicePWM(uint8_t voice);
void initVoiceTRI(uint8_t voice);
void initVoiceNOISE(uint8_t voice);

struct TTENVELOPE;
void setVolume(uint8_t voice, uint8_t volume);
void setDuty(uint8_t voice, uint8_t duty);
void setPitch(uint8_t voice, uint16_t pitch);
void setEnable(uint8_t voice, bool enable);
void setEnvelope(uint8_t voice, struct TTENVELOPE* envelope);
void setBitCrunch(uint8_t voice, uint8_t crunch);
void setPortaRate(uint8_t voice, uint16_t p_rate);
void setPorta(uint8_t voice, bool enable);
void waitMS(uint16_t ms);

struct TTENVELOPE {
  int8_t num_points;
  // Fixed point 9.7 starting volume;
  uint8_t starting_level;
  // Fixed point slopes 9.7
  int16_t* env_slopes;
  // Number of ticks to run each slope.
  uint8_t* point_ticks;
  // Tick to halt envelope until note off.
  uint8_t sustain_tick;
};

struct TTVOICE {
  enum {
    TT_PWM, TT_TRI, TT_NOISE
  } voice_type;

  // Integer hz.
  uint16_t hz;
  // Fractional hz for portamento accuracy. 0.4 fp.
  uint8_t f_hz;
  // Volume.
  uint8_t volume;
  int8_t _s_volume;
  // Fixed 9.7 envelope volume
  int16_t _env_volume;
  // Index of the current envelope's progress
  uint8_t _env_idx;
  // Number of ticks remaining on the current envelope node.
  uint8_t _env_ticks_left_for_node;
  //total env_ticks
  uint8_t _env_ticks;
  // Have we hit the sustain_pt?
  uint8_t sustaining;
  uint8_t gliding;
  uint16_t porta_rate;
  uint16_t porta_pitch;

  struct TTENVELOPE* _envelope;
  // Voice enabled
  bool enabled;
  uint16_t _period;
  // error term.
  uint16_t _err;
  uint16_t bcrunch;
  void (*_getSample)(struct TTVOICE*);
  void (*_setVolume)(struct TTVOICE*, uint8_t);
  void (*_setDuty)(struct TTVOICE*, uint8_t duty);
  void (*_setPitch)(struct TTVOICE*, uint16_t pitch);
  void (*_setEnable)(struct TTVOICE*, bool enable);
  union {
    struct {
      uint8_t duty;
      uint16_t duty_period;
    } pwm;
    struct {
      uint8_t duty;
      uint16_t rise_period;
      // 9.7 bit fixed point.
      int16_t fp_vol;
      int16_t level;
      int16_t rise_slp;
      int16_t fall_slp;
    } tri;
  } _params;
};

struct song_definition {
  uint8_t** pattern_data;
  uint16_t* pattern_lengths;
  uint8_t num_patterns;
  uint8_t* pattern_order;
  uint16_t bpm;
  uint8_t rows_per_beat;
  uint8_t ticks_per_row;
  struct TTENVELOPE** envelopes;
};

struct song_info {
  uint16_t samples_per_tick;
  uint16_t tick;
  uint16_t next_tick;
  uint16_t tick_smp_count;
  struct song_definition* song_def;
  uint8_t cur_pattern;
  uint8_t order_idx;
  uint16_t pat_idx;
  bool playing;
} song_info;

void playSong(struct song_definition* song_def);

#endif
