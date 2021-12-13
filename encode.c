#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_row_callback(png_structp png_ptr, png_uint_32 row, int pass);
void write_row_callback(png_structp png_ptr, png_uint_32 row, int pass);

int main(int argc, char **argv) {
  FILE *fp = fopen(*(argv + 1), "rb");
  if(!fp) {
    puts("ERROR CAN'T READ THIS FILE");
    return 420;
  }

  void *signature = malloc(8);
  memset(signature,0,8);
  fread(signature,1,8,fp);
  if(png_sig_cmp(signature,0,8)) {
    puts("ERROR THIS IS NOT A PNG FILE");
    return 420;
  }
  free(signature);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if(!png_ptr) {
    puts("ERROR WITH png_create_read_struct");
    return 420;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    puts("ERROR WITH png_create_info_struct");
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    return 420;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    puts("ERROR WITH png_create_info_struct");
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    return 420;
  }

  png_init_io(png_ptr,fp);
  png_set_sig_bytes(png_ptr,8);

  png_set_read_status_fn(png_ptr,read_row_callback);

  png_set_alpha_mode(png_ptr, PNG_ALPHA_PNG, PNG_DEFAULT_sRGB);

  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);

  char *msg = malloc(sizeof(char) * 1024);
  strncat(msg,*(argv + 2), 1024);
  strncat(msg,"$", 1);
  int index = 1;
  char c = *msg;

  for(int i = 0; *(row_pointers + i); ++i) {
    unsigned char* row = *(row_pointers + i);
    for(int j = 0; *(row + j); j += 3) {
      for(int k = 0; k < 3; ++k) {
        if (index == 9) {
          if(!*(++msg)) {
            goto EXIT_ENCODING;
          }
          index = 1;
          c = *msg;
        }
        int bit = (c >> ((sizeof(char)*8) - index++)) & (0x01);
        *(row + j + k) =  (*(row + j + k) & (0xFE)) | bit;
      }
    }
  }

EXIT_ENCODING:

  fclose(fp);

  
  // ################
  // WRITTING NOW !!!!
  // ################
  

  fp = fopen("encoded.png", "wb");
  if(!fp) {
    puts("ERROR WITH FILE POINTER");
    return 420;
  }

  png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL, NULL, NULL);
  if(!write_ptr) {
    puts("ERROR CREATING WRITE STRUCTURE");
    return 420;
  }

  png_infop write_info_ptr = png_create_info_struct(write_ptr);
  if(!write_info_ptr) {
    puts("ERROR CREATING INFO STRCTURE");
    return 420;
  }

  png_init_io(write_ptr, fp);

  png_set_write_status_fn(write_ptr,write_row_callback);

  png_set_rows(write_ptr,info_ptr, row_pointers);
  png_write_png(write_ptr,info_ptr,PNG_TRANSFORM_IDENTITY, NULL);

  return 0;
}

void read_row_callback(png_structp png_ptr, png_uint_32 row, int pass) {
  //printf("row: %i\n",row);
}

void write_row_callback(png_structp png_ptr, png_uint_32 row, int pass) {
  //printf("row: %i\n",row);
}
