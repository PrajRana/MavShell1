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
  //stores all user input
  char ** history=  malloc(sizeof(char *) * 15);
  //stores pid IDs
  int *pid_array = (int*) malloc(sizeof(int) * 15);
  //starts pid history array index from zero
  int index=0;
  //starts history array index from zero
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
    //moved semicolon to next line to avoid seg fault
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) )
    ;
     //handles enter as input
     if(cmd_str[0]=='\n')
     {
      continue;
     }


    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int  token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;

    char *working_str  = strdup( cmd_str );

    //saves user input here if input is not enter,exit,quit, and !n
    if((cmd_str[0]!= '\n') && (cmd_str[0]!='e') && (cmd_str[0]!='q') &&(cmd_str[0]!='!'))
    {   //malloc each index in history array to store history
        history[hist_index] = malloc(strlen(cmd_str));
        strncpy(history[hist_index],cmd_str,strlen(cmd_str));
        hist_index++;
        if(hist_index>14)
        {
          hist_index=0;
        }
    }

    //if user input is !n, it strtoks input and checks for the n index
    //in history array
    if(cmd_str[0]=='!')
    {
     char *c=strtok(&cmd_str[1],"\n");
     int a= atoi(c);
     //prevents from segfaulting if user types higer value than the current
     //highest index in array
     if(a > hist_index  && a < 15)
     {
       printf("out of bounds\n");
       continue;
      }
      //handles input higer than 15 or less than zero
     else if(a > 15 || a <0)
     {
       printf("index: %d\n",hist_index);
       printf("Command not in history\n");
       continue;
     }
     //if given index is valid then copy that command from history
     //into working_str
     else
     {
       strcpy(working_str,history[a]);
     }
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
      if( strlen(token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }

        token_count++;
    }

       //exit if input is quit or exit
      if(strcmp(token[0],"exit")==0 || strcmp(token[0],"quit")==0)
       {
         printf("exiting\n");
         exit(0);
      }
      //if input is cd then change directory
      else if(strcmp(token[0],"cd")==0)
      {
        int hold;
        hold=chdir(token[1]);
        //if return value is less than 0 that means directory directory
        //does not exist
        if(hold<0)
        {
          printf("Directory does not exist\n");
        }

      }
      //if input is showpids
      else if(strcmp(token[0],"showpids")==0)
      {
        int current =0;
        int i,j,k;
        int counter=0;

        //count total number of elemnts in an array
        for(i=0; i<15;i++)
        {
          if(pid_array[i]!=0)
          {
            counter++;
          }
        }
        //if array has less than 15 elements
        if(counter<15)
        {
          for (j=0; j<counter;j++)
          {
            printf("%d: %d\n",j,pid_array[j]);
          }
        }
        //if array has more than 15 elements
        else
        {
          current = index;
          for (k =0; k<15;k++)
          {
            printf("%d %d\n",k,pid_array[current]);
            current++;
            if(current > 14)
            {
             current = 0;
            }
           }
        }
    }
    //if input is history
    else if(strcmp(token[0],"history")==0)
    {
      int counter=0;
      int i,j,k,l;
      //count number of elements in history array
      for(i=0;i<15;i++)
      {
        if(history[i]!=0)
        {
          counter++;
        }
      }
      //if array has less than 15 elements
      if(counter<15)
      {
        for(j=0; j<counter;j++)
        {
          printf("%d %s",j,history[j]);
        }

      }
      //if array has more than 15 elements
      else
      {
        k=hist_index;
        for(l=0; l<15;l++)
        {
          printf("%d %s",l,history[k]);
          k++;
          if(k>14)
          {
            k=0;
          }

        }
      }

    }
    //handles all other input other than checked condtion above
    else
      {
        pid_t pid =fork();
        if(pid == 0)
         {
          int ret = execvp(token[0],&token[0]);
          //invalid command
          if(ret== -1)
           {
             printf("%s: command not found\n",token[0]);
             exit(0);
           }
           else
           {
             printf("success\n");
           }
         }
         //error occured during fork call
         else if(pid<0)
         {
           printf("fork call unsuccesful..\n");
         }
         //store pid values in parent because child will always be zero
         else
         {
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
