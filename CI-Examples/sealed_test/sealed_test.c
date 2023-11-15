#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char** argv) {
   FILE *fp_write,*fp_read;
   char buffer[100];
   int ret;
   if (argc != 2)
        errx(EXIT_FAILURE, "Usage: %s <plain file>", argv[0]);
   size_t arglen=strlen(argv[1]);
   char *input_file=argv[1];
   char* en_filename=malloc(arglen+17); //Enough to hold the whole string
   strcpy(en_filename,"tmp_enc/");
   strcat(en_filename,argv[1]);
   strcat(en_filename,"_encrypted");
   ret = access(input_file, F_OK);
   if (ret < 0)
      errx(EXIT_FAILURE, "Input file is not accessible");
   fp_read = fopen(input_file, "r");
   /* Open file for both reading and writing */
   fp_write = fopen(en_filename, "w+");


   /* Seek to the beginning of the file */
   fseek(fp_read, 0, SEEK_SET);

   /* Read and display data */
   while((fread(buffer, 50, 1, fp_read)>0)){
        fwrite(buffer , 50 , 1 , fp_write );
        printf("%s\n", buffer);
   }
   fclose(fp_write);
   fclose(fp_read);
   
   return(0);
}
