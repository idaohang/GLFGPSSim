/* wavsprt.h -- Support for .WAV 8-bit mono sound files (HEADER FILE)
                allows scanning or writing a .WAV file serially, without
                buffering in memory, which allows small DOS programs
                to access .WAV data
                2/11/95 by Gary Flispart */

typedef struct
  {
   unsigned int channels;
   unsigned long smplsec;
   unsigned long avgbytsec;
   unsigned int blkalign;
   unsigned int bitsmpl;
  }
   wave_fmt_block;

typedef struct
  {
   char name[65];
   FILE *unit;
   char spec;
   wave_fmt_block wfb;
   unsigned long datasize;
   unsigned long riffsize;
   unsigned long position;
  }
   wave_block;

int wave_open(wave_block *wb, const char name[], const char spec[]);
void wave_specify(wave_block *wb, int chls, int bits, long freq);
void wave_close(wave_block *wb);
int wave_put(wave_block *wb, int value);
int wave_get(wave_block *wb);


