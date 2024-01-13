#include <stdio.h>
#include "encode.h"
#include <string.h>
#include "types.h"
#include "decode.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;                                                                                               //declare the structure variables
    DecodeInfo decInfo;
	if(argc>1)                                                                                                         //check the command line arguments
	{
	
    if(check_operation_type(argv)==e_encode)                                                                            //check the operation
    {
        printf("Selected Encoding\n");
        if(argc>=4)
        {
          if(read_and_validate_encode_args(argv,&encInfo)==e_success)                                                   //check the read and Validate function
          {
              printf("INFO: Read and Validate Success\n");
              if(do_encoding(&encInfo)==e_success)                                                                      //check the all files are encoded or not
              {
                  printf("##========== Encoding Done Successfully ==========##\n");
              }
              else
              {
                  printf("##========== Encoding Failed ==========##\n");
                  return e_failure;
              }
          }
		  else
		  {
			  printf("INFO:Read and Validate Failed\n");
			  return e_failure;
		  }
        } 
        else
        {
            printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
        }
    }
    else if(check_operation_type(argv)==e_decode)
    {
      printf("Selected Decoding\n");
      if(argc>=3)
      {
          if(read_and_Validate_decode_args(argv,&decInfo)==d_success)                                                     //check the read and and validate function
          {
              printf("INFO: Read and Validate Success\n");
              if(do_decoding(&decInfo)==d_success)                                                                        //check all files are decoded or not
              {
                  printf("##========== Decoding Done Successfully ==========##\n");
              }
              else
              {
                  printf("##========== Decoding Failed ==========##\n");
                  return d_failure;
              }
          }
	  }
          else
          {
              printf("%s : Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
          }
    }
    
    }
    else
    {
        printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
        printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
    }

    return 0;
}
OperationType check_operation_type(char *argv[])
{

    if(!strcmp(argv[1],"-e"))                                                                                             //check the operation is encode or decode
            return e_encode;
    else if(!strcmp(argv[1],"-d"))
        return e_decode;
    else
        return e_unsupported;
}

