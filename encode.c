#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_file_size(FILE *fptr)
{
	fseek(fptr,0,SEEK_END);
	long size = ftell(fptr);//find secret file size
	return size;
}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
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
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;

}

Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
	//initialize two strings
	char str1[10]=".bmp";
	char str2[10]=".txt";
		 if(argv[2]!=NULL)
		{
		  if(strstr(argv[2],str1))//check argv[2] is .bmp file or not
       		  {
		       printf("INFO: Source File Saved\n\n");
		       encInfo->src_image_fname=argv[2];//store argv[2] into structer
		  }
		  else
		  {
			printf("Error: please enter .bmp file\n"); //if argv[2] is not .bmp file print error messag
			return e_failure;
		  } 
		}
		else
                {
                        printf("Error: please enter .bmp file\n");//if argv[2] is not .bmp file print error messag
                        return e_failure;
                }

		if(argv[3]!=NULL)
		{
		  if(strstr(argv[3],str2))//check argv[3] is .txt file or not
		  {
		     printf("INFO: Secret File Saved\n\n");
		     encInfo->secret_fname = argv[3];//store argv[3] into structer
		  }
		  else
		  {
			printf("Error: please enter .txt file\n"); //if argv[3] is not .txt file print error messag
			return e_failure;
		  }
		}
		else
                {
                     printf("Error: please enter .txt file\n"); //if argv[3] is not .txt file print error messag
                     return e_failure;
                }

		if(argv[4]!=NULL)
		{
		  if(strstr(argv[4],str1))//check argv[4] is .bmp or not
                  {
                     printf("INFO: Destination file saved\n\n");
		     encInfo->stego_image_fname = argv[4];//store argv[4] into structer
                  }
                  else
                  {
                        printf("Error: please enter .bmp file\n");//if argv[4] is not .bmp file print error messag
                        return e_failure;
                  }
                }
		else
                {
	              encInfo->stego_image_fname = "stegno.bmp";//manualy store stegno.bmp into structer
                      printf("INFO: Destination file saved\n\n");
	        }

		return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
     uint src_file_size = get_image_size_for_bmp(encInfo->fptr_src_image);//call get_image_size_for_bmp function to find source image file size
     long size1 = get_file_size(encInfo->fptr_secret); //call get_file_size function to find secret file size
     int size2 = strlen(MAGIC_STRING)*8;//find MAGIC_STRING size
     char str[] = ".";
     char *ptr = strstr(encInfo->stego_image_fname,str);
     int size3 = strlen(ptr)*8;//find secret file extension size
     
     if(src_file_size > (54 + size2 + 32 + size3 + 32 + (size1 * 8)))
     {
	     return e_success;
     }
     else
     {
	     return e_failure;
     }
}
Status encode_byte_to_lsb(char ch,char *arr)
{
  for(int i=0;i<8;i++)//run a loop 8 times
  {
	int res1 = arr[i] & ~(1<<0);//clear a bit from lsb end
	int res2 = (ch & (1<<i))>>i;//get a bit from character 
	int res3 = res1|res2;//replace a bit to lsb end 
	  arr[i] = res3; 
  }
  
}
Status encode_data_to_image(char *data,FILE *fptr_src_image,FILE *fptr_stego_image)
{
	char arr[8];
	int size = strlen(data);//find size of the character pointer
	for(int j=0;j<size;j++)//run loop size time
	{
	fread(arr,8,1,fptr_src_image);//read 8 characters from the source file
	encode_byte_to_lsb(data[j],arr);//call encode_byte_to_lsb function for encodeing data
	fwrite(arr,8,1,fptr_stego_image);//write encoded data into fptr_stego_image file
	}
	return e_success;
}
Status encode_magic_string(char *magic_string,FILE *fptr_src_image,FILE *fptr_stego_image)
{
	encode_data_to_image(magic_string, fptr_src_image, fptr_stego_image);//call encode_data_to_image function for encode magic string data
	return e_success;
}
Status encode_size_to_lsb(char *arr,int size)
{
	for(int i=0;i<32;i++)
        {
        int res1 = arr[i] & ~(1<<0);//clear a bit from lsb end
        int res2 = (size & (1<<i))>>i;//get a bit from character
        int res3 = res1|res2;//replace a bit to lsb end
          arr[i] = res3;
        }
}
Status encode_secret_file_extn_size(int extn_size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char arr[32];//declare one character array
	fread(arr,1,32,fptr_src_image);//read 32 byte data from the fptr_src_image
	encode_size_to_lsb(arr,extn_size);//call encode_size_to_lsb function for encodeing data
        fwrite(arr,32,1,fptr_stego_image);//write 32 byte of data into fptr_stego_image
	return e_success;
}
Status encode_secret_file_extn(char *extn, EncodeInfo *encInfo)
{
   encode_data_to_image(extn,encInfo->fptr_src_image,encInfo->fptr_stego_image);//call encode_data_to_image function for encode extension data
   return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	fseek(encInfo->fptr_secret,0,SEEK_END);//set fseek at the end
	long int size = ftell(encInfo->fptr_secret);//find secret file size by using ftell function
	char arr[size];
	fseek(encInfo->fptr_secret,0,SEEK_SET);//set fseek at first
	fread(arr,1,size,encInfo->fptr_secret);//read size time data into fptr_secret file
	encode_data_to_image(arr, encInfo->fptr_src_image, encInfo->fptr_stego_image);//call encode_data_to_image function for encodeing data
	
	return e_success;
	
}
Status do_encoding(EncodeInfo *encInfo)
{
	//step1: call open_files function(encInfo);
	if(open_files(encInfo) == e_success)//call open_files function for open all files
	{
		printf("INFO: File opened successfully\n\n");

		if(check_capacity(encInfo)==e_success)//call check_capacity function for check source file size is greater or not
		{
		   if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)//call copy_bmp_header function for copy source file header into destination file
		   {
                      printf("INFO: Copying Image Header\n\n");
                      if(encode_magic_string(MAGIC_STRING, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)//call encode_magic_string function for encode magic string data into destination file
		      {
			      printf("INFO: Encoding Magic String Signature\n\n");
			      char str[] = ".";
			      char *ptr = strstr(encInfo->stego_image_fname,str);
			      int extn_size = strlen(ptr)*8;//find secret file extension size
			      if(encode_secret_file_extn_size(extn_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)//call encode_secret_file_extn_size for encode extension size into destination file
			      {
				      printf("INFO: Encoding secret.txt File Extenstion Size\n\n");
				      char str1[] = ".";
				      char *extn = strstr(encInfo->secret_fname,str1);
				      if(encode_secret_file_extn(extn,encInfo) == e_success)//call encode_secret_file_extn function for encode secret file extension
				      {
					      printf("INFO: Encoding secret.txt File Extenstion\n\n");
					      fseek(encInfo->fptr_secret,0,SEEK_END);//set fseek at the end
					      long int file_size = ftell(encInfo->fptr_secret);//find secret file size
                                              if(encode_secret_file_extn_size(file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)//call encode_secret_file_extn_size for encode secret file size 
					      {
		                                printf("INFO: Encoding secret.txt File Size\n\n");
                                                if(encode_secret_file_data(encInfo) == e_success)//call encode_secret_file_data function for encode all secret data into destination file
						{
							printf("INFO: Encoding secret.txt File Data\n\n");
							if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)//call copy_remaining_img_data function for copy remaining source file data into destination file
							{
								printf("INFO: Copying Left Over Data\n\n");
								printf("INFO: ## Encoding Done Successfully ##\n\n");
							}
							else
							{
								printf("Error: not copy remaining data\n");
							}
						}
						else
						{
							printf("Error: not encode secret file data\n");
						}
					      }
                                              else
					      {
	                                         printf("Error: not encode secret file size\n");
					      }						 
       				      }
				      else
				      {
					      printf("Error: secret file extension not encoded\n");
					      return e_failure;
				      }
			      }
			      else
			      {
                                      printf("Error: secret file extension not encoded\n");
				      return e_failure;
			      }
		      }
		      else
		      {
			      printf("Error: magic string not encoded\n");
			      return e_failure;
		      }
		   }
		   else
		   {
			   printf("Error: bmp header not copied\n");
			   return e_failure;
		   }
                    
		}
		else
		{
                   printf("Error: file size is small\n");
		   return e_failure;
		}
	}
	else
	{
		printf("Error: file is not opened\n");
		return e_failure;
	}
}
uint copy_bmp_header(FILE *fptr_src_image,FILE *fptr_stego_image)
{
   char ch[54];//declare one character array
   fseek(fptr_src_image,0,SEEK_SET);//set fseek at first
   fread(ch,1,54,fptr_src_image);//read first 54 bytes from the source file
   fwrite(ch,1,54,fptr_stego_image);//write 54 bytes as it is into destination file
   return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src_image,FILE *fptr_stego_image)
{
	char ch;//declare one character
	while((fread(&ch,1,1,fptr_src_image))!= 0)//run the loop untill NULL caharacter
	{
	     fwrite(&ch,1,1,fptr_stego_image);//write all data into destination file 
	}
	return e_success;
}
