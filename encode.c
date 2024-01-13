#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "encode.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{

    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);                                                       

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);                                                              
	
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return (width * height * BYTESPERPIXEL);
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");                                             //source image file
    printf("Info: Opened %s \n",encInfo->src_image_fname);   

    if (encInfo->fptr_src_image == NULL)                                                                        //do error handling
    {
    	perror("ifopen");
    	fprintf(stderr, "INFO: ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");                                                     //secret file
    printf("Info: Opened %s \n",encInfo->secret_fname);
    if (encInfo->fptr_secret == NULL)                                                                             //do error handling
    {
    	perror("fopen");
    	fprintf(stderr, "INFO: ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");                                            //stego image file
    printf("Info: Opened %s \n",encInfo->stego_image_fname);          
    if (encInfo->fptr_stego_image == NULL)                                                                        //do error handling
    {
    	perror("fopen");
    	fprintf(stderr, "INFO: ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    return e_success;                                                                                             //No failure return e_success
}


Status  read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
    char arr[2];
    FILE *fp;
    fp=fopen(argv[2],"r");                                                                                         //source file
    if(fp==NULL)
    {
       printf("INFO: Error File Not exist\n");  
       return e_failure;
    }
    fread(&arr,1,2,fp);                                                                                            //read 2 byte of data
    if(!(strcmp(arr,"BM")))                                                                                        //check bmp file or not
    {
        encInfo->src_image_fname=malloc(50);
        encInfo->src_image_fname=argv[2];
    }
    
    if(strchr(argv[3],'.')==NULL)                                                                                   //check secret file
    {
        printf("INFO: Error: Pass the Valid Secret File\n");
        return e_failure;
    }
    else
    {
        encInfo->secret_fname=malloc(32);
        encInfo->secret_fname=argv[3];                                                                              //copy secret file name
        char *e=strchr(argv[3],'.');
        strcpy(encInfo->extn_secret_file,e);                                                                        //copy the secret file extension
    }
     if(argv[4]!=NULL)                                                                                             //check output file
     {
       encInfo->stego_image_fname=malloc(32);
       if(strchr(argv[4],'.')==NULL)
	   {
		   printf("INFO: Please Pass Valid Output File\n");
	   
          return e_failure;
	   }
       else if(!(strcmp(strchr(argv[4],'.'),".bmp")))                                                               //check output file is .bmo or not
            encInfo->stego_image_fname=argv[4];
        else 
        {
               printf("INFO: Error: Pass the Valid Output File Name\n");
               return e_failure;
        }
     }
     else
     {
         printf("INFO: Output File not mentioned. Creating steged_img.bmp as default\n");
         encInfo->stego_image_fname="steged_img.bmp";                                                                //set default output file name
     }
     return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)                                                                               //check all files are opened
    {
        printf("INFO: Done\n##========= Encoding Procedure Started ==========##\n");
    }
    else
    {
        printf("INFO: Opening Files Failed\n");
        return e_failure;
    }
    if(check_capacity(encInfo)==e_success)                                                                           //check the capacity of the image to encode
    {
        printf("INFO: Done. Found OK\n");
    }
    else
    {
        printf("INFO: Found Not\n");
        return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)                               //copy the header data
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Failed\n");
        return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)                                                        //encode the magic string
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Encoding Magic String Failure\n");
        return e_failure;
    }
    if(encode_secret_file_extn_size(encInfo)==e_success)                                                             //encode the secret file extension size
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Encoding Secret File Extension Size Failure\n");
        return e_failure;
    }
    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)                                          //encode secret file extension
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Encoding Secret File Extension Failure\n");
        return e_failure;
    }   
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)                                           //encode the secret file size
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Encoding Secret File size failure\n");
        return e_failure;
    }
    if(encode_secret_file_data(encInfo)==e_success)                                                                     //encode the secret file data
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Encoding Secret File data Failure\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)                            //copy the remaining data from source image to output image
    {
        printf("INFO: Done\n");
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		fclose(encInfo->fptr_stego_image);
    }
    else
    {
        printf("INFO: Encoding Secret File remaining Copy failure\n");
        return e_failure;
    }
    return e_success;
    
}
Status check_capacity(EncodeInfo *encInfo)
{
   encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);                                               //check the image capacity to encode 
   printf("INFO: Cheking for %s size\n",encInfo->secret_fname); 
   if((encInfo->size_secret_file=get_file_size(encInfo->fptr_secret))>1)                                                    //check the secret file size 
   {
     printf("INFO: Done. Not Empty\n");
   }
   else
   {
       printf("INFO: Empty Secret File\n");
       return e_failure;
   }
   printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
   if(encInfo->image_capacity>=54+(strlen(MAGIC_STRING)+4+strlen(encInfo->extn_secret_file)+4+encInfo->size_secret_file)*8)   //check the overall size to encode
   {
      
       return e_success;
   }
   else
   {
       printf("INFO: %s file is very small to encode\n",encInfo->src_image_fname);
       return e_failure;
   }
}
uint get_file_size(FILE *fptr)
{
    
    fseek(fptr,0,SEEK_END);                                                                                                  //get the secret file size
    uint secret_file_size=ftell(fptr);

    return secret_file_size;
}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    rewind(fptr_dest_image);
    printf("Copying Image Header\n");
    unsigned char arr[54];
    fread(&arr,sizeof(char),54,fptr_src_image);                                                                             //read 54 bytes from source image
    if(ferror(fptr_src_image))                                                                                              //do error handling
    {
        fprintf(stderr,"INFO: Error while fetching data from Source Image\n");                                              
        clearerr(fptr_src_image);
        return e_failure;
    }
    fwrite(&arr,sizeof(char),54,fptr_dest_image);                                                                           //write the 54 bytes into output image
    if(ferror(fptr_dest_image))                                                                                            //do error handling
    {
        fprintf(stderr,"INFO: Error while fetching data from Destination Image\n");
        clearerr(fptr_dest_image);
        return e_failure;
    }
    return e_success;
}
Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{
    if(encode_data_to_image((char*)magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success) //encode the magic string
    {
        printf("INFO: Encoding Magic String Signature\n");
    }
    else
    {
        return e_failure;
    }
    return e_success;
}
Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char arr[MAX_IMAGE_BUF_SIZE];
    int i;
    for(i=0;i<size;i++)
    {
        fread(arr,1,MAX_IMAGE_BUF_SIZE,fptr_src_image);                                                                  //read the 8 bytes from source file 
        if(encode_byte_to_lsb(data[i],arr)==e_success)                                                                   //check the data is encoded
        {
          fwrite(arr,1,MAX_IMAGE_BUF_SIZE,fptr_stego_image);                                                             //write the encoded data to the output image    
        }
        else
        {
            fprintf(stderr,"INFO: Error in Encoding byte to lsb function\n");
            return e_failure;
        }
    }
    return e_success;
}
Status encode_byte_to_lsb(char data,char *image_buffer)
{
    for(int i=0;i<MAX_IMAGE_BUF_SIZE;i++)
    {
       image_buffer[i]=image_buffer[i] & 0xFE;                                                                           //clear the bit
       char bit=(data>>(7-i)) & 0x01;                                                                                    //get the bit
       image_buffer[i]=image_buffer[i] | bit;                                                                            //set the bit
    }
    return e_success;
}
Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Extension Size\n",encInfo->secret_fname);
    uint size=strlen(encInfo->extn_secret_file);
    uint n=MAX_IMAGE_BUF_SIZE*sizeof(int);
    char data[n];
    fread(&data,1,n,encInfo->fptr_src_image);                                                                            //read 32 byte for encoding the extension size
    if(ferror(encInfo->fptr_src_image))                                                                                  //do error handling
    {
        fprintf(stderr,"INFO: Error: While Reading the data from Source Image File\n");
        clearerr(encInfo->fptr_src_image);
        return e_failure;
    }
    for(int i=0;i<n;i++)
    {
        data[i]=data[i]&0xFE;                                                                                              //clear the bit
        char bit=size>>(31-i) & 0x01;                                                                                      //get the bit
        data[i]=data[i]|bit;                                                                                               //set the bit
    }
    fwrite(&data,1,n,encInfo->fptr_stego_image);                                                                           //write the encoded data into the output image
    if(ferror(encInfo->fptr_stego_image))
    {
        fprintf(stderr,"INFO: Error: While Writing the data to the Destination Image File\n");
        clearerr(encInfo->fptr_stego_image);
        return e_failure;
    }
    return e_success;

}
Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File extension\n",encInfo->secret_fname);
    uint size=strlen(file_extn);
    char data[MAX_IMAGE_BUF_SIZE];
    for(int i=0;i<size;i++)
    {
        fread(&data,1,MAX_IMAGE_BUF_SIZE,encInfo->fptr_src_image);                                                            //read the 8 byte of data from source image
        if(encode_byte_to_lsb(file_extn[i],data)==e_success)                                                                  //check the extension is encoded or not
        {
            fwrite(&data,1,MAX_IMAGE_BUF_SIZE,encInfo->fptr_stego_image);                                                     //write the encoded data into the output image
        }
        else
        {
            return e_failure;
        }
    }
    return e_success;
}

Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
    printf("INFO: Endoding %s File  Size\n",encInfo->secret_fname);
    uint len=file_size;
    uint n=MAX_IMAGE_BUF_SIZE * sizeof(int);
    char data[n];
    fread(&data,1,32,encInfo->fptr_src_image);                                                                                 //read 32 byte of data to encode the secret file size
    if(ferror(encInfo->fptr_src_image))                                                                                        //do error handling
    {
        fprintf(stderr,"INFO: Error: While Reading the data from Source Image File\n");
        clearerr(encInfo->fptr_src_image);
        return e_failure;
    }
    for(int i=0;i<n;i++)
    {
        data[i]=data[i] & 0xFE;                                                                                                  //clear the bit
        char bit=len>>(31-i) & 0x01;                                                                                             //get the bit
        data[i]=data[i]|bit;                                                                                                     //set the bit
    }
    fwrite(&data,1,32,encInfo->fptr_stego_image);                                                                                //write the 32 bytes into the output image
    if(ferror(encInfo->fptr_stego_image))                                                                                        //do error handling
    {
        fprintf(stderr,"INFO: Error: While writing the data to the destination image file\n");
        clearerr(encInfo->fptr_stego_image);
        return e_failure;
    }
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    printf("INFO: Encoding %s File data\n",encInfo->secret_fname);
    char data[(encInfo->size_secret_file)+1];
    fread(data,sizeof(char),encInfo->size_secret_file,encInfo->fptr_secret);                                                       //read the secret file size of data to encode the secret file data
    data[encInfo->size_secret_file]='\0';
    if(ferror(encInfo->fptr_secret))                                                                                               //do error handling
    {
        fprintf(stderr,"INFO: Error:While Reading the data from the Source image File\n");
        clearerr(encInfo->fptr_secret);
        return e_failure;
    }
    if(encode_data_to_image(data,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)          //check the secret file data is encoded or not
    {
        fprintf(stderr,"INFO: Error:%s function failed\n","encode_data_to_image()");
        return e_failure;
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    printf("INFO: Copying Left Over data\n");
    int ch;
    while((ch=getc(fptr_src))!=EOF)                                                                                                //copy the remaining data 
    {
        if(ferror(fptr_src))
        {
            fprintf(stderr,"INFO: Error unable to read from source image\n");
            clearerr(fptr_src);
            return e_failure;
        }
        putc(ch,fptr_dest);
        if(ferror(fptr_dest))
        {
            fprintf(stderr,"INFO: Error unable to write to destination image\n");
            clearerr(fptr_dest);
            return e_failure;
        }
    
    }
    return e_success;
}




