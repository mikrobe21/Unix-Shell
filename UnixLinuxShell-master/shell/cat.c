#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
 	FILE *file;
               char line[100];
            
               file = fopen(argv[1],"r");
            
               // reading file line by line entering to buffer line
               while(fscanf(file,"%[^\n]\n",line)!=EOF)
               {
                    printf("%s\n", line);
               }
               // file close
               fclose(file);
               return 0;
}