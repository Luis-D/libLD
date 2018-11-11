
#ifndef _LD_AUDIO_H
#define _LD_AUDIO_H

#ifdef __cplusplus
extern "C" 
{
#endif

void ChangeVolume16bits(void * dest, void * orig, int bytes, uint32_t Volumen);
void GenerateVolumeforFixedPoint16bitsStereo(float VolumenNormal, uint32_t * VolumenGenerado);

#ifdef __cplusplus
}
#endif

#endif