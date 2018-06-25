#include "jpgReader.h"
#include "SkyInterface.h"


extern "C" void printf(const char* str, ...);
jpgReader::jpgReader()
{
  bytes_per_pixel = 3;  // @TODO: This should be a parameter
  color_space = JCS_RGB;
}


laz_img jpgReader::readJPG(char * filename)
{

  unsigned char *raw_image = NULL;
  /* these are standard libjpeg structures for reading(decompression) */
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  /* libjpeg data structure for storing one row, that is, scanline of an image */
  JSAMPROW row_pointer[1];
	
  FILE *infile = fopen( filename, "rb" );
  unsigned long location = 0;
  int i = 0;
	
  if ( !infile )
    {
      printf("Error opening jpeg file %s\n!", filename );
      //return NULL;
    }
  /* here we set up the standard libjpeg error handler */
  cinfo.err = jpeg_std_error( &jerr );
  /* setup decompression process and source, then read JPEG header */
  jpeg_create_decompress( &cinfo );
  /* this makes the library read from infile */
  jpeg_stdio_src( &cinfo, infile );
  /* reading the image header which contains image information */
  jpeg_read_header( &cinfo, (boolean)true );
  /* Uncomment the following to output image information, if needed. */
  /*--
    printf( "JPEG File Information: \n" );
    printf( "Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height );
    printf( "Color components per pixel: %d.\n", cinfo.num_components );
    printf( "Color space: %d.\n", cinfo.jpeg_color_space );
    --*/
  /* Start decompression jpeg here */
  jpeg_start_decompress( &cinfo );

  /* allocate memory to hold the uncompressed image */
  raw_image = (unsigned char*)malloc( cinfo.output_width*cinfo.output_height*cinfo.num_components );
  /* now actually read the jpeg into the raw buffer */
  row_pointer[0] = (JSAMPROW)malloc( cinfo.output_width*cinfo.num_components );
  /* read one scan line at a time */
  while( cinfo.output_scanline < cinfo.image_height )
    {
      jpeg_read_scanlines( &cinfo, row_pointer, 1 );
      for( i=0; i<cinfo.image_width*cinfo.num_components;i++) 
	raw_image[location++] = row_pointer[0][i];
    }
  /* Create the laz_img object */
  laz_img img(raw_image, cinfo.output_width, cinfo.output_height);

  /* wrap up decompression, destroy objects, free pointers and close open files */
  jpeg_finish_decompress( &cinfo );
  jpeg_destroy_decompress( &cinfo );
  free( row_pointer[0] );
  fclose( infile );
  /* yup, we succeeded! */
  return img;
}


/**
 * write_jpeg_file Writes the raw image data stored in the raw_image buffer
 * to a jpeg image with default compression and smoothing options in the file
 * specified by *filename.
 *
 * \returns positive integer if successful, -1 otherwise
 * \param *filename char string specifying the file name to save to
 *
 */
int jpgReader::writeJPG(laz_img img, char *filename )
{

  unsigned char *raw_image = img.getData();
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
	
  /* this is a pointer to one row of image data */
  JSAMPROW row_pointer[1];
  FILE *outfile = fopen( filename, "wb" );
	
  if ( !outfile )
    {
      printf("Error opening output jpeg file %s\n!", filename );
      return -1;
    }
  cinfo.err = jpeg_std_error( &jerr );
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, outfile);

  /* Setting the parameters of the output file here */
  cinfo.image_width = img.getWidth();	
  cinfo.image_height = img.getHeight();
  cinfo.input_components = bytes_per_pixel;
  cinfo.in_color_space = color_space;
  /* default compression parameters, we shouldn't be worried about these */
  jpeg_set_defaults( &cinfo );
  /* Now do the compression .. */
  boolean write_all_tables = (boolean)true;
  jpeg_start_compress( &cinfo, write_all_tables);
  /* like reading a file, this time write one row at a time */
  while( cinfo.next_scanline < cinfo.image_height )
    {
      row_pointer[0] = (JSAMPROW)&raw_image[ cinfo.next_scanline * cinfo.image_width *  cinfo.input_components];
      jpeg_write_scanlines( &cinfo, row_pointer, 1 );
    }
  /* similar to read file, clean up after we're done compressing */
  jpeg_finish_compress( &cinfo );
  jpeg_destroy_compress( &cinfo );
  fclose( outfile );
  /* success code is 1! */
  return 1;
}
