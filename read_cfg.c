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


int main ()
{
    char port[256]; int flag_port=0;
    char path[256]; int flag_path=0;
    char line[256];
    char str[100]; 
    int linenum=0;
    char standart[]="# coments\nPORT 8080\nPATH www/index.html";

    FILE * fp;
    if((fp=fopen("config.txt", "r+"))==NULL)
    {
        printf ("Cannot open file,create new file \n");
        fp=fopen("config.txt", "a+t");
        fputs("# coments\nPORT 8080\nPATH www/index.html", fp); 
        /////рекурсивный вызов самой себя 
    }
    while(fgets(line, 256, fp) != NULL)
    {
        char buff1[256], buff2[256];

        linenum++;
        if(line[0] == '#') continue;

        if(sscanf(line, "%s %s", buff1, buff2) != 2)
        {
                fprintf(stderr, "Syntax error, line %d\n", linenum);
                continue;
        }

        
        if (strcmp(buff1,"PORT")==0)
        {   
            flag_port=1;
            strcpy(port,buff2);
        }
        if (strcmp(buff1,"PATH")==0)
        {   
            flag_path=1;
            strcpy(path,buff2);
        }
    }
    if (flag_port==0|| flag_path==0)
    {
        printf("BAD config file,create new file,old file will be renamed ");
        long int ttime = time (NULL);
        strcpy(str,ctime (&ttime));

        for(int i = 0; str[i] != '\0'; i++)
        {
        if(str[i]== ' '|| str[i]== '\n')
        {
            str[i]='_';
        }
        }
        strcat(str,"config.txt");
        printf("Получили: %s\n",str);
        if (-1 == rename ("config.txt",str))
        printf ("Ошибка переименования файла, удалите configs.txt \n");
        else 
        printf ("Выполнено переименование\n");
        fp=fopen("config.txt", "a+t");
        fputs(standart, fp);
        /////рекурсивный вызов самой себя 
    }
    
    printf("\tlen_port-%d\n\tlen_path-%d\n",strlen(port),strlen(path));
    printf("\tport-%s\n\tpath-%s\n",port,path);
}