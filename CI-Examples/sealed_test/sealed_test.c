#include <stdio.h>
#include <string.h>

int main () {
   FILE *fp,*fp_read;
   char buffer[100];

   fp_read = fopen("input.txt", "r");
   /* Open file for both reading and writing */
   fp = fopen("tmp_enc/encrypted_out.txt", "w+");


   /* Seek to the beginning of the file */
   fseek(fp_read, 0, SEEK_SET);

   /* Read and display data */
   fread(buffer, 50, 1, fp_read);
   fwrite(buffer , 50 , 1 , fp );
   printf("%s\n", buffer);
   fclose(fp);
   fclose(fp_read);
   
   return(0);
}
