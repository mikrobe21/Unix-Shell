#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<pwd.h>
#include<grp.h>
#include<string.h>
#include<time.h>
#include<libgen.h>
#include <dirent.h>

int main(int argc, char** argv)
{
		
	if (argc < 2)
	{
		DIR           *d;
  		struct dirent *dir;
  		d = opendir(".");
  		if (d)
  		{
    		while ((dir = readdir(d)) != NULL)
    		{
      		    printf("%s\n", dir->d_name);
    		}

    	closedir(d);
  }
        return 1;
	}

	struct stat buf;
	if (lstat(argv[1], &buf) < 0)
	{
		perror("can't open file");
		return 1;
	}

	 if (strcmp(argv[1], "-l") == 0)
    	{
        for (int i = 1; i < argc; i++)
            argv[i] = argv[i+1];
    	}
	
	
	unsigned char type;
	if (S_ISDIR(buf.st_mode)) type = 'd';
	else if (S_ISREG(buf.st_mode)) type = '-';
	else if (S_ISLNK(buf.st_mode)) type = 'l';
	else type = '?';

	unsigned short rights = buf.st_mode & (S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
	
	unsigned int link = buf.st_nlink;
	
	char username[50];
	
	struct passwd *user = getpwuid(buf.st_uid);
	strncpy(username, user -> pw_name, 49);
	username[49] = '\0';
	
	char groupname[50];
	struct group *grp = getgrgid(buf.st_gid);
	strncpy(groupname, grp -> gr_name, 49);
	groupname[49] = '\0';
	
	unsigned int size = buf.st_size;
	
	char time[50];
	strncpy(time, ctime(&buf.st_atime), 50);
	time[49] = '\0';
	int len = strlen(time);
	if (time[len-1] == '\n') time[len-1] = '\0';
	
	printf("%c %3o %u %s %s %u %s %s\n", type, rights, link, username, groupname, size, time, basename(argv[1]));
	
	if(access(argv[1], R_OK) == 0) printf("Read OK\t");
	else printf("No Read\t");
	if(access(argv[1], W_OK) == 0) printf("Write OK\t");
	else printf("No Write\t");
	if(access(argv[1], X_OK) == 0) printf("Execute OK\t");
	else printf("No Execute\t");
	printf("\n");    
}
