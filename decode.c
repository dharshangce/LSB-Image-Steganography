#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status open_file(DecodeInfo *decInfo)
{
    printf("INFO: Opening required files\n");
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"r");                                       //source file 
    printf("Info: Opened %s \n",decInfo->stego_image_fname);
    if(decInfo->fptr_stego_image==NULL)                                                                   //do error handling
    {
        perror("fopen'");
        fprintf(stderr,"INFO: Error Unable to open the file %s\n",decInfo->stego_image_fname);
        return d_failure;
    }
    return d_success;
}

Status read_and_Validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
    char arr[2+1];
    FILE *fp;
    fp=fopen(argv[2],"r");
    if(fp==NULL)
    {
        printf("INFO: Error File Not exist\n");
        return d_failure;
    }
    fread(&arr,1,2,fp);                                                                                  //check the file is bmp or not
    arr[2]='\0';
    if(!(strcmp(arr,"BM")))
    {
        decInfo->stego_image_fname=malloc(20);
        decInfo->stego_image_fname=argv[2];                                                                     
    }
    else
    {
        printf("INFO: Error Pass the Valid Source file\n");
        return d_failure;
    }
    if(argv[3]!=NULL)                                                                                    //check the output file 
    {
        decInfo->output_fname=malloc(20);
        decInfo->output_fname=argv[3];
    }
    else
    {
        char arr[20];
        strcpy(arr,"output");                                                                             //set default  output file name
        decInfo->output_fname=malloc(20);
        strcpy(decInfo->output_fname,arr);
    }
    return d_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ##  Decoding Procedure Started ##\n");
    if(open_file(decInfo)==d_success)                                                                      //check the source file
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Opening Files Failed\n");
        return d_failure;
    }
    if(decode_size(strlen(MAGIC_STRING),decInfo)==d_success)                                                //decode the magic string
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Error in Decoding the Magic String Signature\n");
        return d_failure;
    }
    if(decode_secret_file_extn_size(decInfo)==d_success)                                                     //decode the secret file extention size
    {
        printf("INFO: Done\n");
        
    }
    else
    {
        printf("INFO: Error in Decoding the Output File Extenstion size\n");
        return d_failure;
    }
    if(decode_secret_file_extn(decInfo)==d_success)                                                           //decode the secret file extention
    {
        printf("INFO: Done\n");
        if(strchr((decInfo->output_fname),'.')==NULL)
        {
           printf("INFO: Output File not mentioned. Creating output%s as default\n",decInfo->output_extn); 
           decInfo->output_fname=strcat(decInfo->output_fname,decInfo->output_extn);
        }
		else
		{
			if(strstr(decInfo->output_fname,decInfo->output_extn)==NULL)
			{
			 printf("INFO: Output file extention is not matched so creating default extention\n");
			 decInfo->output_fname=strcat(decInfo->output_fname,decInfo->output_extn);                         //store the output file extention 
			}
		}
        decInfo->out=fopen(decInfo->output_fname,"w");                                                         //output file
		if(ferror(decInfo->out))                                                                               //do error handling
		{
			clearerr(decInfo->out);
			fprintf(stderr,"Error while opening %s\n",decInfo->output_fname);
			return d_failure;
		}

        printf("INFO: Opened %s\n",decInfo->output_fname);      
    }
    else
    {
        printf("INFO: Error in Decoding  the Output File Extenstion\n");
        return d_failure;
    }
    if(decode_secret_file_size(decInfo)==d_success)                                                            //decode secret file size
    {
        printf("INFO: Done\n");
    
    }
    else
    {
        printf("INFO: Error in Decoding %s File Size\n",decInfo->output_fname);
        return d_failure;
    }
    if(decode_secret_file_data(decInfo)==d_success)                                                            //decode the secret file data
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("INFO: Error in Decoding  %s File data\n",decInfo->output_fname);
        return d_failure;
    }

    return d_success;
}
Status decode_size(int size,DecodeInfo *decInfo)
{
    printf("INFO: Decoding Magic String Signature\n");
    char data[size+1];
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);                                                            //set the file pointer                                                               
   if( decode_data_to_image(data,size,decInfo->fptr_stego_image)==d_success)
   {
       data[size]='\0';

     if(decode_magic_string(data)==d_success)                                                                //check the magic string
    {
        return d_success;
    }
   }
   return d_failure;
  
}
Status decode_magic_string(const char *magic_string)
{
    if(!(strcmp(magic_string,MAGIC_STRING)))                                                                   //compare the magic_string with decoded data
    {
       
        return d_success;
    }
    else
    {
        return d_failure;
    }
}
Status decode_data_to_image(char *data,int size,FILE *fptr_stego_image)
{
    char arr[8];
    for(int i=0;i<size;i++)
    {
        fread(arr,sizeof(char),8,fptr_stego_image);                                                               //read the 8 bytes from source image

        data[i]=decode_byte_to_lsb(arr);
        
    }
   
    return d_success;
}
char decode_byte_to_lsb(char *image_buffer)
{
    char ch=0;
    for(int i=0;i<8;i++)
    {
        ch=ch<<1;                                                                                                 //data shifted by one
        ch=(image_buffer[i]&0x01)| ch;                                                                            //get and set the bit
       
    }
    return ch;                                                                                                    //return the character
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    printf("INFO: Decoding the Output File Extenstion Size\n"); 
    char arr[32];
    int c=0;
    fread(arr,sizeof(char),32,decInfo->fptr_stego_image);                                                         //read 32 byts for decode the secret file extention size
	if(ferror(decInfo->fptr_stego_image))                                                                         //do error handling
	{
		clearerr(decInfo->fptr_stego_image);
		fprintf(stderr,"INFO: Error while Opening the %s file",decInfo->stego_image_fname);
		return d_failure;
	}
    for(int i=0;i<32;i++)
    {
        c=c<<1;
        c=(arr[i]&0x01) | c;                                                                                      //get the bit and set the bit
    } 
    decInfo->output_extn_size=c;                                                                                  //store the size into the structure
    return d_success;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    printf("INFO: Decoding the Output File Extenstion\n");
    char data[(decInfo->output_extn_size)+1];
    if(decode_data_to_image(data,decInfo->output_extn_size,decInfo->fptr_stego_image)==d_success)                 //decode the secret file extention
    {
        data[(decInfo->output_extn_size)]='\0';
        strcpy(decInfo->output_extn,data);                                                                        //store the secret file extention to the structure 
       
    }
    else
    {
        return d_failure;
    }
    return d_success;

}
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    printf("INFO: Decoding %s File Size\n",decInfo->output_fname);
    char arr[32];
    int c=0;
    fread(arr,sizeof(char),32,decInfo->fptr_stego_image);                                                        //read 32 bytes to decode the secret file size
	if(ferror(decInfo->fptr_stego_image))                                                                        //do error handling
	{
		clearerr(decInfo->fptr_stego_image);
		fprintf(stderr,"Error :While Opening %s file",decInfo->stego_image_fname);
		return d_failure;

	}
    for(int i=0;i<32;i++)
    {
        c=c<<1;
        c=(arr[i]&0x01)|c;                                                                                       //get the bit and set the bit
    }
    decInfo->output_file_size=c;
    return d_success;
}
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("INFO: Decoding the %s File Data\n",decInfo->output_fname);
    char data[(decInfo->output_file_size)+1];
    if(decode_data_to_image(data,decInfo->output_file_size,decInfo->fptr_stego_image)==d_success)                //decode the secret file data
    {
        data[(decInfo->output_file_size)+1]='\0';
        fwrite(data,sizeof(char),decInfo->output_file_size,decInfo->out);                                        //write the seccret file data into the output file
      
    }
	fclose(decInfo->fptr_stego_image);
	fclose(decInfo->out);
    return d_success;
}



   





  
