#ifndef __OES_DDS_H_
#define __OES_DDS_H_

#include <string.h>

/*This loads DDS textures for OPENGL ES*/

//SOURCE: https://gist.github.com/Luis-D/826683f5fcbea7c21a7555bc8ea0d6f4
//Forked from: https://gist.github.com/tilkinsc/d1a8a46853dea160dc86aa48618be6f9
GLuint OES_Texture_Load_DDS(const char* path,GLint MAG_FILTER, GLint MIN_FILTER) 
{
	FILE* f;
	f = fopen(path, "rb");
	if(f == 0)
		return 0;

	char filecode[4];
	fread(&filecode, 1, 4, f);
	if(strncmp(filecode, "DDS ", 4) != 0) {
		fclose(f);
		return 0;
	}

	unsigned char header[124];
	fread(&header, 1, 124, f);


	unsigned int width = (header[8]) | (header[9] << 8) | (header[10] << 16) | (header[11] << 24);
	unsigned int height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
	unsigned int linearSize = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
	unsigned int mipMapCount = (header[24]) | (header[25] << 8) | (header[26] << 16) | (header[27] << 24);

	
	char* fourCC = (char*)&(header[80]);

	
	unsigned int buffer_size = (mipMapCount > 1 ? linearSize + linearSize : linearSize);
	unsigned char* buffer =(unsigned char*) malloc(buffer_size * sizeof(unsigned char));
	if(buffer == 0) {
	
		return 0;
	}

	fread(buffer, 1, buffer_size, f);

	fclose(f);


	unsigned int blockSize;
	unsigned int format;
	if(strncmp(fourCC, "DXT1", 4) == 0) {
		format = 0x83f0;
		blockSize = 8;
	} else if(strncmp(fourCC, "DXT3", 4) == 0) {
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		blockSize = 16;
	} else if(strncmp(fourCC, "DXT5", 4) == 0) {
		format = 0x83f3;
		blockSize = 16;
	} else {
	
		free(buffer);
		return 0;
	}

	
	GLuint tid;
	glGenTextures(1, &tid);
	if(tid == 0) {
		free(buffer);
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, tid);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MAG_FILTER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MIN_FILTER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	unsigned int offset = 0;
	for (unsigned int level=0; level<mipMapCount && (width || height); level++) {
		
		if(width <= 4 || height <= 4) {
		
			break;
		}

	
		unsigned int size = ((width+3)/4) * ((height+3)/4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

	
		offset += size;


		width = width/2;
		height = height/2;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	free(buffer);
	return tid;

}

#endif 

