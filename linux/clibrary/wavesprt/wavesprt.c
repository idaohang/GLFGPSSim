/* wavesprt.c -- Support for .WAV 8-bit mono & stereo sound files

                allows scanning or writing a .WAV file serially, without
                buffering in memory, which allows small DOS programs
                to access .WAV data


                2/11/1995 by Gary Flispart
                8/16/2004 updated (GLF)       */

#include "wavesprt.h"


int wave_open(wave_block *wb, const char name[], const char spec[])

  {
   unsigned int wtemp;
   unsigned long dwtemp;

   unsigned long offset;
   unsigned long filesize;
   unsigned long chunksize;
   unsigned long tempsize;
   unsigned long temppos;

   char chunkwork[260];

   int needbytes;
   int bytesread;
   int preexist;


   wb->spec = 0;
   wb->unit = NULL;
   strncpy(wb->name,name,64);
   wb->name[64] = 0;


   preexist = FALSE;

   if (stricmp(spec,"a") == 0) /* open append */
     {
      if (NULL != (wb->unit = fopen(name,"rb")))  /* if preexists... */
        {
         fclose(wb->unit);
         wb->unit = NULL;
         preexist = TRUE;
        }

      printf("\nGOT PAST OPEN TEST -- preexist = %d\n",preexist);

      if ((preexist) || (spec[0] == 'A')) /* if upper case use preset specifiers
                                              or original specs IF file preexists */
        {
        }
      else /* use default specifiers */
        {
         /* write header for 8-bit mono as if data buffer contains 0 bytes */
         wb->wfb.channels = 1;
         wb->wfb.smplsec = 11025;
         wb->wfb.avgbytsec = 11025;
         wb->wfb.bitsmpl = 8;
         wb->wfb.blkalign = 1;
        }

      if (preexist)
        {
         if (NULL == (wb->unit = fopen(name,"r+b")))
           {
            return FALSE;
           }

         /* temporary dummy values until format is verified and loaded */
         wb->datasize = 70000L;
         wb->riffsize = 70036L;
         wb->position = 0;

         printf("\nTry RIFF\n");
         /* verify RIFF header and get file size */
         if (fread(chunkwork,4,1,wb->unit) != 1)
           {
            fclose(wb->unit);
            return FALSE;
           }
         if (strncmp(chunkwork,"RIFF",4) != 0)
           {
            printf("\nNot a RIFF file.\n\n");
            fclose(wb->unit);
            return FALSE;
           }
         if (fread(&filesize,4,1,wb->unit) != 1)
           {
            fclose(wb->unit);
            return FALSE;
           }

         printf("\nTry WAVE\n");
         /* verify WAVE format and get data size */
         if (fread(chunkwork,4,1,wb->unit) != 1)
           {
            fclose(wb->unit);
            return FALSE;
           }
         if (strncmp(chunkwork,"WAVE",4) != 0)
           {
            printf("\nNot a WAVE file.\n\n");
            fclose(wb->unit);
            return FALSE;
           }

         printf("\nTry fmt\n");
         /* begin expecting chunks -- first chunk should be "fmt " type);
         if (fread(chunkwork,4,1,wb->unit) != 1)
           {
            fclose(wb->unit);
            return FALSE;
           }
         if (strncmp(chunkwork,"fmt ",4) != 0)
           {
            printf("\nFirst chunk not fmt.\n\n");
            fclose(wb->unit);
            return FALSE;
           }

         if (fread(&chunksize,4,1,wb->unit) != 1)
           {
            fclose(wb->unit);
            return FALSE;
           }
         if (chunksize != 16)
           {
            printf("\n'fmt ' chunk wrong size.\n\n");
            fclose(wb->unit);
           }

         printf("\nTry PCM\n");
         if (fread(&wtemp,2,1,wb->unit) != 1)
           {
            fclose(wb->unit);
            return FALSE;
           }
         if (wtemp != 1)
           {
            printf("\nNot a PCM file.\n\n");
            fclose(wb->unit);
           }

         printf("\nTry fmt data\n");
         /* get format block data */
         if (fread(&(wb->wfb),2,1,wb->unit) != 1)
           {
            printf("\nNo format data loaded.\n\n");
            fclose(wb->unit);
            return FALSE;
           }

         /* now must skip intervening chunks until 'data' chunk found */
         offset = 36;
         strcpy(chunkwork,"    ");
         do
           {
            if (fseek(wb->unit,offset,0) != 0)
              {
               fclose(wb->unit);
               return FALSE;
              }
            if (fread(chunkwork,4,1,wb->unit) != 1)
              {
               fclose(wb->unit);
               return FALSE;
              }
            if (fread(&chunksize,4,1,wb->unit) != 1)
              {
               fclose(wb->unit);
               return FALSE;
              }
            offset = offset + chunksize + 8;
           }
         while (strncmp(chunkwork,"data",4) != 0);

         printf("\nFilesize: %ld     Data chunk 1 size: %ld\n\n",filesize,offset);

         offset = offset - chunksize;


         wb->datasize = chunksize;
         wb->riffsize = filesize;
         wb->begindata = offset;

         /* NOTE: this is where append first differs from read... */
         wb->position = wb->datasize;
         offset = offset + wb->datasize;

         printf("\noffset to FIRST data byte = %ld\n\n",wb->begindata);
         printf("\noffset to appended data byte = %ld\n\n",offset);

         if (fseek(wb->unit,offset,0) != 0)
           {
            fclose(wb->unit);
            return FALSE;
           }

         /* set up riffsize to be updated by wave_close() */
         wb->spec = 'A';
         wb->riffsize = 36;

         /* at this point file is ready to write at end of preexisting data */
        }
      else
        {
         if (NULL == (wb->unit = fopen(name,"wb")))
           {
            return FALSE;
           }

         wb->spec = 'A';
         wb->datasize = 0;
         wb->riffsize = 36;
         wb->position = 0;



         wb->begindata = wb->riffsize + 8;


         fputs("RIFF",wb->unit);
         fwrite(&(wb->riffsize),4,1,wb->unit);
         fputs("WAVEfmt ",wb->unit);
         dwtemp = 16;
         fwrite(&dwtemp,4,1,wb->unit);

         wtemp = 1;
         fwrite(&wtemp,2,1,wb->unit);

         fwrite(&(wb->wfb.channels),2,1,wb->unit);
         fwrite(&(wb->wfb.smplsec),4,1,wb->unit);
         fwrite(&(wb->wfb.avgbytsec),4,1,wb->unit);
         fwrite(&(wb->wfb.blkalign),2,1,wb->unit);
         fwrite(&(wb->wfb.bitsmpl),2,1,wb->unit);

         fputs("data",wb->unit);
         fwrite(&(wb->datasize),4,1,wb->unit);

         /* note -- at this point, file pointer is ready to write sound bytes
              this technique allows unbuffered file processing for large files */

        }

      return TRUE;
     }



   if (stricmp(spec,"w") == 0) /* open write */
     {
      if (spec[0] == 'W') /* if upper case use preset specifiers */
        {
        }
      else /* use default specifiers */
        {
         /* write header for 8-bit mono as if data buffer contains 0 bytes */
         wb->wfb.channels = 1;
         wb->wfb.smplsec = 11025;
         wb->wfb.avgbytsec = 11025;
         wb->wfb.bitsmpl = 8;
         wb->wfb.blkalign = 1;
        }

      if (NULL == (wb->unit = fopen(name,"wb")))
        {
         return FALSE;
        }


      wb->spec = 'W';
      wb->datasize = 0;
      wb->riffsize = 36;
      wb->position = 0;

      wb->begindata = wb->riffsize + 8;


      fputs("RIFF",wb->unit);
      fwrite(&(wb->riffsize),4,1,wb->unit);
      fputs("WAVEfmt ",wb->unit);
      dwtemp = 16;
      fwrite(&dwtemp,4,1,wb->unit);

      wtemp = 1;
      fwrite(&wtemp,2,1,wb->unit);

      fwrite(&(wb->wfb.channels),2,1,wb->unit);
      fwrite(&(wb->wfb.smplsec),4,1,wb->unit);
      fwrite(&(wb->wfb.avgbytsec),4,1,wb->unit);
      fwrite(&(wb->wfb.blkalign),2,1,wb->unit);
      fwrite(&(wb->wfb.bitsmpl),2,1,wb->unit);

      fputs("data",wb->unit);
      fwrite(&(wb->datasize),4,1,wb->unit);

      /* note -- at this point, file pointer is ready to write sound bytes
           this technique allows unbuffered file processing for large files */

      return TRUE;
     }

   if (stricmp(spec,"r") == 0) /* open read -- TEMPORARY HACK! NEEDS WORK! */
     {
      wb->spec = 'R';
      if (NULL == (wb->unit = fopen(name,"rb")))
        {
         return FALSE;
        }

      /* default header for 8-bit mono as if data buffer contains 70000 bytes */
      wb->wfb.channels = 1;
      wb->wfb.smplsec = 11025;
      wb->wfb.avgbytsec = 11025;
      wb->wfb.blkalign = 1;
      wb->wfb.bitsmpl = 8;
      wb->datasize = 70000L;
      wb->riffsize = 70036L;
      wb->position = 0;

      printf("\nTry RIFF\n");
      /* verify RIFF header and get file size */
      if (fread(chunkwork,4,1,wb->unit) != 1)
        {
         fclose(wb->unit);
         return FALSE;
        }
      if (strncmp(chunkwork,"RIFF",4) != 0)
        {
         printf("\nNot a RIFF file.\n\n");
         fclose(wb->unit);
         return FALSE;
        }
      if (fread(&filesize,4,1,wb->unit) != 1)
        {
         fclose(wb->unit);
         return FALSE;
        }

      printf("\nTry WAVE\n");
      /* verify WAVE format and get data size */
      if (fread(chunkwork,4,1,wb->unit) != 1)
        {
         fclose(wb->unit);
         return FALSE;
        }
      if (strncmp(chunkwork,"WAVE",4) != 0)
        {
         printf("\nNot a WAVE file.\n\n");
         fclose(wb->unit);
         return FALSE;
        }

      printf("\nTry fmt\n");
      /* begin expecting chunks -- first chunk should be "fmt " type);
      if (fread(chunkwork,4,1,wb->unit) != 1)
        {
         fclose(wb->unit);
         return FALSE;
        }
      if (strncmp(chunkwork,"fmt ",4) != 0)
        {
         printf("\nFirst chunk not fmt.\n\n");
         fclose(wb->unit);
         return FALSE;
        }

      if (fread(&chunksize,4,1,wb->unit) != 1)
        {
         fclose(wb->unit);
         return FALSE;
        }
      if (chunksize != 16)
        {
         printf("\n'fmt ' chunk wrong size.\n\n");
         fclose(wb->unit);
        }

      printf("\nTry PCM\n");
      if (fread(&wtemp,2,1,wb->unit) != 1)
        {
         fclose(wb->unit);
         return FALSE;
        }
      if (wtemp != 1)
        {
         printf("\nNot a PCM file.\n\n");
         fclose(wb->unit);
        }

      printf("\nTry fmt data\n");
      /* get format block data */
      if (fread(&(wb->wfb),2,1,wb->unit) != 1)
        {
         printf("\nNo format data loaded.\n\n");
         fclose(wb->unit);
         return FALSE;
        }

      /* now must skip intervening chunks until 'data' chunk found */
      offset = 36;
      strcpy(chunkwork,"    ");
      do
        {
         if (fseek(wb->unit,offset,0) != 0)
           {
            fclose(wb->unit);
            return FALSE;
           }
         if (fread(chunkwork,4,1,wb->unit) != 1)
           {
            fclose(wb->unit);
            return FALSE;
           }
         if (fread(&chunksize,4,1,wb->unit) != 1)
           {
            fclose(wb->unit);
            return FALSE;
           }
         offset = offset + chunksize + 8;
        }
      while (strncmp(chunkwork,"data",4) != 0);

      printf("\nFilesize: %ld     Data chunk 1 size: %ld\n\n",filesize,offset);

      offset = offset - chunksize;


      wb->datasize = chunksize;
      wb->riffsize = filesize;
      wb->position = 0;
      wb->begindata = offset;


      printf("\noffset to first data byte = %ld\n\n",wb->begindata);

      if (fseek(wb->unit,offset,0) != 0)
        {
         fclose(wb->unit);
         return FALSE;
        }
      return TRUE;
     }
   return FALSE;
  }


void wave_specify(wave_block *wb, int chls, int bits, long freq)

  {
   /* alter header for other than default 8-bit mono 11 khz */
   /* support mono or stereo, 8 or 16-bit */

   if ((freq >= 4000L) && (freq <= 44100L))
     {
      wb->wfb.smplsec = freq;
     }
   else
     {
      wb->wfb.smplsec = 11025;
     }

   wb->wfb.channels = 1;
   wb->wfb.blkalign = 1;
   if (chls == 1)
     {
      wb->wfb.avgbytsec = wb->wfb.smplsec;
     }
   else
     {
      wb->wfb.avgbytsec = wb->wfb.smplsec * 2;
      wb->wfb.channels = 2;
      wb->wfb.blkalign = 2;
     }

   wb->wfb.bitsmpl = 8;
   if (bits == 8)
     {
     }
   else
     {
      wb->wfb.bitsmpl = 16;
      wb->wfb.avgbytsec *= 2;
      wb->wfb.blkalign *= 2;
     }
  }







void wave_close(wave_block *wb)

  {
   unsigned int wtemp;
   unsigned long dwtemp;
   long int offset;

   if (wb->spec == 0)
      return;

   if (wb->spec == 'R')
     {
      fclose(wb->unit);
     }

   if ((wb->spec == 'W') || (wb->spec == 'A'))
     {
      if (wb->datasize > 0)
        {
         printf("\ndatasize = %ld\n\n",wb->datasize);
         /* go back to beginning of file and update length fields */
         wb->riffsize += wb->datasize;
         offset = 4;
         if (fseek(wb->unit,offset,0) == 0)
           {
            fwrite(&(wb->riffsize),4,1,wb->unit);
           }
         offset = 40;
         if (fseek(wb->unit,offset,0) == 0)
           {
            fwrite(&(wb->datasize),4,1,wb->unit);
           }
        }
      fclose(wb->unit);
     }

   wb->unit = NULL;
   wb->spec = 0;
  }


void wave_seek(wave_block *wb, long doffset)
  {
   doffset = doffset + wb->begindata;
   if (doffset <= (wb->datasize + wb->begindata))
     {
      fseek(wb->unit,doffset,0);
     }
  }


int wave_put(wave_block *wb, int value)    /* currently 8-bit only */

  {
   if ((wb->spec == 'W') || (wb->spec == 'A'))
     {
      fputc(value,wb->unit);
      (wb->position) += 1;
      if (wb->position > wb->datasize)
        {
         (wb->datasize) += 1;
        }
     }
   return TRUE;
  }


int wave_get(wave_block *wb)

  {
   int kar;

   if (wb->spec == 'R')
     {
      if ((wb->datasize == 0) || (wb->position < wb->datasize))
        {
         kar = fgetc(wb->unit);
         if (kar != EOF)
           {
            (wb->position) += 1;
           }
         else
           {
            wb->datasize = wb->position;   /* force to end reading */
            return EOF;
           }
         return kar;
        }
     }
   return EOF;
  }




