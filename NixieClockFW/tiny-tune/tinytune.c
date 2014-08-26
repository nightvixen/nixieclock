#include "settings.h"
#define SAMPLE_CLOCK (F_CPU / 8)
#define SAMPLE_CLOCK_DIVIDER SAMPLE_CLOCK/SAMPLE_RATE
#define OUTPUT_SCALE_SHIFT 3
#define TIMEOUT F_CPU / SAMPLE_RATE

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/portpins.h>
#include <inttypes.h>
#include <string.h>
#include "tinytune.h"

struct TTVOICE voices[N_VOICES];
volatile uint32_t sample_cnt = 0;
int16_t sample_buffer[SAMPLE_BUFFER];
uint8_t sample_buf_clock = SAMPLE_BUFFER - 1;
uint8_t sample_update_idx = 0;

void _setDutyTRI(struct TTVOICE* v, uint8_t duty);

// Gosh.. a mul instruction would be great.
// May be best to precompute this when volume is set, since that can be slow..
inline int16_t four_bit_scale(int16_t input, uint8_t scale) {
  if (scale == 0x10)
    return input;
  if (scale == 0)
    return 0;
  int16_t output = 0;
  asm volatile(
      "swap %[scale]\n"
      "lsl %[scale]\n"
      "brcc j1\n"
      "add %A[output], %A[input]\n"
      "adc %B[output], %B[input]\n"
      "j1:\n"
      "lsl %A[output]\n"
      "rol %B[output]\n"
      "lsl %[scale]\n"
      "brcc j2\n"
      "add %A[output], %A[input]\n"
      "adc %B[output], %B[input]\n"
      "j2:\n"
      "lsl %A[output]\n"
      "rol %B[output]\n"
      "lsl %[scale]\n"
      "brcc j3\n"
      "add %A[output], %A[input]\n"
      "adc %B[output], %B[input]\n"
      "j3:\n"
      "lsl %A[output]\n"
      "rol %B[output]\n"
      "lsl %[scale]\n"
      "brcc j4\n"
      "add %A[output], %A[input]\n"
      "adc %B[output], %B[input]\n"
      "j4:\n"
      :
      [output] "+r" (output):
      [input] "r" (input),
      [scale] "r" (scale)
  );
  return output >> 4;
}

/*
void initPLL(void) {
  PLLCSR = 1 << PLLE; //Enable PLL
  _delay_us(100); // Wait for a sync;
  while (!(PLLCSR & (1 << PLOCK))) {
  };
  // PLL Synced
  PLLCSR |= (1 << PCKE); // Enable Asynchronous PCK mode.
}
*/



void do_song_tick();

#define SYNTH_TASK 1
#define SONG_TASK 2
volatile uint8_t task_bits = 0;

ISR(TIMER1_OVF_vect)

{
  OCR1A = sample_buffer[sample_buf_clock++];
  ++sample_cnt;
  ++song_info.tick_smp_count;
  if (sample_buf_clock == SAMPLE_BUFFER)
    sample_buf_clock = 0;
  uint8_t fill_sample_buffer = 0;
  if (sample_buf_clock == 0) {
    fill_sample_buffer = 1;
    sample_update_idx = SAMPLE_BUFFER / 2;
  } else if (sample_buf_clock == SAMPLE_BUFFER / 2) {
    fill_sample_buffer = 1;
    sample_update_idx = 0;
  }

  if (fill_sample_buffer && !(task_bits & SYNTH_TASK)) {
    task_bits |= SYNTH_TASK;
  //  PORTB |= 1;
    sei();
    memset(&sample_buffer[sample_update_idx], 0, SAMPLE_BUFFER);
    for (uint8_t i = 0; i < N_VOICES; ++i) {
      if (voices[i].enabled)
        voices[i]._getSample(&voices[i]);
    }
    for (uint8_t i = 0; i < SAMPLE_BUFFER / 2; ++i) {
      int16_t tmp = sample_buffer[sample_update_idx + i] >> OUTPUT_SCALE_SHIFT;
      tmp = MIN(tmp, 0x7f);
      tmp = MAX(tmp, -0x7f);
      sample_buffer[sample_update_idx + i] = tmp + 0x80;
    }
  //  PORTB &= ~1;
    task_bits &= ~SYNTH_TASK;
    return;
  }
  if (song_info.playing && song_info.tick_smp_count
      > song_info.samples_per_tick && !(task_bits & SONG_TASK) && !(task_bits
      & SYNTH_TASK)) {
    //task_bits &= ~SYNTH_TASK;
    // Re enable sample interrupts while we 'do' the song tick.
    // Note sets have division and stuff, so it might take a little while.
    // If it takes longer than the tick itself, then you have problems.
    task_bits |= SONG_TASK;
    sei();
   // PORTB |= 2;
    song_info.tick_smp_count = 0;
    do_song_tick();
  //  PORTB &= ~2;
    task_bits &= ~SONG_TASK;
  }
}

void waitMS(uint16_t ms) {
  uint32_t dest = sample_cnt + (SAMPLE_RATE / 1000) * ms;
  do {
  } while (sample_cnt < dest);
}
void initPWMB(void) {
 /* TCCR1B = (1 << WGM12)|(1 << CS10); // Run at PCK/1
  //GTCCR = (1 << PWM1B) | (1 << COM1B0); //Enable PWMB (pb4)
  // PWM CTC ?
  TCCR1A =  (1 < COM1A0);
  DDRB |= (1 << PB1); // Output on pb4
  //OCR1A = 255;
  //OCR1C = 0xff;
  */
    /* use OC1A pin */
    DDRB |= _BV(PB1);

    /* 
    * clear OC1A/OC1B on compare match 
    * set OC1A/OC1B at BOTTOM, non-inverting mode
    * Fast PWM, 8bit
    */
    TCCR1A = _BV(COM1A1) | _BV(WGM10);
    
    /* 
    * Fast PWM, 8bit
    * Prescaler: clk/1 = 8MHz
    * PWM frequency = 8MHz / (255 + 1) = 31.25kHz
    */
    TCCR1B = _BV(WGM12) | _BV(CS11);	
    
    /* set initial duty cycle to zero */
    OCR1A = 255;
}

void initTinyTune() {
  for (uint8_t i = 0; i < N_VOICES; ++i)
    initVoicePWM(i);
  //initPLL();
  initPWMB();
  song_info.playing = 0;

}

void _getSamplePWM(struct TTVOICE* v) {
  for (uint8_t i = 0; i < SAMPLE_BUFFER / 2; ++i) {
    int8_t outp = v->_s_volume;
    v->_err += v->hz;
    if (v->_err >= SAMPLE_RATE) {
      v->_err -= SAMPLE_RATE;
    }
    if (v->_err >= v->_params.pwm.duty_period) {
      outp = -outp;
    }

    sample_buffer[i + sample_update_idx] += outp;
  }
}

inline uint8_t prnd() {
  static volatile uint8_t x = ~0;
  x += TCNT1;
  x ^= 0xb5;
  return x;
}

void _getSampleNOISE(struct TTVOICE* v) {
  int8_t val = v->_params.pwm.duty_period;
  uint16_t hz4 = v->hz << 4;
  for (uint8_t i = 0; i < SAMPLE_BUFFER / 2; ++i) {
    v->_err += hz4;
    if (v->_err >= SAMPLE_RATE) {
      v->_err -= SAMPLE_RATE;
      val = four_bit_scale(prnd(), (v->_s_volume >> 3));
    }
    sample_buffer[sample_update_idx + i] += val;
  }
  v->_params.pwm.duty_period = val;
}

void _getSamplePWM_ASM(struct TTVOICE* v) {
  asm volatile (
      "samp_loop_p:\n"
      "movw %[vol], %[s_vol]\n"
      "cp %A[err],  %A[s_rate]\n"
      "cpc %B[err], %B[s_rate]\n"
      "brlo no_new_cycle\n"
      "sub %A[err], %A[s_rate]\n"
      "sbc %B[err], %B[s_rate]\n"
      "no_new_cycle:\n"
      "add %A[err], %A[hz]\n"
      "adc %B[err], %B[hz]\n"
      "cp %A[err],  %A[duty_pd]\n"
      "cpc %B[err], %B[duty_pd]\n"
      "brlo no_vol_flip\n"
      "movw %[vol], %[ns_vol]\n"
      "no_vol_flip:\n"
      "ld __tmp_reg__, %a[out_buf]\n"
      "add  __tmp_reg__, %A[vol]\n"
      "st %a[out_buf]+, __tmp_reg__\n"
      "ld __tmp_reg__, %a[out_buf]\n"
      "adc __tmp_reg__,  %B[vol]\n"
      "st %a[out_buf]+, __tmp_reg__\n"
      "subi %[buf_len], 1\n"
      "brne samp_loop_p\n":
      [err] "+w" (v->_err):
      [hz] "r" (v->hz),
      [s_vol] "r" ((uint16_t)v->_s_volume),
      [ns_vol] "r" ((uint16_t)-v->_s_volume),
      [vol] "r" ((uint16_t) 0),
      [s_rate] "r" ((uint16_t)SAMPLE_RATE),
      [buf_len] "r" (SAMPLE_BUFFER / 2),
      [out_buf] "e" (&sample_buffer[sample_update_idx]),
      [duty_pd] "r" (v->_params.pwm.duty_period)
  );
}

void _getSampleTRI(struct TTVOICE* v) {
  uint8_t lp = SAMPLE_BUFFER / 2;
  uint8_t i = sample_update_idx;
  uint16_t err = v->_err;
  int16_t lev = v->_params.tri.level;
  int16_t fp_vol = v->_params.tri.fp_vol;
  uint16_t rise_p = v->_params.tri.rise_period;
  uint16_t rslp = v->_params.tri.rise_slp;
  uint16_t fslp = v->_params.tri.fall_slp;
  uint16_t bcrunch = v->bcrunch;
  do {

    if (err >= SAMPLE_RATE) {
      err -= SAMPLE_RATE;
      lev = fp_vol;
    }
    if (err < rise_p) {
      lev += rslp;
    } else {
      lev += fslp;
    }
    err += v->hz;
    sample_buffer[i++] += (lev >> 7) & bcrunch;
  } while (--lp != 0);
  v->_params.tri.level = lev;
  v->_err = err;
}

void _getSampleTRI_ASM(struct TTVOICE* v) {
  asm volatile (
      "samp_loop:\n"
      "cp %A[err], %A[s_rate]\n"
      "cp %B[err], %B[s_rate]\n"
      "brlo skip_period\n"
      "; Reset err term and level\n"
      "sub %A[err], %A[s_rate];   reset err to period for countdown\n"
      "sbc %B[err], %B[s_rate];   reset err to period for countdown\n"
      "movw %[lev], %[fp_vol]\n"
      "skip_period:\n"
      "add %A[err], %A[hz]\n"
      "adc %B[err], %B[hz]\n"
      "cp %A[err],%A[rise_p]\n"
      "cpc %B[err],%B[rise_p]\n"
      "brlo rising\n"
      "add %A[lev], %A[fall_slp]\n"
      "adc %B[lev], %B[fall_slp]\n"
      "rjmp done_slopes\n"
      "rising:\n"
      "add %A[lev], %A[rise_slp]\n"
      "adc %B[lev], %B[rise_slp]\n"
      "done_slopes:\n"
      "movw r2,%[lev]\n"
      "lsl r2\n" // signed right shift by 7 voodoo
      "mov r2, r3\n"
      "rol r3\n"
      "sbc r3, r3\n"
      "lsl r2\n" //This can be optimized into the above shift..
      "rol r3\n"
      "ld __tmp_reg__, %a[out_buf]\n"
      "add r2, __tmp_reg__\n"
      "st %a[out_buf]+, r2\n"
      "ld __tmp_reg__, %a[out_buf]\n"
      "adc __tmp_reg__, r3\n"
      "st %a[out_buf]+, __tmp_reg__\n"
      "subi %[buf_len], 1\n"
      "brne samp_loop\n":
      [err] "+w" (v->_err),
      [lev] "+w" (v->_params.tri.level):
      [buf_len] "r" (SAMPLE_BUFFER / 2),
      [out_buf] "e" (&sample_buffer[sample_update_idx]),
      [s_rate] "r" ((uint16_t)SAMPLE_RATE),
      [hz] "r" ((uint16_t)v->hz),
      [fp_vol] "r" (v->_params.tri.fp_vol),
      [rise_p] "r" (v->_params.tri.rise_period),
      [rise_slp] "r" (v->_params.tri.rise_slp),
      [fall_slp] "r" (v->_params.tri.fall_slp),
      [bcrunch] "r" (v->bcrunch)
      :"r2", "r3");
}

void _setDutyPWM(struct TTVOICE* v, uint8_t duty) {
  if (duty < 0x80)
    duty = 0x80 + (0x79 - duty);
  v->_params.pwm.duty = duty;
  v->_params.pwm.duty_period
      = (((uint32_t) duty * (uint32_t) SAMPLE_RATE) >> 8);
}

void _setPitchPWM(struct TTVOICE* v, uint16_t pitch) {
  v->hz = pitch;
  v->_period = SAMPLE_RATE / pitch;
  _setDutyPWM(v, v->_params.pwm.duty);
}

void _setDutyNOISE(struct TTVOICE* v, uint8_t duty) {

}

void _setPitchNOISE(struct TTVOICE* v, uint16_t pitch) {
  v->hz = pitch;
}

void _setPitchTRI(struct TTVOICE* v, uint16_t pitch) {
  v->hz = pitch;
  v->_period = SAMPLE_RATE / pitch;
  _setDutyTRI(v, v->_params.pwm.duty);
}

void _setEnable(struct TTVOICE* v, bool enable) {
  if (v->_envelope && !v->gliding) {
    if (enable) {
      v->enabled = enable;
      v->_env_volume = v->_envelope->starting_level << 7;
      v->_env_idx = 0;
      v->_env_ticks = 0;
      v->_env_ticks_left_for_node = v->_envelope->point_ticks[0];
      v->sustaining = 0;
    } else {
      // note disables do note effect envelopes.
      // If the env volume hits zero the channel will be disabled.
      v->sustaining = 0;
      v->_env_ticks++;
      if (v->_env_ticks < v->_envelope->sustain_tick)
        v->_env_ticks = v->_envelope->sustain_tick + 1;
    }
  } else {
    v->enabled = enable;
  }
}

void _setVolume(struct TTVOICE* v, uint8_t volume) {
  v->volume = volume;
  if (v->_envelope) {
    v->_s_volume = ((v->_env_volume >> 7) * (volume >> 1)) >> 8;
  } else
    v->_s_volume = volume >> 1;
}

void _setDutyTRI(struct TTVOICE* v, uint8_t duty) {
  if (duty < 0x80)
    duty = 0x80 + (0x79 - duty);
  v->_params.tri.duty = duty;
  uint16_t rise_len = (((uint32_t) duty * v->_period) >> 8);
  rise_len = MAX(1, rise_len);
  rise_len = MIN(v->_period - 1, rise_len);
  v->_params.tri.rise_period
      = (((uint32_t) duty * (uint32_t) SAMPLE_RATE) >> 8);
  v->_params.tri.rise_slp = -(v->_params.tri.fp_vol << 1) / rise_len;
  v->_params.tri.fall_slp = (((int32_t) v->_params.tri.fp_vol << 1)
      / (v->_period - rise_len));
}

void _setVolumeTRI(struct TTVOICE* v, uint8_t volume) {
  _setVolume(v, volume);
  v->_params.tri.fp_vol = -(v->_s_volume << 7);
  _setDutyTRI(v, v->_params.tri.duty);
}

void initVoiceCommon(struct TTVOICE* v) {
  v->enabled = 0;
  v->_err = 0;
  v->_setDuty(v, 0x80);
  v->_setVolume(v, 0xb0);
  v->_setPitch(v, 440);
  v->_setEnable(v, 0);
  v->_envelope = 0;
  v->gliding = 0;
  v->f_hz = 0;
  v->bcrunch = 0xffff;
}

void initVoicePWM(uint8_t voice) {
  struct TTVOICE* v = &voices[voice];
  v->voice_type = TT_PWM;
  v->_getSample = &_getSamplePWM_ASM;
  v->_setEnable = &_setEnable;
  v->_setDuty = &_setDutyPWM;
  v->_setPitch = &_setPitchPWM;
  v->_setVolume = &_setVolume;
  initVoiceCommon(v);
}

void initVoiceTRI(uint8_t voice) {
  struct TTVOICE* v = &voices[voice];
  v->voice_type = TT_TRI;
  v->_getSample = &_getSampleTRI_ASM;
  v->_setEnable = &_setEnable;
  v->_setDuty = &_setDutyTRI;
  v->_setPitch = &_setPitchTRI;
  v->_setVolume = &_setVolumeTRI;
  initVoiceCommon(v);
}

void initVoiceNOISE(uint8_t voice) {
  struct TTVOICE* v = &voices[voice];
  v->voice_type = TT_NOISE;
  v->_getSample = &_getSampleNOISE;
  v->_setEnable = &_setEnable;
  v->_setDuty = &_setDutyNOISE; //Nop
  v->_setPitch = &_setPitchNOISE;
  initVoiceCommon(v);
}

void setVolume(uint8_t voice, uint8_t volume) {
  struct TTVOICE* v = &voices[voice];
  v->_setVolume(v, volume);
}
void setDuty(uint8_t voice, uint8_t duty) {
  struct TTVOICE* v = &voices[voice];
  v->_setDuty(v, duty);
}

void setPitch(uint8_t voice, uint16_t pitch) {
  struct TTVOICE* v = &voices[voice];
  if (!v->gliding)
    v->_setPitch(v, pitch);
  else
    // If we're gliding, don't set the pitch now, set the destination.
    // The tick handler will call _setPitch.
    v->porta_pitch = pitch;
}

void setEnable(uint8_t voice, bool enable) {
  struct TTVOICE* v = &voices[voice];
  v->_setEnable(v, enable);
}

void setBitCrunch(uint8_t voice, uint8_t crunch) {
  struct TTVOICE* v = &voices[voice];
  v->bcrunch = 0xffff << (8 - crunch);
}
void setPortaRate(uint8_t voice, uint16_t p_rate) {
  struct TTVOICE* v = &voices[voice];
  v->porta_rate = p_rate;
}

void setPorta(uint8_t voice, bool enable) {
  struct TTVOICE* v = &voices[voice];
  v->gliding = enable;
}

// Call this with null to disable envelope.
void setEnvelope(uint8_t voice, struct TTENVELOPE* envelope) {
  struct TTVOICE* v = &voices[voice];
  if (v->_envelope != envelope && envelope) {
    v->_envelope = envelope;
    v->_env_volume = v->_envelope->starting_level;
    v->_env_idx = 0;
    v->_env_ticks = 0;
    v->_env_ticks_left_for_node = v->_envelope->point_ticks[0];
    v->sustaining = 0;
  }
  if (!envelope) {
    v->_envelope = envelope;
  }
}

//ticks_sec is in 12.4 fixed point
void playSong(struct song_definition* song) {
  // Yeah, fixed point.
  uint16_t ticks_sec = ((song->bpm << 4) / 60) * song->rows_per_beat
      * song->ticks_per_row;
  song_info.song_def = song;
  song_info.pat_idx = 0;
  song_info.order_idx = 0;
  song_info.cur_pattern = pgm_read_byte(&song->pattern_order[0]);
  song_info.tick = 0;
  song_info.next_tick = 0;
  song_info.tick_smp_count = 0;
  song_info.samples_per_tick = ((uint32_t) SAMPLE_RATE << 4) / ticks_sec;
  song_info.playing = 1;
}

// 14.2 fixed point note hz from c4-b4
uint16_t PROGMEM pitch_table[] = { 1046, 1108, 1174, 1244, 1318, 1396, 1479,
    1567, 1661, 1760, 1864, 1975 };

uint16_t get_pitch(uint8_t note_code) {
  // if this is too slow switch to 4.4 octave.note coding.
  // otherwise midi compatibility is nice.
  int8_t octave = (note_code / 12) - 7;
  uint8_t note = note_code % 12;
  uint16_t note_hz = pgm_read_word(&(pitch_table[note]));
  if (octave >= 0) {
    return note_hz << octave;
  } else {
    return note_hz >> -octave;
  }
}

void do_song_tick() {
  if (song_info.tick == 0) {
    char done_row = 0;
    // Porta must be re-enabled every row.
    for (int i = 0; i < N_VOICES; ++i)
      setPorta(i, 0);
    char
        * cur_pat =
            pgm_read_word(&(song_info.song_def->pattern_data[song_info.cur_pattern]));
    do {

      uint8_t code = pgm_read_byte(&(cur_pat[song_info.pat_idx++]));
      // Upper nibble is the voice id.
      uint8_t voice = code >> 4;
      // Lower nibble is the instruction.
      code = code & 0xf;
      switch (code) {
      case NOTE_ON_FULL_VOL:
        setVolume(voice, 0xff);
      case NOTE_ON: {
        uint8_t note = pgm_read_byte(&(cur_pat[song_info.pat_idx++]));
        // The top bit determines whether a volume follows the note-on.
        uint16_t pitch = get_pitch(note & 0x7f);
        setPitch(voice, pitch);
        setEnable(voice, 1);
        // if bit7 is set, trundle on into the volume code
        // Break otherwise.
        if ((note & 0x80) == 0) {
          //setVolume(voice, 0xff);
          break;
        }
      }
      case SET_VOL: {
        uint8_t volume = pgm_read_byte(&(cur_pat[song_info.pat_idx++]));
        setVolume(voice, volume);
      }
        break;
      case NOTE_OFF:
        setEnable(voice, 0);
        break;
      case ROW_ADV: {
        // We use voice for the number of rows to wait.
        // For more than 16.. wait twice.
        song_info.next_tick = (1 + voice) * song_info.song_def->ticks_per_row;
        done_row = 1;
      }
        break;
      case SET_ENV: {
        uint8_t env_id = pgm_read_byte(&(cur_pat[song_info.pat_idx++]));
        setEnvelope(voice, song_info.song_def->envelopes[env_id]);
      }
        break;
      case SET_GLIDE_SPEED: {
        uint8_t gl_low = pgm_read_byte(&(cur_pat[song_info.pat_idx++]));
        uint8_t gl_high = pgm_read_byte(&(cur_pat[song_info.pat_idx++]));
        setPortaRate(voice, gl_low | (gl_high << 8));
      }
        break;
      case PORTAMENTO: {
        setPorta(voice, 1);
      }
        break;
      }
    } while (!done_row);
  }

  if (song_info.pat_idx >= pgm_read_word(
      &(song_info.song_def->pattern_lengths[song_info.cur_pattern]))) {
    song_info.pat_idx = 0;
    song_info.order_idx++;
    if (song_info.order_idx >= song_info.song_def->num_patterns)
      song_info.order_idx = 0;
    song_info.cur_pattern
        = pgm_read_byte(&song_info.song_def->pattern_order[song_info.order_idx]);
  }

  if (++song_info.tick >= song_info.next_tick) {
    song_info.tick = 0;
  }

  // Now lets do our envelopes and effects!
  for (uint8_t i = 0; i < N_VOICES; ++i) {
    struct TTVOICE* v = &voices[i];
    if (v->gliding && v->hz != v->porta_pitch) {
      // add our fractional component back in.
      uint32_t fp_hz = (v->hz << 4) + v->f_hz;
      if (v->hz < v->porta_pitch) { // gliding up.
        uint32_t p_mul = 0x10000 + v->porta_rate;
        fp_hz = (fp_hz * p_mul) >> 16;
        v->hz = MIN(fp_hz >> 4, v->porta_pitch);
      } else { // Glide down.
        uint32_t p_mul = 0x10000 - v->porta_rate;
        fp_hz = (fp_hz * p_mul) >> 16;
        v->hz = MAX(fp_hz >> 4, v->porta_pitch);
      }
      v->f_hz = fp_hz & 0xf;
      v->_setPitch(v, v->hz);
    }
    if (v->enabled && voices[i]._envelope && v->_env_idx
        < v->_envelope->num_points) {
      if (v->_env_ticks == v->_envelope->sustain_tick) {
        v->sustaining = 1;
      }
      if (v->sustaining)
        continue; // Move along. We're sustaining!
      v->_env_ticks++;
      v->_env_volume += v->_envelope->env_slopes[v->_env_idx];
      v->_env_volume = MAX(0, v->_env_volume);
      v->_env_volume = MIN(0xff << 7, v->_env_volume);
      v->_setVolume(v, v->volume);
      if (--v->_env_ticks_left_for_node == 0) {
        v->_env_idx++;
        if (v->_env_idx == v->_envelope->num_points)
          continue;
        v->_env_ticks_left_for_node = v->_envelope->point_ticks[v->_env_idx];
      }
    } else {
      if (v->enabled && voices[i]._envelope) {
        if (v->_s_volume < 5)
          v->enabled = 0;
      }
    }
  }
}
