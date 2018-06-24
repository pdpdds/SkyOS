// TestEngine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h> 
#include <windows.h> 
#include "SkyMockInterface.h"
#include "I_HangulEngine.h"
#include "I_ImageInterface.h"

#include "SDL.h"
#include <iostream>

#pragma comment(lib, "SDL2.LIB")


FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;

typedef void (*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef I_ImageInterface* (*PGetImageInterface)();


void* GetModuleFunction(HINSTANCE handle, const char* func_name)
{
	return (void*)GetProcAddress(handle, func_name);
}

#undef main

int screen_w;
int screen_h;
SDL_Surface *screen;


int main()
{
	HINSTANCE dllHandle = NULL;

	//디버그엔진 모듈을 로드한다.
	dllHandle = LoadLibrary("libpng.dll");

	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)GetModuleFunction(dllHandle, "SetSkyMockInterface");
	PGetImageInterface PngGetInterface = (PGetImageInterface)GetModuleFunction(dllHandle, "GetImageInterface");

	//SetSkyMockInterface 함수를 사용해서 디버그엔진 모듈에 파일인터페이스와 입출력, 화면출력 인터페이스를 제공한다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
	I_ImageInterface* pImageInterface = PngGetInterface();

	SkyImageBuffer* pBuffer = pImageInterface->GetPixelDataFromFile("sample1.png");



	//윈도우와 렌더러를 생성
	SDL_Window *pWindow;
	SDL_Renderer *pRenderer;
#ifndef WIN32 
	if (SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN, &pWindow, &pRenderer) < 0)
#else
	if (SDL_CreateWindowAndRenderer(1024, 768, 0, &pWindow, &pRenderer) < 0)
#endif
	{
		std::cout << "SDL_CreateWindowAndRenderer Error: " << SDL_GetError() << std::endl;
		return 0;
	}
	SDL_GetWindowSize(pWindow, &screen_w, &screen_h);
	screen = SDL_CreateRGBSurface(0, screen_w, screen_h, 32,
		0,
		0,
		0,
		0);

	if (screen == 0)
	{
		std::cout << "SDL_CreateRGBSurface Error: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_Texture *pTexture = SDL_CreateTexture(pRenderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		screen_w, screen_h);

	if (pRenderer == 0)
	{
		std::cout << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	/*
	//비트맵 이미지를 로드
	SDL_Surface *pHellowBMP = SDL_LoadBMP("sample1.bmp");
	if (pHellowBMP == 0)
	{
		SDL_DestroyRenderer(pRenderer);
		SDL_DestroyWindow(pWindow);
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
		return 0;
	}
	//비트맵 이미지로부터 텍스처를 생성
	SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pHellowBMP);
	SDL_FreeSurface(pHellowBMP);*/


	if (pTexture == 0)
	{
		SDL_DestroyRenderer(pRenderer);
		SDL_DestroyWindow(pWindow);
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return 0;
	}

	
	unsigned char* pixels = (unsigned char*)screen->pixels;

	if (pBuffer->_bufferType == IAMGE_BUFFER_2D_ARRAY)
	{
		for (int y = 0; y < pBuffer->_height; y++)
		{
			for (int x = 0; x < pBuffer->_width; x++)
			{
				char* row = (char*)pBuffer->_pBuffer[y];

				char* pixel = &row[x * 4]; //SDL Surface => BGRA : PNG => RGBA
				pixels[4 * (y * screen->w + x) + 0] = pixel[2];
				pixels[4 * (y * screen->w + x) + 1] = pixel[1];
				pixels[4 * (y * screen->w + x) + 2] = pixel[0];
				pixels[4 * (y * screen->w + x) + 3] = pixel[3];
			}
		}
	}
	else
	{

	}

	pImageInterface->SavePixelData("sample3.png", nullptr, 0);
	
	

	bool running = true;
	//루프를 돌며 화면을 그린다.
	while (running)
	{
		//이벤트를 가져온다.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{	//키보드 이벤트가 발생했다면
			if (event.type == SDL_KEYDOWN)
			{        //ESC키를 눌렀다면 프로그램 종료
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
			} //QUIT 메시지가 들어왔다면 프로그램 종료
			else if (event.type == SDL_QUIT)
			{
				running = false;
			}
		}
		//렌더러를 클리어하고 Hello World 텍스처를 렌더러에 복사한다.
		SDL_RenderClear(pRenderer);
		SDL_UpdateTexture(pTexture, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(pRenderer, pTexture, NULL, NULL);
		//렌더러의 내용을 화면에 뿌린다.
		SDL_RenderPresent(pRenderer);
	}
	//텍스처, 렌더러, 윈도우 객체를 제거하고 SDL을 종료한다.
	SDL_DestroyTexture(pTexture);
	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}

