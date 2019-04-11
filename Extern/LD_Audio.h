
#ifndef _LD_AUDIO_H
#define _LD_AUDIO_H

#ifdef __cplusplus
extern "C" 
{
#endif

void ChangeVolume16bits(void * dest, void * orig, int bytes, uint32_t Volumen);
void GenerateVolumeforFixedPoint16bitsStereo(float VolumenNormal, unsigned int * VolumenGenerado);
void  Buffer_16bits_ADD(void * Destiny, void * Origin,unsigned int bytesize);


#ifdef __cplusplus
}
#endif

#endif
