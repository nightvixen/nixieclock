/*
 * Created: 1/15/2012 11:34:34 PM
 *  Author: Blake Livingston
 */

#include <avr/io.h>
#include <avr/portpins.h>
#include "tinytune.h"

#define TEST_ZELDA

#ifdef TEST_WILY
#include "wily_song.h"
int main(void)
{
  initTinyTune();
  initVoiceTRI(3);
  setBitCrunch(3,4);
  playSong(&wily_song);
  while(1)
  {
  }
}
#endif

#ifdef TEST_LUNA
#include "luna_song.h"
int main(void)
{
  initTinyTune();
  setDuty(0, 0x6);
  playSong(&luna_song);
  while(1)
  {

  }
}
#endif

#ifdef TEST_ZELDA
#include "zelda_song.h"
int main(void)
{
  initTinyTune();
  initVoiceTRI(0);
  setBitCrunch(0,4);
  playSong(&zelda_song);
  while(1)
  {
  }
}
#endif

#ifdef TEST_STILL
#include "still_song.h"
int main(void)
{
  initTinyTune();
  initVoicePWM(4);
  initVoicePWM(1);
  setBitCrunch(4,4);
  setDuty(0,50);
  playSong(&still_song);
  while(1)
  {
  }
}
#endif

#ifdef TEST_FAXANADU
#include "faxanadu_song.h"
int main(void)
{
  initTinyTune();
  initVoiceTRI(2);
  setBitCrunch(2,4);
  playSong(&faxanadu_song);
  while(1)
  {
  }
}
#endif

#ifdef TEST_PITCHBEND
#include "pitch_song.h"
int main(void)
{
  initTinyTune();
  playSong(&pitch_song);
  while(1)
  {
  }
}
#endif

#ifdef TEST_DEMO
#include "demo1_song.h"
int main(void)
{
  initTinyTune();
  initVoiceTRI(4);
  initVoiceTRI(0);
  setDuty(1,0xe0);
  setDuty(2,0xe0);
  setBitCrunch(0,4);
  playSong(&demo1_song);
  short i = 0;

  while(1)
  {
  }
}
#endif

#ifdef TEST_NOISE
#include "noise_song.h"
int main(void) {
  initTinyTune();
  initVoiceNOISE(0);
  initVoiceNOISE(1);
  playSong(&noise_song);
  short i = 0;

  while (1) {

    //setPitch(0,i);
  }
}
#endif
