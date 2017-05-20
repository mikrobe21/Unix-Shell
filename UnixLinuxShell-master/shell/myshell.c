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

int mysh_cd(char **args);
int mysh_exit(char **args);
int mysh_pwd(char **args);

char *builtin_str[] = {
  "cd", "exit", "pwd"
};

int (*builtin_func[]) (char **) = {
  &mysh_cd, &mysh_exit, &mysh_pwd
};

int mysh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int mysh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "mysh: need argument to cd \n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("mysh");
    }
  }
  return 1;
}

int mysh_pwd(char **args)
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    return 1;
}

int mysh_exit(char **args)
{
  return 0;
}

int mysh_launch(char **args)
{
  pid_t pid, wpid;
  int status;
  pid = fork();
  
  if (pid == 0) 
  {    
    
   i  if (strcmp(args[0], "ls") == 0)
    {
    	execvp("/Users/michaelrobertson/Desktop/shell/ls", args);
    }
    else if(strcmp(args[0], "cp") == 0)
    {
        execvp("/Users/michaelrobertson/Desktop/shell/cp", args);
    }
    else if(strcmp(args[0], "cat") == 0)
    {
        execvp("/Users/michaelrobertson/Desktop/shell/cat", args);
    }
    else
    {
    	printf("Unknown input! \n");
    }	 
    exit(EXIT_FAILURE);
  } 
  else if (pid < 0) 
  {
	perror("mysh");
  } 
  else 
  {
    do 
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

int mysh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < mysh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) 
    {
      return (*builtin_func[i])(args);
    }
  }
  return mysh_launch(args);
}

#define MYSH_RL_BUFSIZE 1024
char *mysh_read_line(void)
{
  int bufsize = MYSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "mysh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += MYSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "mysh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define MYSH_TOK_BUFSIZE 64
#define MYSH_TOK_DELIM " \t\r\n\a"
char **mysh_split_line(char *line)
{
  int bufsize = MYSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "mysh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, MYSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += MYSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "mysh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, MYSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

void mysh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("myshell$ ");
    line = mysh_read_line();
    args = mysh_split_line(line);
    status = mysh_execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv)
{

  mysh_loop();


  return EXIT_SUCCESS;
}
