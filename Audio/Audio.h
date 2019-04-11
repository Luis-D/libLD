#ifndef _PHO_AUDIO_H_
#define _PHO_AUDIO_H_

#include "PA/LD_PA_VORBIS.h"
#include "../libLDExtra/LD_Tricks.h"

AudioMixer AudioSystem; 
PaStream * Stream;

void Pho_Audio_Init(){Pa_Initialize();}

void Pho_Audio_Configure(int BufferCount, int SoundsPerBuffer)
{
    Pa_CloseStream(Stream);



    AudioMixer_Generate( BufferCount,&AudioSystem);
    
    array_foreach(struct _PA_SoundBuffer_struct_, Item,AudioSystem.Buffer,BufferCount)
    {
	AudioMixer_AlocateBuffer(SoundsPerBuffer,Item); 
    }
    Set_AudioMixer( &AudioSystem, &Stream);

    Pa_StartStream( Stream );
}

#define Pho_Audio_AddSoundFile(file,buffer,sound,flag) \
     AudioMixer_AddSound_file(file,AudioSystem.Buffer[buffer].Sound+sound,flag)

#endif
