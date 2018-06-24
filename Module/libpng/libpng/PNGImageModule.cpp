#include "PNGImageModule.h"
#include "SkyInterface.h"
#include "memory.h"

extern void printf(const char* str, ...);

PNGImageModule::PNGImageModule()
{
}


PNGImageModule::~PNGImageModule()
{
}

bool PNGImageModule::Initialize()
{
	return true;
}

SkyImageBuffer* PNGImageModule::GetPixelDataFromFile(char* szFileName)
{
	char header[8];
	FILE *fp = fopen(szFileName, "rb");
	if (!fp)
	{
		printf("[read_png_file] File %s could not be opened for reading", szFileName);
		return nullptr;
	}

	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_const_bytep)header, 0, 8))
	{
		printf("[read_png_file] File %s is not recognized as a PNG file", szFileName);
		fclose(fp);
		return nullptr;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		printf("[read_png_file] png_create_read_struct failed");
		fclose(fp);
		return nullptr;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		printf("[read_png_file] png_create_info_struct failed");
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		fclose(fp);
		return nullptr;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[read_png_file] Error during init_io");
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);		
		fclose(fp);
		return nullptr;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);


	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[read_png_file] Error during read_image");
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(fp);
		return nullptr;
	}

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for (y = 0; y < height; y++)
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);

	m_imageBuffer._pBuffer = (char**)row_pointers;
	m_imageBuffer._height = height;
	m_imageBuffer._width = width;
	m_imageBuffer._colorType = color_type;
	m_imageBuffer._bpp = bit_depth;
	m_imageBuffer._bufferType = IAMGE_BUFFER_2D_ARRAY;

	//ProcessFile();
	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	fclose(fp);

	return &m_imageBuffer;
}

SkyImageBuffer* PNGImageModule::GetPixelDataFromBuffer(char* pBuffer, int size)
{
	//not implemented
	return nullptr;
}

bool PNGImageModule::SavePixelData(char* szFileName, char* pBuffer, int size)
{
	/* create file */
	FILE *fp = fopen(szFileName, "wb");
	if (!fp)
	{
		printf("[write_png_file] File %s could not be opened for writing", szFileName);
		return false;
	}

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		printf("[write_png_file] png_create_write_struct failed");
		fclose(fp);
		return false;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		printf("[write_png_file] png_create_info_struct failed");
		fclose(fp);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[write_png_file] Error during init_io");
		fclose(fp);
		return false;
	}

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[write_png_file] Error during writing header");
		fclose(fp);
		return false;
	}

	png_set_IHDR(png_ptr, info_ptr, width, height,
		bit_depth, color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[write_png_file] Error during writing bytes");
		fclose(fp);
		return false;
	}

	png_write_image(png_ptr, row_pointers);


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		printf("[write_png_file] Error during end of write");
		fclose(fp);
		return false;
	}

	png_write_end(png_ptr, NULL);

	/* cleanup heap allocation */
	for (y = 0; y<height; y++)
		free(row_pointers[y]);
	free(row_pointers);

	fclose(fp);

	return false;
}

void PNGImageModule::ProcessFile(void)
{
	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
	{
		printf("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
			"(lacks the alpha channel)");

		return;
	}

	if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
	{
		printf("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
			PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));
		return;
	}

	for (y = 0; y<height; y++) {
		png_byte* row = row_pointers[y];
		for (x = 0; x<width; x++) {
			png_byte* ptr = &(row[x * 4]);
			//printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
				//x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

			/* set red value to 0 and green value to the blue one */
			ptr[0] = 0;
			ptr[1] = ptr[2];
		}
	}
}