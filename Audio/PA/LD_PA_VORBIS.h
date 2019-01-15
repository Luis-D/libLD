/*This is a simplified API for PortAudio*/
/*Obviously it depends on the linkage of PortAudio*/
/*It uses 16bits samples*/

/* - Luis Delgado. 2018 */

#ifndef _LD_PA_VORBIS_H
#define _LD_PA_VORBIS_H

#include <stdio.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <portaudio.h>

#include <string.h>

#include "../LD_Audio.h"
#include "../LD_Utils.h"

#define SAMPLE_SIZE 256
#define SAMPLE_RATE 44100
#define Normal_Volumen 0x80008000


typedef struct AudioMixer{

    float Master_Volumen;
    int SampleSize;
    int Num_Buffers;

    struct _PA_SoundBuffer_struct_{

        char Flag;
        int SoundsInBuffer;
        float Volumen;

        struct _PA_SoundStruct_struct_{
            char Flag;
            float Volumen;
            uint32_t Volumen_FixedPoint;
            //uint64_t PCM_Samples;
            OggVorbis_File Audio;
        } * Sound;
    }* Buffer;

}AudioMixer;

/**
    FLAGs:
    -bit 0: 1= Occupied slot;           0= Free Slot.
    -bit 1: 1= Play;                    0= Do not play.
    -bit 2: 1= Play in loop;            0= Do not play in loop.
    -bit 3: 1= Status: Playing;         0= Status: Not playing.

    -bit 4: 1= Status: At Beginning;    0= Status: Not at beginning.
    -bit 5: 1= Tag Audio to restart
    -bit 6: 1= Tag to decodify but with no volume.
    -bit 7: 1= Status: End reached.
**/

#define  SizeofSoundBuffer sizeof(struct _PA_SoundBuffer_struct_)
#define  SizeofSound       sizeof(struct _PA_SoundStruct_struct_)

static int AudioMixerCallBack( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    (void) inputBuffer;
    AudioMixer * _AudioMixer_ = (AudioMixer *) userData;

    int err;
    int rest=0;
    int acu=0;
    int audio;
    short * out = (short* )outputBuffer;

    char tempout[SAMPLE_SIZE]; ///<- Search for a Dynamic solution
    char outbuff[SAMPLE_SIZE]; ///<- Search for a Dynamic solution

    struct _PA_SoundBuffer_struct_ * BufferPTR = _AudioMixer_->Buffer;
    struct _PA_SoundBuffer_struct_ * BufferPTREND = BufferPTR+_AudioMixer_->Num_Buffers-1;

    int NumeroDeAudios;
    OggVorbis_File * Audio;
    struct _PA_SoundStruct_struct_ * Sonido;

    char PrimeraMuestra = 1; uint32_t flag;

    char HaySonido = 0;

    for(struct _PA_SoundBuffer_struct_ * Ex = BufferPTR;Ex<=BufferPTREND;Ex++)
    {
        NumeroDeAudios= Ex->SoundsInBuffer;
        Sonido = Ex->Sound;

        struct _PA_SoundStruct_struct_ * UltimoSonido = &Ex->Sound[NumeroDeAudios];

        for (struct _PA_SoundStruct_struct_ * Sx = Sonido;Sx<UltimoSonido;Sx++)
        {
            acu=0;
            flag = (uint32_t) Sx->Flag;
            Audio = &(Sx->Audio);

            if((flag&32)==32)
            {
                ov_pcm_seek(Audio,0);
                Sx->Flag|=16;
                Sx->Flag&=223;
            }

            if((flag&3)==3)
            {
      

                if(Audio->os.e_o_s)
                {
                    if((flag&4)==4)
                    {
                        ov_pcm_seek(Audio,0);
                        Sx->Flag|=16;
                    }
                    else
                    {
      
                        Sx->Flag|=176;
                        Sx->Flag&=245;
                    }
                }

                err=ov_read(Audio,tempout,SAMPLE_SIZE,0,2,1,&audio);
                Sx->Flag|=8;
                Sx->Flag&=111;

                if(err<SAMPLE_SIZE)
                {
                  while(acu<SAMPLE_SIZE-err)
                  {
                      rest=ov_read(Audio,tempout+err+acu,SAMPLE_SIZE-err-acu,0,2,1,&audio);
                      acu+=rest;
                  }
                }

                if(!((flag&64) == 64))
                {
                    ChangeVolume16bits(tempout,tempout,SAMPLE_SIZE,Sx->Volumen_FixedPoint);

                    if((flag&11)==11){HaySonido=1;}

                    if(PrimeraMuestra == 1)
                    {
                        fastmemcpy  (outbuff,tempout,SAMPLE_SIZE);
                        PrimeraMuestra = 0;
                    }
                    else
                    {
                        Buffer_16bits_ADD((uint16_t*)outbuff,(uint16_t*)tempout,SAMPLE_SIZE);
                    }
                }
            }
        }
    }


    if(HaySonido==1){fastmemcpy (out,outbuff,SAMPLE_SIZE);}
    else{Buffer_Clear(out,SAMPLE_SIZE);}

    return paContinue;
}

int AudioMixer_Generate(int Number_Of_Buffers,AudioMixer * _AudioMixer_)
{
    _AudioMixer_->Master_Volumen=1.f;
    _AudioMixer_->SampleSize=SAMPLE_SIZE;
    struct _PA_SoundBuffer_struct_ * tmp =
    (struct _PA_SoundBuffer_struct_*)malloc(Number_Of_Buffers * SizeofSoundBuffer);
    _AudioMixer_->Buffer=tmp;

    if(tmp == NULL ){_AudioMixer_->Num_Buffers=0;return 0;}
    _AudioMixer_->Num_Buffers=Number_Of_Buffers;

    struct _PA_SoundBuffer_struct_ * END = tmp+Number_Of_Buffers;
    for(;tmp<END;tmp++)
    {tmp->SoundsInBuffer=0;}

    return 1;
}

int AudioMixer_AlocateBuffer(int NumberOfAudios, struct _PA_SoundBuffer_struct_ * SoundBufferPTR)
{
    struct _PA_SoundStruct_struct_* tmp;
    tmp=(struct  _PA_SoundStruct_struct_*)malloc(SizeofSound* NumberOfAudios);
    SoundBufferPTR->Sound = tmp;

    if(tmp==NULL){return 0;}

    struct _PA_SoundStruct_struct_ * END = tmp+NumberOfAudios;
    for(;tmp<END;tmp++)
    {tmp->Flag=0;}

    SoundBufferPTR->Flag=0;
    SoundBufferPTR->Volumen = 1.f;
    SoundBufferPTR->SoundsInBuffer = NumberOfAudios;
    return 1;
}

void AudioMixer_ClearSound(struct _PA_SoundStruct_struct_ * Sound)
{ov_clear(&(Sound->Audio)); Sound->Flag=0;}

void AudioMixer_DealocateBuffer(struct _PA_SoundBuffer_struct_ * SoundBufferPTR)
{
    struct _PA_SoundStruct_struct_ * End;
    End = SoundBufferPTR->Sound+SoundBufferPTR->SoundsInBuffer-1;
    for(struct _PA_SoundStruct_struct_ * Ex = SoundBufferPTR->Sound;Ex<=End;Ex++)
    {AudioMixer_ClearSound(Ex);}

    free(SoundBufferPTR->Sound);
}

int AudioMixer_AddSound_file(const char * file, struct _PA_SoundStruct_struct_ * Sound,char FLAG)
{
    if(!(CheckBit(Sound->Flag ,0)))
    {
        FILE * infile; ///Check if this file gets closed sometime
        infile = fopen(file,"rb");
        int err = ov_open_callbacks(infile, &(Sound->Audio), NULL,0, OV_CALLBACKS_DEFAULT);
        if(err<0){printf("ERROR::AUDIO:: ");printf("%s ?\n",file);return 0;}

        //Sound->PCM_Samples = ov_pcm_total(&(Sound->Audio),-1);
        Sound->Volumen = 1.0f;
        Sound->Volumen_FixedPoint = Normal_Volumen;
        Sound->Flag = FLAG | 1;
        return 1;
    }
    return 0;
}


void AudioMixer_Update(AudioMixer * _AudioMixer_)
{
    struct _PA_SoundBuffer_struct_ * BufferPTR = (_AudioMixer_->Buffer);

    ///Update volumen///
    float tmpNewVolumen; float tmpAUDVolumen;
    for(int i = _AudioMixer_->Num_Buffers;i--;)
    {
        tmpNewVolumen = _AudioMixer_->Master_Volumen * BufferPTR->Volumen;

       struct  _PA_SoundStruct_struct_ * End;
        End = BufferPTR->Sound+BufferPTR->SoundsInBuffer-1;
        for(struct _PA_SoundStruct_struct_ * Ex = BufferPTR->Sound;Ex<=End;Ex++)
        {
            tmpAUDVolumen = tmpNewVolumen * Ex->Volumen;
            GenerateVolumeforFixedPoint16bitsStereo(tmpAUDVolumen,&Ex->Volumen_FixedPoint);
        }

        BufferPTR++;
    }
    ///------------------///
}


int Set_AudioMixer(AudioMixer * _AudioMixer_, PaStream   **stream )
{
    return Pa_OpenDefaultStream( stream,
                                0,
                                2,
                                paInt16,
                                SAMPLE_RATE,
                                SAMPLE_SIZE/4,
                                AudioMixerCallBack,
                                _AudioMixer_);
}

void LD_Audio_Mute(struct _PA_SoundStruct_struct_ * _Audio_)
{_Audio_->Flag |= 64;}

void LD_Audio_UnMute(struct _PA_SoundStruct_struct_ * _Audio_)
{_Audio_->Flag &= 191;}

void LD_Audio_ToggleMute(struct _PA_SoundStruct_struct_ * _Audio_)
{_Audio_->Flag ^= 64;}

void LD_Audio_Play_Start(struct _PA_SoundStruct_struct_ * _Audio_)
{
    _Audio_->Flag &= 191;
    _Audio_->Flag |= 34;
}

void LD_Audio_Play_Stop(struct _PA_SoundStruct_struct_ * _Audio_)
{ _Audio_->Flag &= 245;}

void LD_Audio_Play_Continue(struct _PA_SoundStruct_struct_ * _Audio_)
{_Audio_->Flag |= 2;}

#endif
