#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>


typedef struct {
 char *ext;
 char *mediatype;
} extn;

//Possible media types
extn extensions[] =
{
   {".gif", "image/gif" },
   {".txt", "text/plain" },
   {".jpg", "image/jpg" },
   {".jpeg","image/jpeg"},
   {".png", "image/png" },
   {".ico", "image/ico" },
   {".zip", "image/zip" },
   {".gz",  "image/gz"  },
   {".tar", "image/tar" },
   {".htm", "text/html" },
   {".html","text/html" },
   {".php", "text/html" },
   {".pdf","application/pdf"},
   {".zip","application/octet-stream"},
   {".rar","application/octet-stream"},
   {0,0} 
};

char *file_open_and_read(char *s);
int main ()   //вытаскиваем имена файлов из GET запросов.
{
   char line[256];
   char buffer[256];
   char buffer2[256];
   FILE *mf;

   mf=fopen("log_server", "r+");
file_open_and_read("log_server");
   while(fgets(line, 256, mf) != NULL)
   {
      
      int lo=0;

      for (int i=0; extensions[i].ext!=NULL;i++)
      {
         char *lo=strstr(line,extensions[i].ext);
         char  *r=strstr(line,"GET /");
         if(lo!=NULL)
         {  
             for (int bf=5,fb=0; bf<=(int)(lo-line-1);bf++,fb++)
            {
               
               buffer[fb]=line[bf];
                
            }
            strcat(buffer,extensions[i].ext);
            printf("%s\n",buffer);
            memset(buffer, '\0', 256);
         }
      }
   }
 
}
//сохранено на всякий случай.
/*if(http_index_html==0)
            {   
                if (!fork())
                {  
                    close(sockfd); // child doesn't need the listener
                    char line[10000];
                    char buffer[256];
                    char buffer2[256];
                    FILE *mf=fdopen (new_fd,"r");

                    //mf=fopen("log_server", "r+");

                    while(fgets(line, request_buffer_size - 1,mf ) != NULL)
                    {
                    
                        int lo=0;

                        for (int i=0; extensions[i].ext!=NULL;i++)
                        {
                            char *lo=strstr(line,extensions[i].ext);

                            if(lo!=NULL)
                            {  
                                for (int bf=5,fb=0; bf<=(int)(lo-line-1);bf++,fb++)
                                {
                                
                                buffer[fb]=line[bf];

                                }

                                strcat(buffer,extensions[i].ext);
                                printf("%s\n",buffer);
                                //file_open_and_read(buffer)
                                //if (send(new_fd, file_open_and_read(buffer), strlen(file_open_and_read(buffer)), 0) == -1)
                                //   perror("send str1");
                                memset(buffer, '\0', 256);
                            }      
                        }
                    }
                    close(new_fd);
                    exit(0);
                }
                */