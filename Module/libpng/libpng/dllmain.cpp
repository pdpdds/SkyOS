// dllmain.cpp: DLL 응용 프로그램의 진입점을 정의합니다.

#include "windef.h"
#include "SkyMockInterface.h"
#include "SkyInterface.h"
#include "zlib.h"
#include "string.h"
#include "stl/IOStream.h"
#include "sprintf.h"

extern FILE* g_skyStdErr;
extern FILE* g_skyStdOut;

void printf(const char* str, ...)
{
	if (!str)
		return;

	va_list		args;
	va_start(args, str);
	size_t i;
	for (i = 0; i < strlen(str); i++) {

		switch (str[i]) {

		case '%':

			switch (str[i + 1]) {

				/*** characters ***/
			case 'c': {
				char c = va_arg(args, char);
				g_mockInterface.g_printInterface.sky_printf("%c", c);
				//SkyConsole::WriteChar(c);
				i++;		// go to next character
				break;
			}

					  /*** address of ***/
			case 's': {
				int c = (int&)va_arg(args, char);
				char str[256];
				strcpy(str, (const char*)c);
				g_mockInterface.g_printInterface.sky_printf("%s",str);
				//SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

					  /*** integers ***/
			case 'd':
			case 'i': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 10, str);
				g_mockInterface.g_printInterface.sky_printf("%s", str);
				//SkyConsole::Write(str);
				i++;		// go to next character
				break;
			}

					  /*** display in hex ***/
					  /*int*/
			case 'X': {
				int c = va_arg(args, int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				//SkyConsole::Write(str);
				g_mockInterface.g_printInterface.sky_printf("%s", str);
				i++;		// go to next character
				break;
			}
					  /*unsigned int*/
			case 'x': {
				unsigned int c = va_arg(args, unsigned int);
				char str[32] = { 0 };
				itoa_s(c, 16, str);
				//SkyConsole::Write(str);
				g_mockInterface.g_printInterface.sky_printf("%s", str);
				i++;		// go to next character
				break;
			}

			default:
				va_end(args);
				return;
			}

			break;

		default:
//			SkyConsole::WriteChar(str[i]);
			g_mockInterface.g_printInterface.sky_printf("%c", str[i]);
			break;
		}

	}

	va_end(args);
	return;
}
#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0
#define DLL_THREAD_ATTACH 2
#define DLL_THREAD_DETACH 3

extern void _cdecl InitializeConstructors();
extern void _cdecl Exit();

bool __stdcall DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	//gzFile file;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		
		

	}
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


extern "C" bool WINAPI _DllMainCRTStartup(
	HANDLE  hDllHandle,
	DWORD   dwReason,
	LPVOID  lpreserved
)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// set up our minimal cheezy atexit table
		//_atexit_init();

		// Call C++ constructors
		//_initterm(__xc_a, __xc_z);

		InitializeConstructors();
	}

	bool retcode = DllMain(hDllHandle, dwReason, lpreserved);

	if (dwReason == DLL_PROCESS_DETACH)
	{
		Exit();
	}

	return retcode;
}


void TestZLib()
{
	const  int BUF = 1024;
	const  int DBUF = BUF * 2 + 13;

	Bytef raw_data[] = "안녕하세요.";
	Bytef deflate_data[DBUF];

	uLong raw_size = strlen((const  char*)raw_data);
	uLong deflate_size = DBUF;

	//compress 사용하기
	{
		compress(deflate_data, &deflate_size, raw_data, raw_size);
		std::cout << "Raw Data Size:" << raw_size << std::endl;
		std::cout << "Deflate Data Size:" << deflate_size << std::endl;
	}

	Bytef inflate_data[BUF];
	uLong inflate_size = BUF;
	//uncompress 사용하기
	{
		uncompress(inflate_data, &inflate_size, deflate_data, deflate_size);
		std::cout << "Deflate Data Size:" << deflate_size << std::endl;
		std::cout << "Inflate Size:" << inflate_size << std::endl;
		inflate_data[inflate_size] = NULL;
		std::cout << "원본  데이터:" << (const  char*)inflate_data << std::endl;
	}
}





#define PNG_DEBUG 3
#include <png.h>

void abort_(const char * s, ...)
{
	/*va_list args;
	va_start(args, s);
	vfprintf(g_skyStdOut, s, args);
	fprintf(g_skyStdErr, "\n");
	va_end(args);
	abort();*/
}

int x, y;

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;

void read_png_file(char* file_name)
{
	char header[8];    // 8 is the maximum size that can be checked

					   /* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp)
		abort_("[read_png_file] File %s could not be opened for reading", file_name);
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_const_bytep)header, 0, 8))
		abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		abort_("[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during init_io");

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
		abort_("[read_png_file] Error during read_image");

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for (y = 0; y<height; y++)
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);

	fclose(fp);
}


void write_png_file(char* file_name)
{
	/* create file */
	FILE *fp = fopen(file_name, "wb");
	if (!fp)
		abort_("[write_png_file] File %s could not be opened for writing", file_name);


	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		abort_("[write_png_file] png_create_write_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during init_io");

	png_init_io(png_ptr, fp);


	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, info_ptr, width, height,
		bit_depth, color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during writing bytes");

	png_write_image(png_ptr, row_pointers);


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[write_png_file] Error during end of write");

	png_write_end(png_ptr, NULL);

	/* cleanup heap allocation */
	for (y = 0; y<height; y++)
		free(row_pointers[y]);
	free(row_pointers);

	fclose(fp);
}


void process_file(void)
{
	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
		abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
			"(lacks the alpha channel)");

	if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
		abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
			PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

	for (y = 0; y<height; y++) {
		png_byte* row = row_pointers[y];
		for (x = 0; x<width; x++) {
			png_byte* ptr = &(row[x * 4]);
			printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
				x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

			/* set red value to 0 and green value to the blue one */
			ptr[0] = 0;
			ptr[1] = ptr[2];
		}
	}
}

void TestPNGLib()
{
	read_png_file("sample1.png");
	process_file();
	write_png_file("sample2.png");
}

extern "C" __declspec(dllexport) void GetHangulEngine()
{
	TestZLib();
	TestPNGLib();

}