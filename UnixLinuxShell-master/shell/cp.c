#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

int isdir(const char * path)
{
    struct stat bf;
    // read status of the file
    if(stat(path, &bf) == -1){
        printf("isdir(%s), stat(%s) error!\n",path, path);
        return -1;
    }
    if((S_IFMT & bf.st_mode) == S_IFDIR) {
        return 1;
    }
    else
        return 0;
}

//copy file using paths
int cpfile(const char * from, const char * to)
{
    int f1, f2, n;
    char buf[BUFSIZ];
    struct stat old_mode;
    // read status of the old file
    if(stat(from, &old_mode) == -1){
        printf("cpfile(%s, %s), stat(%s) error!\n", from, to, from);
        return 0;
    }
    // open the old file
    if( (f1 = open(from, O_RDONLY)) == -1){
        printf("cpfile(%s, %s), can't open %s.\n", from, to, from);
        return 0;
    }
    // create new file
    if( (f2 = creat(to, old_mode.st_mode)) == -1){
        printf("cpfile(%s, %s), can't create %s.\n",  from, to, to);
        close(f1);
        return 0;
    }
    // int fchmod(int fd, mode_t mode)
    if(fchmod(f2, old_mode.st_mode) == -1){
        printf("cpfile(%s, %s), fchmod(%s) error!\n", from, to, to);
        return 0;
    }
    // read and write
    while((n = read(f1, buf, BUFSIZ)) > 0){
        if(write(f2, buf, n) != n){
            printf("cpfile(%s, %s), write(%s) error!\n", from, to, to);
            close(f1);
            close(f2);
            return 0;
        }
    }
    close(f1);
    close(f2);
    return 1;
}

// get file name or directory name
void getfilename(char * bf, char * name)
{
    int i, n, j;
    n = strlen(bf);
    for(i = n - 1; i >=0 ; i--){
        if(bf[i]=='/'){
            break;
        }
    }
    for(i++, j = 0; i < n; i++, j++)
        name[j] = bf[i];
    name[j] = '\0';
}

// copy a directory, including the files and sub-directories, into a directory that exists
void cpdir(const char * from, const char * to)
{
    char bffrom[BUFSIZ],  bfto[BUFSIZ]; // use to store filepath of the source and destination
    char name[BUFSIZ];
    int flag;
    flag = isdir(from);
    
    strcpy(bffrom, from);
    strcpy(bfto, to);
    
    if( access(to, F_OK ) != -1 ) {
        // file exists
    } else {
        // file doesn't exist
        mkdir(bfto,0777);

        cpdir(bffrom,bfto);
        return;
    }
    
    if(flag == 0) // regular file
    {
        
        getfilename(bffrom, name);  // get file name
        strcat(bfto, "/");
        strcat(bfto, name);
        cpfile(bffrom, bfto);
        return;
    }
    else
        if(flag == 1) // directory
        {
            
            
            // make the same dir
            getfilename(bffrom, name);  // get dir name                  
            strcat(bfto, "/");          // "dir/"
            strcat(bfto, name);         // name= "dir/"
            
            if(strcmp(name, ".")==0 || strcmp(name, "..")==0 ){
                return ; // current and parent!
            }
            struct stat old_mode;    // get the old_mode
            
            if(stat(from, &old_mode) == -1){
                printf("mkdir(%s), stat(%s) error!\n", bfto, bffrom);
                return;
            }
            
            mkdir(bfto, old_mode.st_mode | O_CREAT); // make dir bfto
            chmod(bfto, old_mode.st_mode); // change mode of bfto
            

            // copy the files and subdir in the dir
            DIR * pdir;
            struct dirent * pdirent;
                
            pdir = opendir(bffrom);
            while(1) {
                pdirent = readdir(pdir) ;
                if(pdirent == NULL)
                    break;
                else{
                    strcpy(bffrom, from);//key
                    strcat(bffrom, "/");
                    strcat(bffrom, pdirent->d_name); // subfile or subdir path
                    cpdir(bffrom, bfto)  ;   // nested
                }
            }
            closedir(pdir);

            return ;
        }
        else
            return ;
}

//copy a file
int cpFileFile(const char* from, const char* to){
    char buffer[1024];
    int files[2];
    ssize_t count;
    //open the first argument file
    files[0] = open(from, O_RDONLY);
    
    
    if (files[0] == -1){ /* Check if file opened */
        printf("Unable to open first file or file does not exist\n");
        return -1;
    }
    
    files[1] = open(to, O_WRONLY | O_CREAT , 0644); //open or create
    
    if (files[1] == -1) /* Check if file opened (permissions problems ...) */
    {
        close(files[0]);
        return -1;
    }
    
    while ((count = read(files[0], buffer, sizeof(buffer))) != 0)
        write(files[1], buffer, count);
    
    return 0;
}



int main(int argc, char **argv)
{

    
    
    /* Check for insufficient parameters */
    if (argc < 3 || argc>4){
        printf("usage: ./cp sourceFile destinationFile\n       ./cp [-R] sourceDirectory destinationDirectory\n");
        return -1;
    }
    
    if (argc==3){
        char *check = strrchr(argv[1], '.');
        char *check1 = strrchr(argv[2], '.');
        if (!(check && !strcmp(check, ".txt"))){
            printf("usage: ./cp sourceFile.txt destinationFile.txt\n       ./cp [-R] sourceDirectory destinationDirectory\n");
            return 0;
        }
        else if (!(check1 && !strcmp(check1, ".txt"))){
            printf("usage: ./cp sourceFile.txt destinationFile.txt\n       ./cp [-R] sourceDirectory destinationDirectory\n");
            return 0;
        }
        int exit = cpFileFile(argv[1],argv[2]);
        return exit;
    }
    
    
    if (argc==4){
        char *check = strrchr(argv[2], '.');
        char *check1 = strrchr(argv[3], '.');
        if (check && !strcmp(check, ".txt")){
            printf("usage: ./cp sourceFile.txt destinationFile.txt\n       ./cp [-R] sourceDirectory destinationDirectory\n");
            return 0;
        }
        else if (check1 && !strcmp(check1, ".txt")){
            printf("usage: ./cp sourceFile.txt destinationFile.txt\n       ./cp [-R] sourceDirectory destinationDirectory\n");
            return 0;
        }
        else if (strcmp(argv[1],"-R")==0){              //Recursive flag
            cpdir(argv[2], argv[3]);
        }
        else
                printf("usage: ./cp sourceFile destinationFile\n       ./cp [-R] sourceDirectory destinationDirectory\n");
    }
    
    return 0;
}

