/*!
  \file	kernel/i386/vesa/vbe.h
  \brief	VBE data structures
*/

#ifndef _VBE_H
#define _VBE_H

/*! Capabilities of display controller*/
struct vesa_control_info
{
	unsigned char	signature[4];			/*! either VESA or VBE */
	unsigned short	version;				/*! vesa version level*/
	unsigned long	oem_string_ptr;			/*! VBE far pointer to OEM string*/
	unsigned char	capabilities[4];		/*! capabilitis of the display controller*/
	unsigned long	video_mode_ptr;			/*! VBE far pointer to video mode list*/
	unsigned short	total_memory;			/*! Number of 64kb blocks*/
	unsigned short	oem_software_revision;	/*! VBE implementation software revision*/
	unsigned long	oem_vendor_name_ptr;	/*! VBE far pointer to OEM vendor name*/
	unsigned long	oem_product_name_ptr;	/*! VBE far pointer to OEM product name*/
	unsigned long	oem_product_rev_ptr;	/*! VBE far pointer to OEM product revision*/
	unsigned char	reserved[222];
	unsigned char	oem_data[256];
}__attribute__ ((packed));

struct vesa_mode_info
{
	unsigned short	mode_attributes;		
	unsigned char	wina_attributes;
	unsigned char	winb_attributes;
	unsigned short	win_granularity;
	unsigned short	win_size;
	unsigned short	wina_segment;
	unsigned short	winb_segment;
	unsigned long	win_func_ptr;
	unsigned short	bytes_per_scanline;				/*! Full bytes per logical scan line*/
	unsigned short	x_resolution;					/*! horizontal resolution*/
	unsigned short	y_resolution;					/*! vertical resolution*/
	unsigned char	x_char_size;					/*! character cell width in pixels*/
	unsigned char	y_char_size;					/*! character cell height in pixels*/
	unsigned char	number_of_planes;				
	unsigned char	bits_per_pixel;
	unsigned char	number_of_banks;
	unsigned char	memory_model;					/*! memory mode type - 00-text mode 04-packed pixel 06-direct color etc*/
	unsigned char	bank_size;
	unsigned char	number_of_image_pages;
	unsigned char	reserved_page;
	unsigned char	red_mask_size;
	unsigned char	red_mask_pos;
	unsigned char	green_mask_size;
	unsigned char	green_mask_pos;
	unsigned char	blue_mask_size;
	unsigned char	blue_mask_pos;
	unsigned char	reserved_mask_size;
	unsigned char	reserved_mask_pos;
	unsigned char	direct_color_mode_info;
	unsigned long	physical_base_ptr;
	unsigned long	linear_bytes_per_scan_line;
	unsigned char	bank_number_of_images;
	unsigned char	linear_number_of_images;
	unsigned char	linear_red_mask_size;
	unsigned char	linear_red_mask_pos;
	unsigned char	linear_green_mask_size;
	unsigned char	linear_green_mask_pos;
	unsigned char	linear_blue_mask_size;
	unsigned char	linear_blue_mask_pos;
	unsigned char	linear_reserved_mask_size;
	unsigned char	linear_reserved_mask_pos;
	unsigned char	max_pixel_clock;
	unsigned char	reserved[189];
}__attribute__ ((packed));

typedef struct vesa_control_info VESA_CONTROL_INFO, * VESA_CONTROL_INFO_PTR;
typedef struct vesa_mode_info VESA_MODE_INFO, * VESA_MODE_INFO_PTR;

#endif
