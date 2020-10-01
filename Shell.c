#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  char ** history=  malloc(sizeof(char *) * 15);
  int *pid_array = (int*) malloc(sizeof(int) * 15);
  int index=0;
  int hist_index=0;


  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input

    //removed semicolon due to error Hw1.c:36:55: warning: while loop has empty body [-Wempty-body]
    //while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) )
    ;
     //cmd_str[0]='\n';
     if(cmd_str[0]=='\n')
     {
      continue;
     }

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;

    char *working_str  = strdup( cmd_str );

  //  printf("cmd: %s\n",cmd_str);
    //saves user input here
    //check for first character on input and if exit or quit don't add on array
    if((cmd_str[0]!= '\n') && (cmd_str[0]!='e') && (cmd_str[0]!='q'))
    {
        printf("here\n");
        history[hist_index] = malloc(strlen(cmd_str));
        strncpy(history[hist_index],cmd_str,strlen(cmd_str));
        printf("word: %s\n",history[hist_index]);
        hist_index++;
        if(hist_index>14)
        {
          hist_index=0;
        }
      printf("cmd value inside: %s\n",cmd_str);
    }

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      //hold =strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen(token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }

        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    // int token_index  = 0;
    // for( token_index = 0; token_index < token_count; token_index ++ )
    // {
    //   printf("token[%d] = %s\n", token_index, token[token_index] );
    // }


  //  printf("token:%c\n",token[0][0]);
       //exit if quit or exit
      //if(strcmp(token[token_count-token_count],"exit")==0 || strcmp(token[token_count-token_count],"quit")==0)
      if(strcmp(token[0],"exit")==0 || strcmp(token[0],"quit")==0)
       {
         printf("exiting\n");
         exit(0);
      }

      else if(strcmp(token[0],"cd")==0)
      {
        int ret;

        printf("cd called\n");
        chdir(token[1]);


      }
      //else if(token[0][0]=='!')
      // else if(strcmp(token[0],"history")==0)
      // {
      //   int i;
      //    if(history_index!=0)//means its not full
      //    {
      //      for(i=0; i<;i++)
      //      {
      //        printf()
      //      }
      //    }
      //   else //its full
      //   {
      //
      //
      //   }
        //  printf("token number:%c\n",token[0][1]);
        // // printf("view history\n");
      //}
      else if(strcmp(token[0],"showpids")==0)
      {
        int current =0;
        int i,j,k;
        int counter=0;

        //find how many elements are in the array

        for(i=0; i<15;i++)
        {
          if(pid_array[i]!=0)
          {
            counter++;
          }
        }

        if(counter<15) //if array is not full
        {
          for (j=0; j<counter;j++)//print pids less than 15
          {
            printf("Pids %d %d\n",j,pid_array[j]);
          }
        }
        else //if array is full
        {
          current = index;
          for (k =0; k<15;k++)
          {
            printf(" %d %d\n",k,pid_array[current]);
            current++;
            if(current > 14)
            {
             current = 0;
            }
           }
        }
    }
    else if(strcmp(token[0],"history")==0)
    {
      int counter=0;
      int i,j,k,l;
      printf("asking to view history\n");
      for(i=0;i<15;i++)
      {
        if(history[i]!=0)
        {
          counter++;
        }
      }
      if(counter<15)
      {
        for(j=0; j<counter;j++)
        {
          printf("%d %s\n",j,history[j]);
        }

      }
      else
      {
        k=hist_index;
        for(l=0; l<15;l++)
        {
          printf("%d %s\n",l,history[k]);
          k++;
          if(k>14)
          {
            k=0;
          }

        }
      }

    }
    else if(token[0][0]=='!')
    {
      printf("value with !: %c\n",token[0][1]);
      int a= atoi(&token[0][1]);
      printf("a :%d\n",a);
      if(a>15)
      {
        printf("Command not in history\n");
      }
      else
      {
      //  history[a]//do we call fork agaain or do we run it in child pid?
      }

    }
      //if not quit
      else
      {
        pid_t pid =fork();
        if(pid == 0)
         {
          int ret = execvp(token[0],&token[0]);

          if(ret== -1)
           {
             printf("%s: command not found\n",token[0]);
             exit(0);
           }
           else
           {
             printf("success\n");
           }
           //mkdir is called by exec no need to add code
           //when I had this mkdir it was causing this error
          // int hold = mkdir(token[1],0700);//should this be called before exec or after exec? And after I make directory
           // when I do ls it says failure, is it because we are not making a new file so it is not being called
           // if(hold == 0)
           // {
           //   printf("succesful\n");
           // }
           // else if(hold == -1)
           // {
           //   printf("failure\n");
           // }
         }
         else
         {
           //store pid values
           pid_array[index++] = pid;
           if(index>14)
           {
             index = 0;
           }

           int status;
           wait(& status);
         }
      }
    free( working_root );
  }
  return 0;
}
