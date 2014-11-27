#include "screenshot.h"
#include <fstream>
#include "GL/freeglut.h"
#include <iostream>
#include <string>

#define BITMAP_ID 0x4D42;

bool OutputDataToBMPFile(std::string filename, unsigned char* data, unsigned int width, unsigned int height);

bool ScreenShot(std::string filename){
	GLenum image_buffer;
	GLbyte* image_data = 0;
	unsigned long image_size;
	GLint view[4];

	glGetIntegerv(GL_VIEWPORT, view);
	image_size = view[2] * view[3] * 3;

	image_data = (GLbyte*)new unsigned char[image_size];

	if (!image_data){
		return false;
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	glGetIntegerv(GL_READ_BUFFER, (GLint*)&image_buffer);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, view[2], view[3], GL_BGR_EXT, GL_UNSIGNED_BYTE, image_data);
	glReadBuffer(image_buffer);

	if (OutputDataToBMPFile(filename, (unsigned char*)image_data, view[2], view[3])){
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
	file_header.bfSize = width * height * 3 + sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	file_header.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	file_header.bfReserved1 = 0;
	file_header.bfReserved2 = 0;

	bitmap_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info_header.biWidth = width;
	bitmap_info_header.biHeight = height;
	bitmap_info_header.biPlanes = 1;
	bitmap_info_header.biBitCount = 24;
	bitmap_info_header.biCompression = BI_RGB;
	bitmap_info_header.biSizeImage = width * height * 3;
	bitmap_info_header.biXPelsPerMeter = 0;
	bitmap_info_header.biYPelsPerMeter = 0;
	bitmap_info_header.biClrUsed = 0;
	bitmap_info_header.biClrImportant = 0;

	image_file.write((char*)&file_header, sizeof(BITMAPFILEHEADER));
	image_file.write((char*)&bitmap_info_header, sizeof(BITMAPINFOHEADER));

	image_file.seekp(file_header.bfOffBits, std::ios::beg);
	image_file.write((char*)data, bitmap_info_header.biSizeImage);

	image_file.close();
	return true;
}