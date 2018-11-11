	
#ifndef _LD_UTILS_H
#define _LD_UTILS_H

#ifdef __cplusplus
extern "C" 
{
#endif

	void fastmemcpy(void * Destino, void * Origen, int bytes);
	void Buffer_16bits_ADD(uint16_t * A_Resultado, uint16_t * B, int bytes);
	void Buffer_Clear(void * ptr, int Size);
	char CheckBit(uint32_t INT32,uint8_t BIT);

#ifdef __cplusplus
}
#endif

#endif