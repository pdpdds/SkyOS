#include "SkyTest.h"
#include "stdint.h"
#include "stdio.h"
#include "windef.h"
#include "exception.h"
#include "string.h"
#include "Vector.h"
#include "Stack.h"
#include "jsmn.h"
#include "map.hpp"
#include "list1.h"
#include "TestInteger.h"
#include "SkyConsole.h"
#include "SkyQueue.h"
#include "stl\stl_string.h"
#include "stl\deque.h"

#define kprintf SkyConsole::Print

void TestV8086();

static const char *JSON_STRING =
"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void TestMap();
void TestList();
void TestCPP14();
void TestJson();
void TestString();
void TestVector();


void SkyTest()
{
	/*
	Stack<int> s;
	s.push(10);
	s.push(20);
	s.push(30);

	while (s.size() > 0)
	{
	int data = s.pop();
	SkyConsole::Print("%d\n", data);

	}*/


	//TestMap();
	TestList();
	TestCPP14();

	

	//TestV8086();

}

void TestString()
{
	std::string str("abc");
	SkyConsole::Print("%s\n", str.c_str());

	std::string str2 = str;
	str2 = "cdf";
	SkyConsole::Print("%s\n", str2.c_str());
}

void TestDeque()
{
	std::deque<int> a;
	
	for (int i = 1; i < 6; i++)
		a.push_front(i);

	for (int i = 0; i < 5; i++)
		SkyConsole::Print("%d\n", a[i]);		
}

void TestVector()
{
	Vector<int> vec;
	vec.push_back(5);
	vec.push_back(1);
	vec.push_back(3);
	vec.push_back(4);

	while (vec.size() > 0)
	{
		int data = vec.back();
		SkyConsole::Print("%d\n", data);
		vec.pop_back();
	}

	for (;;);
}


void TestMap()
{
	std::map<int, int> m10;
	std::map<int, int>::iterator it1;
	std::map<int, int>::iterator it2;

	m10[1] = 2;
	m10[2] = 4;
	m10[3] = 6;
	m10[4] = 8;
	m10[5] = 10;
	m10[6] = 12;
	m10[7] = 14;
	m10[8] = 16;
	m10[8] = 18;
	int i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		kprintf("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}

	//cout << "ERASE BY KEY" << endl;
	m10.erase(3);
	i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		kprintf("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}

	for (;;);
}

void TestQueue()
{
	SkyQueue queue;
	QueueNode* pNode = new QueueNode();
	pNode->_data = (void*)5;
	queue.Enqueue(pNode);

	QueueNode* pNode2 = new QueueNode();
	pNode2->_data = (void*)10;
	queue.Enqueue(pNode2);

	QueueNode* result = queue.Dequeue();
	delete result;
	result = queue.Dequeue();
	delete result;
}

void TestJson()
{
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		kprintf("Failed to parse JSON: %d\n", r);
		return;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		kprintf("Object expected\n");
		return;
	}

	char buf[256];
	memset(buf, 0, 256);


	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "user") == 0) {
			/* We may use strndup() to fetch string value */
			memcpy(buf, JSON_STRING + t[i + 1].start, t[i + 1].end - t[i + 1].start);
			kprintf("- User: %s\n", buf);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {

			kprintf("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {

			kprintf("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) {
			int j;
			kprintf("- Groups:\n");
			if (t[i + 1].type != JSMN_ARRAY) {
				continue;
			}
			for (j = 0; j < t[i + 1].size; j++) {
				jsmntok_t *g = &t[i + j + 2];
				kprintf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i + 1].size + 1;
		}
		else {
			kprintf("Unexpected key: %.*s\n", t[i].end - t[i].start,
				JSON_STRING + t[i].start);
		}
	}

	for (;;);
}

#include "./v8086/rme.h"
#include "vesa.h"
#include "Math.h"

#define PROMPT_FOR_MODE 1

/* Friggin' frick, this should be a config option
* because it's 4096 on some instances of Qemu,
* ie the one on my laptop, but it's 2048 on
* the EWS machines. */
#define BOCHS_BUFFER_SIZE 2048
#define PREFERRED_VY 4096
#define PREFERRED_B 32

uint16_t bochs_resolution_x = 0;
uint16_t bochs_resolution_y = 0;
uint16_t bochs_resolution_b = 0;


/* vm86 Helpers */
typedef uint32_t  FARPTR;
typedef uintptr_t addr_t;
#define MK_FP(seg, off)        ((FARPTR) (((uint32_t) (seg) << 16) | (uint16_t) (off)))
#define FP_SEG(fp)             (((FARPTR) fp) >> 16)
#define FP_OFF(fp)             (((FARPTR) fp) & 0xffff)
#define FP_TO_LINEAR(seg, off) ((void*) ((((uint16_t) (seg)) << 4) + ((uint16_t) (off))))
#define LINEAR_TO_FP(ptr)      (MK_FP(((addr_t) (ptr) - ((addr_t) (ptr) & 0xf)) / 16, ((addr_t)(ptr) & 0xf)))

uint8_t * bochs_vid_memory = (uint8_t *)0xE0000000;

static void finalize_graphics(uint16_t x, uint16_t y, uint16_t b) {
	bochs_resolution_x = x;
	bochs_resolution_y = y;
	bochs_resolution_b = b;
}

void graphics_install_vesa(uint16_t resX, uint16_t resY)
{
	/* VESA Structs */
	struct VesaControllerInfo *info = (VesaControllerInfo*)0x10000;
	struct VesaModeInfo *modeinfo = (VesaModeInfo*)0x9000;

	/* 8086 Emulator Status */
	tRME_State *emu;
	void * lowCache;
	lowCache = (void *)new BYTE[RME_BLOCK_SIZE];
	memcpy(lowCache, NULL, RME_BLOCK_SIZE);
	emu = RME_CreateState();
	emu->Memory[0] = (uint8_t*)lowCache;
	for (int i = RME_BLOCK_SIZE; i < 0x100000; i += RME_BLOCK_SIZE) {
		emu->Memory[i / RME_BLOCK_SIZE] = (uint8_t*)i;
	}
	int ret, mode;

	/* Find modes */
	uint16_t * modes;
	memcpy(info->Signature, "VBE2", 4);
	emu->AX.W = 0x4F00;
	emu->ES = 0x1000;
	emu->DI.W = 0;
	ret = RME_CallInt(emu, 0x10);
	if (info->Version < 0x200 || info->Version > 0x300)
	{
		SkyConsole::Print("\033[JYou have attempted to use the VESA/VBE2 driver\nwith a card that does not support VBE2.\n");
		SkyConsole::Print("\nSystem responded to VBE request with version: 0x%x\n", info->Version);

		HaltSystem("");
	}
	modes = (uint16_t*)FP_TO_LINEAR(info->Videomodes.Segment, info->Videomodes.Offset);

	uint16_t best_x = 0;
	uint16_t best_y = 0;
	uint16_t best_b = 0;
	uint16_t best_mode = 0;

	for (int i = 1; modes[i] != 0xFFFF; ++i) {
		emu->AX.W = 0x4F01;
		emu->CX.W = modes[i];
		emu->ES = 0x0900;
		emu->DI.W = 0x0000;
		RME_CallInt(emu, 0x10);
#if PROMPT_FOR_MODE
		SkyConsole::Print("%d = %dx%d:%d %d\n", i, modeinfo->Xres, modeinfo->Yres, modeinfo->bpp, modeinfo->physbase);
	}

	SkyConsole::Print("Please select a mode: ");
	KeyBoard::KEYCODE key = SkyConsole::GetChar();
	char selected = KeyBoard::ConvertKeyToAscii(key);
	char buf[10];
	buf[0] = selected;
	buf[1] = '\n';

	mode = atoi(buf);
#else
		if ((abs(modeinfo->Xres - resX) < abs(best_x - resX)) && (abs(modeinfo->Yres - resY) < abs(best_y - resY))) {
			best_mode = i;
			best_x = modeinfo->Xres;
			best_y = modeinfo->Yres;
			best_b = modeinfo->bpp;
		}
}
	for (int i = 1; modes[i] != 0xFFFF; ++i) {
		emu->AX.W = 0x4F01;
		emu->CX.W = modes[i];
		emu->ES = 0x0900;
		emu->DI.W = 0x0000;
		RME_CallInt(emu, 0x10);
		if (modeinfo->Xres == best_x && modeinfo->Yres == best_y) {
			if (modeinfo->bpp > best_b) {
				best_mode = i;
				best_b = modeinfo->bpp;
			}
		}
	}

	if (best_b < 24) {
		SkyConsole::Print("!!! Rendering at this bit depth (%d) is not currently supported.\n", best_b);
		HaltSystem("");
	}

	mode = best_mode;

#endif

	emu->AX.W = 0x4F01;
	if (mode < 100) {
		emu->CX.W = modes[mode];
	}
	else {
		emu->CX.W = mode;
	}
	emu->ES = 0x0900;
	emu->DI.W = 0x0000;
	RME_CallInt(emu, 0x10);

	HaltSystem("sadsd");

	emu->AX.W = 0x4F02;
	emu->BX.W = modes[mode];
	RME_CallInt(emu, 0x10);

	uint16_t actual_x = modeinfo->Xres;
	uint16_t actual_y = modeinfo->Yres;
	uint16_t actual_b = modeinfo->bpp;

	bochs_vid_memory = (uint8_t *)modeinfo->physbase;

	/*uint32_t* lfb = (uint32_t*)bochs_vid_memory;
	for (uint32_t c = 0; c<actual_x*actual_y; c++)
	lfb[c] = 0x90;

	SkyConsole::GetChar();*/

	/*int* lfb = (int*)bochs_vid_memory;
	for (int j = 0; j < actual_x; j++)
	for (int k = 0; k < actual_y; k++)
	lfb[k + (j) * actual_x] = 255;*/

	/*for (;;);

	if(bochs_vid_memory == 0)
	HaltSystem("Sdfdsdfd");*/

	/*if (!bochs_vid_memory)
	{
	uint32_t * herp = (uint32_t *)0xA0000;
	herp[0] = 0xA5ADFACE;

	// Enable the higher memory
	for (uintptr_t i = 0xE0000000; i <= 0xE0FF0000; i += 0x1000)
	{

	dma_frame(get_page(i, 1, kernel_directory), 0, 1, i);
	}
	for (uintptr_t i = 0xF0000000; i <= 0xF0FF0000; i += 0x1000) {
	dma_frame(get_page(i, 1, kernel_directory), 0, 1, i);
	}


	// Go find it
	for (uintptr_t x = 0xE0000000; x < 0xE0FF0000; x += 0x1000) {
	if (((uintptr_t *)x)[0] == 0xA5ADFACE) {
	bochs_vid_memory = (uint8_t *)x;
	goto mem_found;
	}
	}
	for (uintptr_t x = 0xF0000000; x < 0xF0FF0000; x += 0x1000) {
	if (((uintptr_t *)x)[0] == 0xA5ADFACE) {
	bochs_vid_memory = (uint8_t *)x;
	goto mem_found;
	}
	}
	}*/
mem_found:

	/*
	* Finalize the graphics setup with the actual selected resolution.
	*/
	finalize_graphics(actual_x, actual_y, actual_b);

	//InitGraphics(modeinfo);
}


void rect32A(int x, int y, int w, int h, int col) {
	int* lfb = (int*)bochs_vid_memory;
	for (int k = 0; k < h; k++)
		for (int j = 0; j < w; j++)
			lfb[(j + x) + (k + y) * bochs_resolution_x] = col;
}

void TestCPP14()
{
	auto func = [x = 5]() { return x; };
	auto n1 = func();
	kprintf("Lamda n1:%d\n", n1);

	constexpr TestInteger size(10);
	int x[size];
	x[3] = 11;
	kprintf("constexor x[3]:%d\n", x[3]);
}

void TestList()
{

	std::List<int> fstList = std::List<int>();
	std::List<int> scndList = std::List<int>();
	int counter = 0;

	for (int i = 0; i <= 10; ++i) {
		fstList.push_back(i);
	}
	std::List<int>::Iterator iter = fstList.begin();
	while (iter != fstList.end())
	{
		kprintf("item %d  done\n", *iter);
		iter++;
	}

	kprintf("done!!\n");

	/*while (1) {
	for (int i = 0; i <= 10; ++i) {
	fstList.push_back(i);
	}

	for (int i = 6; i <= 15; ++i) {
	scndList.push_front(i);
	}

	std::List<int>::Iterator iter = fstList.begin();

	fstList.splice(++iter, scndList);
	scndList.concat(fstList);
	fstList.concat(scndList);

	fstList.pop_back();
	fstList.clear();
	//std::cout << "Cycle # " << counter << " done" << std::endl;
	printf("Cycle %d  done\n", counter);
	++counter;
	}*/


}

void TestV8086()
{
	graphics_install_vesa(1024, 768);



	//HaltSystem("ff");

	int col = 0;
	bool dir = true;
	SkyConsole::Print("RectGenerate\n");
	while (1) {
		rect32A(200, 380, 100, 100, 0x80);
		if (dir) {
			if (col++ == 0xfe)
				dir = false;
		}
		else
			if (col-- == 1)
				dir = true;
	}
}