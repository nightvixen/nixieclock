
  #include "tinytune.h"
  // Envelopes
  
  static int16_t slopes_0[] = {-399, 932, -136};
  static uint8_t pticks_0[] = {82, 14, 96};
  static struct TTENVELOPE env_0 = {
    .num_points = 3,
    .starting_level = 255,
    .env_slopes = slopes_0,
    .point_ticks = pticks_0,
    .sustain_tick = 255
  };
  
  // Patterns
  static uint8_t pattern_0[] PROGMEM = {
8, 43, 21, 0, 16, 195, 192, 19, 37, 0, 32, 198, 192, 19, 16, 74, 19, 32, 75, 19,
16, 67, 19, 32, 70, 19, 16, 74, 19, 4, 160, 32, 75, 19, 8, 46, 16, 67, 19, 32,
70, 19, 16, 74, 19, 4, 160, 32, 75, 19, 8, 50, 16, 67, 19, 32, 70, 19, 16, 74,
19, 4, 160, 32, 75, 19, 8, 49, 16, 66, 19, 32, 69, 19, 16, 74, 19, 32, 75, 19,
0, 42, 16, 66, 19, 32, 69, 19, 16, 74, 19, 32, 75, 19, 16, 66, 19, 32, 69, 19,
16, 74, 19, 32, 75, 19, 16, 66, 19, 32, 69, 19, 16, 74, 19, 32, 75, 19
};
static uint8_t pattern_1[] PROGMEM = {
8, 41, 21, 0, 16, 193, 192, 19, 37, 0, 32, 196, 192, 19, 16, 74, 19, 32, 75, 19,
16, 65, 19, 32, 68, 19, 16, 74, 19, 4, 160, 32, 75, 19, 8, 41, 16, 65, 19, 32,
68, 19, 4, 160, 16, 74, 19, 8, 44, 32, 75, 19, 16, 65, 19, 4, 160, 32, 68, 19,
8, 49, 16, 74, 19, 32, 75, 3, 4, 160, 3, 8, 48, 16, 64, 19, 32, 67, 19, 16,
74, 19, 4, 160, 32, 75, 19, 8, 40, 16, 64, 19, 32, 67, 19, 16, 74, 19, 32, 75,
19, 16, 64, 19, 32, 67, 19, 16, 74, 19, 32, 75, 19, 16, 64, 19, 32, 67, 19, 16,
74, 19, 32, 75, 19
};
static uint8_t pattern_2[] PROGMEM = {
8, 39, 21, 0, 16, 191, 192, 19, 0, 38, 37, 0, 32, 195, 192, 19, 0, 39, 16, 72,
19, 32, 75, 19, 16, 63, 19, 32, 67, 19, 16, 72, 19, 4, 160, 32, 75, 19, 8, 43,
16, 63, 19, 32, 67, 19, 16, 72, 19, 0, 51, 32, 75, 19, 16, 63, 19, 32, 67, 19,
0, 50, 16, 72, 19, 32, 75, 19, 0, 38, 16, 62, 19, 0, 37, 32, 67, 19, 0, 38,
16, 72, 19, 32, 74, 19, 16, 62, 19, 32, 67, 19, 16, 72, 19, 4, 160, 32, 74, 19,
8, 43, 16, 62, 19, 32, 67, 19, 16, 72, 19, 0, 50, 32, 74, 19, 16, 62, 19, 32,
67, 19, 0, 49, 16, 72, 19, 32, 74, 19
};
static uint8_t pattern_3[] PROGMEM = {
8, 38, 21, 0, 16, 188, 192, 19, 0, 42, 37, 0, 32, 194, 192, 19, 0, 45, 16, 69,
19, 0, 42, 32, 72, 19, 0, 45, 16, 66, 19, 0, 48, 32, 69, 19, 0, 45, 16, 72,
19, 0, 48, 32, 75, 19, 0, 51, 16, 69, 19, 0, 48, 32, 72, 19, 0, 51, 16, 75,
19, 0, 54, 32, 72, 19, 0, 57, 16, 75, 19, 0, 54, 32, 78, 19, 0, 51, 16, 75,
19, 0, 48, 32, 78, 19, 0, 51, 16, 81, 19, 0, 48, 32, 78, 19, 0, 45, 16, 81,
19, 0, 42, 32, 84, 19
};

  static const uint8_t* p_dat[] PROGMEM = {pattern_0,
pattern_1,
pattern_2,
pattern_3};
  static const uint16_t p_len[] PROGMEM = {118, 125, 130, 106};
  static const uint8_t p_ord[] PROGMEM = {0, 1, 2, 3};
  static const struct TTENVELOPE* envs[] = {&env_0};
  struct song_definition zelda_song = {
  .pattern_data = p_dat,
  .num_patterns = 4,
  .pattern_lengths = p_len,
  .pattern_order = p_ord,
  .bpm = 170,
  .rows_per_beat = 4,
  .ticks_per_row = 12,
  .envelopes = envs,
  };
    