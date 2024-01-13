#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_SECRET_BUF SIZE 1
#define MAX_IMAGE_BUF SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
/*
 * Structure to store information required for
 * decoding secret file from source image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
    /*Stego file info*/
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /*Output file info*/
    char *output_fname;
    FILE *out;
    char output_extn[MAX_FILE_SUFFIX];
    long output_extn_size;
    long output_file_size;

} DecodeInfo;
/* Decoding function prototypr*/

/* Read and validate Decode args from argv */
Status read_and_Validate_decode_args(char *argv[],DecodeInfo *decInfo);

/*Perform the Decoding */
Status do_decoding(DecodeInfo *decInfo);

/*Get File pointers for i/p and o/p files */
Status open_file(DecodeInfo *decInfo);

/* Decode magic string size */
Status decode_size(int size,DecodeInfo *decInfo);

/* Decode magic string */

Status decode_magic_string(const char *magic_string);

/* Decode Secret file extenstion */

Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode Secret file ectenstion size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode Secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode Secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function,which does the real Decoding */
Status decode_data_to_image(char *data,int size,FILE *fptr_stego_image);

/* Decode a byte into lsb of image data array */
char decode_byte_to_lsb(char *image_buffer);

#endif



