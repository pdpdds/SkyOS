/*!
  \file	kernel/i386/vesa/vesa.c
  \brief	vesa vbe graphics mode info gathering and setting
*/

#include <ace.h>
#include <string.h>
#include <kernel/debug.h>
#include <kernel/io.h>
#include <kernel/pm/thread.h>
#include <kernel/i386/i386.h>
#include "vbe.h"

#define VESA_INPUT_GET_CONTROL_INFO		0x4F00
#define VESA_INPUT_GET_MODE_INFO		0x4F01
#define VESA_INPUT_SET_MODE				0x4F02

#define VESA_OUTPUT_SUCCESS				0x004F		
#define VESA_OUTPUT_FAILED				0x014F
#define VESA_OUTPUT_FN_NOT_SUPPORTED	0x024F
#define VESA_OUTPUT_FN_INVALID			0x024F

/*! Converts the given VESA farpointer (segment, offset pair) into linear physical address*/
#define VESA_FARPTR_TO_LINEAR(fp)		FP_TO_LINEAR( ((UINT32)fp)>>16, ((UINT32)fp)&0xFFFF )

#define VESA_MODE_BIT_CUSTOM_REFRESH	(1<<11)
#define VESA_MODE_BIT_USE_LFB			(1<<14)
#define VESA_MODE_BIT_NO_CLEAR			(1<<15)

/*! buffer location to store bios outputs*/
#define BUFFER_UNDER_1MB				(100*1024)

#define MAX_VESA_MODES					(50)

#define VESA_DISPLAY_INT_NO				(0x10)

static char * linear_frame_buffer_va=NULL;
static UINT16 vesa_mode_array[MAX_VESA_MODES];
static VESA_MODE_INFO current_vesa_mode_info;

/*! Returns VESA controller information
	\note - the caller has to free up the returned va(one page)
*/
static VESA_CONTROL_INFO_PTR GetVesaControllerInfo()
{
	UINT32 pa, va, fp;
	VESA_CONTROL_INFO_PTR control_info;
	UINT16 * mode_list = NULL;
	REGS_V86 v86_reg, * bios10_out;
	int i;
	
	/*! get vesa controller info*/
	fp = I386LinearToFp(BUFFER_UNDER_1MB);
	v86_reg.reg.eax = VESA_INPUT_GET_CONTROL_INFO;
	v86_reg.v86.es = FP_SEG(fp);
	v86_reg.reg.edi = FP_OFF(fp);
	bios10_out = CallBiosIsr(VESA_DISPLAY_INT_NO, &v86_reg);
	
	if( bios10_out==NULL || bios10_out->reg.eax != VESA_OUTPUT_SUCCESS )
	{
		KTRACE("vesa function not supported %d\n", bios10_out->reg.eax);
		return NULL;
	}
	
	control_info = (VESA_CONTROL_INFO_PTR)MapPhysicalMemory( GetCurrentVirtualMap(), BUFFER_UNDER_1MB, PAGE_SIZE, 0, PROT_READ | PROT_WRITE );
	
	pa = (UINT32) VESA_FARPTR_TO_LINEAR(control_info->video_mode_ptr);
	va = (UINT32) MapPhysicalMemory( GetCurrentVirtualMap(), pa, PAGE_SIZE, 0, PROT_READ | PROT_WRITE );
	mode_list = (UINT16 *)(((char *)va) + (pa - PAGE_ALIGN(pa)));
	/*copy the mode list to local array*/
	i=0;
	while(1)
	{
		vesa_mode_array[i] = mode_list[i];
		if ( mode_list[i] == 0xFFFF )
			break;
		i++;
	}
	/*free mode info - we have copied it to a local buffer*/
	FreeVirtualMemory( GetCurrentVirtualMap(), va, PAGE_SIZE, 0);
	
	return control_info;
}

/*! Get VESA Mode information by invoking BIOS 10h
	\note The caller is responsible for freeing up the returned va
*/
static VESA_MODE_INFO_PTR GetVesaModeInfo(int mode)
{
	UINT32 fp;
	REGS_V86 v86_reg, * bios10_out;
	
	fp = I386LinearToFp(BUFFER_UNDER_1MB);
	v86_reg.reg.eax = VESA_INPUT_GET_MODE_INFO;
	v86_reg.reg.ecx = mode;
	v86_reg.v86.es = FP_SEG(fp);
	v86_reg.reg.edi = FP_OFF(fp);
	bios10_out = CallBiosIsr(VESA_DISPLAY_INT_NO, &v86_reg);
	if( bios10_out==NULL || bios10_out->reg.eax != VESA_OUTPUT_SUCCESS )
	{
		KTRACE("vesa function not supported %d", bios10_out->reg.eax);
		return NULL;
	}
	
	return (VESA_MODE_INFO_PTR) MapPhysicalMemory( GetCurrentVirtualMap(), BUFFER_UNDER_1MB, PAGE_SIZE, 0, PROT_READ | PROT_WRITE );
}

/*! Set VESA mode by invoking BIOS 10h*/
static int SetVesaMode(int mode)
{
	REGS_V86 v86_reg, * bios10_out;
	
	KTRACE("Setting graphics mode %d\n", mode);
	v86_reg.reg.eax = VESA_INPUT_SET_MODE;
	v86_reg.reg.ebx = mode | VESA_MODE_BIT_USE_LFB;
	bios10_out = CallBiosIsr(VESA_DISPLAY_INT_NO, &v86_reg);
	if( bios10_out==NULL || bios10_out->reg.eax != VESA_OUTPUT_SUCCESS )
	{
		KTRACE("vesa mode setting failed");
		return 1;
	}
	return 0;
}

/*! Initializes graphics mode and set the grphics console */
int InitGraphicsConsole()
{
	VESA_CONTROL_INFO_PTR control_info;	
	int current_mode, selected_mode=-1;;
	
	/*get vesa control info*/
	control_info = GetVesaControllerInfo();
	if ( control_info == NULL )
		return 1;
	/*we are interested only in modes, so just free the allocated memory*/
	FreeVirtualMemory( GetCurrentVirtualMap(), (VADDR)control_info, PAGE_SIZE, 0);
	
	/*loop through the mode and find a suitable mode*/
	current_mode = 0;
	while( vesa_mode_array[current_mode]!=0xFFFF )
	{
		VESA_MODE_INFO_PTR mode_info;
		/*get mode info*/
		mode_info = GetVesaModeInfo( vesa_mode_array[current_mode] );
		if( mode_info == NULL )
			return 1;
		KTRACE( "X %d Y %d bpp %d %p bytes per scanline %d %d\n", mode_info->x_resolution, mode_info->y_resolution, mode_info->bits_per_pixel, mode_info->physical_base_ptr, mode_info->bytes_per_scanline, mode_info->linear_bytes_per_scan_line );
		if( mode_info->x_resolution >= 640 && mode_info->bits_per_pixel ==24 && mode_info->physical_base_ptr != 0 )
		{
			selected_mode = vesa_mode_array[current_mode];
			linear_frame_buffer_va = (char *)MapPhysicalMemory(GetCurrentVirtualMap(), mode_info->physical_base_ptr,  mode_info->y_resolution * mode_info->bytes_per_scanline, 0, PROT_READ|PROT_WRITE);
			memmove( &current_vesa_mode_info, mode_info, sizeof(VESA_MODE_INFO));
		}
		FreeVirtualMemory( GetCurrentVirtualMap(), (VADDR)mode_info, PAGE_SIZE, 0);
		current_mode++;
		if ( selected_mode != -1 )
			break;
	}
	
	/*set graphics mode*/
	if( SetVesaMode(selected_mode) )
		return 1;
	
	return 0;
}
