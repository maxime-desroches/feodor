#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_row_callback(png_structp png_ptr, png_uint_32 row, int pass);

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
  
  //char buffer[16];
  char byte = 0;
  //memset(buffer,0,16);
  int index = 0;
  int pos = 1;

  for(int i = 0; *(row_pointers + i); ++i) {
    unsigned char* row = *(row_pointers + i);
    for(int j = 0; *(row + j); j += 3) {
      for(int k = 0; k < 3; ++k) {
        if (pos == 9) {
          //printf(" : %c\n",buffer[index]);
          printf(" : %c\n",byte);
          if (byte == '$') {
            goto EXIT_DECODING;
          }
          pos = 1;
          byte = 0;
          //index++;
        }
        int bit = *(row + j + k) & 0x01;
        printf("%i",bit);
        //buffer[index] = buffer[index] | (bit << ((sizeof(char)*8) - pos));
        byte = byte | (bit << ((sizeof(char)*8) - pos++));
      }
    }
  }

EXIT_DECODING:

  //printf("%s\n",buffer);
  //puts("");

  fclose(fp);

  
  return 0;
}

void read_row_callback(png_structp png_ptr, png_uint_32 row, int pass) {
  //printf("row: %i\n",row);
}
