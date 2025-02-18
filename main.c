/*
Description:

Name: Rohit Andani
Date: 10/10/2024
Project Name: LSB Image Segnography
Description: Hinding text data into the image storeing in another image file and extracting text data by decoding
Batch: 24018G
*/

#include <stdio.h>
#include "decode.h"
#include "encode.h"
#include "types.h"
#include <string.h>

int main(int argc,char *argv[])
{
	  int op_type;
	  if(argc < 2)//check arguments grater then 2 or not
	  {
	    printf("Erroe: Please enter few many arguments\n");
	    return 0;
	  }
	  else
	  {
	     op_type = check_operation_type(argv[1]);
	  }
	if(op_type == e_encode)
	{
		printf("INFO: ## Encoding Procedure Started ##\n\n");
		//creating structure variable
		EncodeInfo encInfo;
		if(read_and_validate_encode_args(argv,&encInfo) == e_success)
		{
			//proced to next
			do_encoding(&encInfo);//call do_encoding function
		}
		else
		{
			printf("INFO: Read and validation failed..\n");
			return 0;
		}
	}

	if(op_type == e_decode)
	{
		printf("INFO: ## Decoding Procedure Started ##\n\n");
		//creating structer variable
		DecodeInfo decInfo;
		if(read_and_validate_decode_args(argv, &decInfo) == e_success)
		{
			//proced to next
			do_decoding(&decInfo);//call do_decoding function
		}
		else
		{
			printf("INFO: Read and validation failed..\n");
			return 0;
		}
	}

}
OperationType check_operation_type(char *arg)//check the argument typy
{
	if(!(strcmp(arg,"-e")))
	{
		return e_encode;//return e_encode to function
	}
	else if(!(strcmp(arg,"-d")))
	{
		return e_decode;//return e_decode to function
	}
	else
	{
		return e_unsupported;//return e_unsupported to function
	}

}

