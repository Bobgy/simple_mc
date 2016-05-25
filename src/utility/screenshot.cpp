#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "utility/screenshot.h"

#define BITMAP_ID 0x4D42;
 
bool OutputDataToBMPFile(std::string filename, unsigned char* data, unsigned int width, unsigned int height);
bool WriteBitmapFile(std::string filename, int width, int height, unsigned char * bitmapData);

bool ScreenShot(std::string filename){
	GLenum image_buffer;
	GLubyte* image_data = 0;
	unsigned long image_size;

	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	printf("w=%d,h=%d", width, height);
	image_size = (width*24+31)/32 * height * 4;
	
	image_data = new GLubyte[image_size];
	memset(image_data, 0, image_size);

	glGetIntegerv(GL_READ_BUFFER, (GLint*)&image_buffer);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, image_data);
	glReadBuffer(image_buffer);

	if (OutputDataToBMPFile(filename, (unsigned char*)image_data, width, height)){
		return true;
	}

	return false;
}

bool OutputDataToBMPFile(std::string filename, unsigned char* data, unsigned int width, unsigned int height)
{
	std::ofstream image_file;

	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER bitmap_info_header;
	image_file.open(filename, std::ios::out | std::ios::binary);
	if (!image_file.is_open())
	{
		std::cout << "fail to open" << filename << std::endl;
		return false;
	}

	file_header.bfType = BITMAP_ID;
	file_header.bfSize = (width * 24 + 31) / 32 * height * 4 + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	file_header.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	file_header.bfReserved1 = 0;
	file_header.bfReserved2 = 0;

	bitmap_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_header.biWidth = width;
	bitmap_info_header.biHeight = height;
	bitmap_info_header.biPlanes = 1;
	bitmap_info_header.biBitCount = 24;
	bitmap_info_header.biCompression = BI_RGB;
	bitmap_info_header.biSizeImage = (width * 24 + 31) / 32 * height * 4;
	bitmap_info_header.biXPelsPerMeter = 0;
	bitmap_info_header.biYPelsPerMeter = 0;
	bitmap_info_header.biClrUsed = 0;
	bitmap_info_header.biClrImportant = 0;

	image_file.write((char*)&file_header, sizeof(BITMAPFILEHEADER));
	image_file.write((char*)&bitmap_info_header, sizeof(BITMAPINFOHEADER));
	image_file.write((char*)data, bitmap_info_header.biSizeImage);
	image_file.close();
	return true;
}