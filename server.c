//easy HTML server
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

/* 
этапы
1.функция для создания сокета
2.вывод GET запроса через отдельную функцию
3.ответ браузеру ввиде страницы HTML
4.загрузка HTML страниц из файла + GET ответы для подгрузки страниц.
5.конфигурационный файл в котором ПОРТ и место расположения index.html + создание дефолтного файла конфигурации. 
6.много поточность с помошью форков/потоков и теори по обслуживанию клиентов
.
.
.
вывод дефолтной странички по get запросу  с описание сервера и тд.
*/
    // схема создания сокета для сервера
    //getaddrinfo()
    // socket()
    // bind()
    // listen()
    // accept()

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m" // example  printf("This is " RED "red" RESET " and this is " BLU "blue" RESET "\n");


typedef struct {
 char *ext;
 char *mediatype;
 int number;
} extn;

//поисково ответвная структура.
extn extensions[] =
{
   {".gif", "Content-Type: image/gif\r\n\r\n" ,1},
   {".txt", "Content-Type: text/plain\r\n\r\n",2},
   {".jpg", "Content-Type: image/jpg\r\n\r\n" ,3},
   {".jpeg","Content-Type: image/jpeg\r\n\r\n",4},
   {".png", "Content-Type: image/png\r\n\r\n" ,5},
   {".ico", "Content-Type: image/ico\r\n\r\n" ,6},
   {".zip", "Content-Type: image/zip\r\n\r\n" ,7},
   {".gz",  "Content-Type: image/gz\r\n\r\n"  ,8},
   {".tar", "Content-Type: image/tar\r\n\r\n" ,9},
   {".htm", "Content-Type: text/html\r\n\r\n" ,10},
   {".html","Content-Type: text/html\r\n\r\n" ,11},
   {"GET / HTTP/1.1","Content-Type: text/html\r\n\r\n",12},
   {".php","Content-Type: text/html\r\n\r\n" ,13},
   {".pdf","Content-Type: application/pdf\r\n\r\n",14},
   {".zip","Content-Type: application/octet-stream\r\n\r\n",15},
   {".rar","Content-Type: application/octet-stream\r\n\r\n",16},
   {".css","Content-Type: text/css\r\n\r\n"},
   {".js","Content-type: application/javascript\r\n\r\n"},
   {0,0} 
};



#define LEN 10000
//#define PORT "3491"
#define MAX_CONNECTION 1000
char PORT[10];
char path_index[100];
int size_file_open_and_read;
int get_searcher(char *s,int fd_new); //открываем нужный файл и сразу отправляем)

int cfg_reader(); ///читаем конфиг файл (или создаём пустой с деволтным портом и местом расположениея .html)
char *file_open_and_read(char *s);/// получение и чтение данных из файла
void *get_in_addr(struct sockaddr *sa);
int http_request_type(char *recv_message); 
int main(int argc, char *argv[]) //для передачи или порта или управляюших ключей или колличества пользователей НА БУДУЩИЕ:)
{    
    int sock, yes=1,rv;;
    int sockfd, new_fd;  // слушаем 
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // адресная информация
    socklen_t sin_size;
    int http_get_errors = 0; //для обработки ошибок
    int http_index_html = 0;//для проверки запроса Index.html
    ////отправляемые данные
    int i;
    char str1[LEN]="\n\nServer: Web Server in C start!\r\n",
    str2[]="www/index.html";
    char *read_file_buffer;
   // str2[]="<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Example</title>\r\n</head>\r\n<body>\r\n<p>This is an example of a simple HTML page with one paragraph.</p>\r\n</body>\r\n</html>",
    //str3[LEN];
    if (cfg_reader()==-1)
    {
        printf(RED"eror cfg_reader\n"RESET); 
    }//заполняем путь и порт из файла конфинурации!
    
    //read_file_buffer=file_open_and_read(path_index);


   // strcat(str1, str3);
    printf(CYN"\n%s\n\n"RESET, str1);
    //printf(RED"STR3\n%s"RESET, read_file_buffer);

    memset(&hints, 0, sizeof(struct addrinfo)); //занулили
    hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p=servinfo; p!=NULL; p=p->ai_next)
    {
       if((sockfd = socket(p->ai_family,p->ai_socktype ,p->ai_protocol))==-1)
       {
           perror("server:socket");
           exit(1);
       }

       if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))==-1)
       {
            perror("setsockopt");
            exit(1);
       }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }
    
    freeaddrinfo(servinfo);
    
    if (p == NULL)      
    {
	    fprintf(stderr, "server: failed to bind\n");
	    exit(1);
    }

    if (listen(sockfd, MAX_CONNECTION) == -1) 
    {
	    perror("listen");
	    exit(1);
    }
         
    while(1) 
    {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
        perror("accept");
        continue;
        }
        inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr),
        s, sizeof s);
        printf(RESET"server: got connection from %s\n", s);
        //////////////////////////////////////////////////////////////////////////////////////выдача get запроса в терминал
        const int request_buffer_size = 65536;
        char      request[request_buffer_size];
        int bytes_recvd = recv(new_fd, request, request_buffer_size - 1, 0);
        if (bytes_recvd < 0)
        {
          fprintf(stderr, "error recv\n");
          //return ;
        }
        else printf("bytes_recvd=%d",bytes_recvd);
        http_get_errors = http_request_type(request); // проверка GET ли это или нет

        if (http_get_errors == 1)
        {
            printf("\nprishlo chto to strannoe\n");
        }
        if (http_get_errors == 2)
        {
            printf(RESET"\naga GET zapros prishOOOL\n\n");
            
            //http_index_html=strncmp(request, "GET / HTTP/1.1",14); /////////////// ищем в первых 14 символах нужные нам символы
           
           
           // if(http_index_html==0)
           // {
               if (!fork())
               { // this is the child process
                   close(sockfd); // child doesn't need the listener

                    char *razdel=strtok(request,"\n"); // http://all-ht.ru/inf/prog/c/func/strtok.html немного гайда
                    int i=0,breaks=0;
                    while (razdel != NULL) 
                    {
                        // Вывод очередной выделенной части
                        printf ("%d)%s\n",i,razdel);
                        i++;
                        // Выделение очередной части строки
                        breaks=get_searcher(razdel,new_fd);
                        if (breaks=1) {break;}
                        else
                        razdel = strtok (NULL,"\n");
                    }   

                    close(new_fd);
                    exit(0);
                 //   if (send(new_fd, str1, strlen(str1), 0) == -1)
                 //       perror("send str1");
                 //   if (send(new_fd, read_file_buffer, strlen(read_file_buffer), 0) == -1)
                 //       perror("send read_file_buffer");
                    
                }
                close(new_fd);  // parent doesn't need this
          //  }
            
            //else printf(RED"easy"RESET" ERROR not GET / HTTP/1.1\n");
        }

        //request[bytes_recvd] = '\0';
        //printf("request:\n%s\n",request);

        //////////////////////////////////////////////////////////////////////////////////////
        
    }
        
    return 0;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
 
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int http_request_type(char *recv_message)
{
    char massage[255]   = {0};
    int  i = 0;
    
    int ret=0;
    sscanf(&recv_message[i], "%s", massage);

    if (massage[0]=='G' && massage[1]=='E' && massage[2]=='T')
    {
        
        ret=2;
        
    }
    else 
    {
        ret=1;
    }

    return ret;
}

char *file_open_and_read(char *s)
{ 
  char *eror="EROR204";
    
  FILE * ptrFile = fopen( s , "r+b" );
 
  if (ptrFile == NULL) //обраюотка ошибки 204 нет контента
  {
      
      printf("Ошибка файла|%s|\n",s);
      return eror;
     // exit(1);
  }
 
  // определяем размер файла
  fseek(ptrFile , 0 , SEEK_END);                          // устанавливаем позицию в конец файла
  long lSize = ftell(ptrFile);                            // получаем размер в байтах
  rewind (ptrFile);                                       // устанавливаем указатель в конец файла
 
  char * buffer = (char*) malloc(sizeof(char) * lSize); // выделить память для хранения содержимого файла
  if (buffer == NULL)
  {
      fputs("Ошибка памяти", stderr);
      exit(2);
  }
 
  size_t result = fread(buffer, 1, lSize, ptrFile);       // считываем файл в буфер
  if (result != lSize)
  {
      fputs("Ошибка чтения", stderr);
      exit (3);
  }
  size_file_open_and_read=result;
 printf("size read file =%d\n",size_file_open_and_read);
  //содержимое файла теперь находится в буфере
 // puts(buffer);
   // завершение работы
  fclose (ptrFile);

 /* отчистить после использование*/
 // free (buffer);
  
  return buffer;
}

int cfg_reader()
{
    char port[256]; int flag_port=0;
    char path[256]; int flag_path=0;
    char line[256];
    char str[100];
    char standart[]="# coments\nPORT 8080\nPATH folders/";
    int linenum=0;
    
    FILE * fp;
    if((fp=fopen("config.txt", "r+"))==NULL)
    {   
        printf ("Cannot open file,create new file \n");
        fp=fopen("config.txt", "a+t");
        fputs(standart, fp); 
        fclose(fp);
        /////рекурсивный вызов самой себя 
        fp=fopen("config.txt", "r+");
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
    fclose(fp);
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
        {
            printf ("Выполнено переименование\n");
            cfg_reader(); return -1;
        }  
        fp=fopen("config.txt", "a+t");
        fputs(standart, fp);
        fclose(fp);
        /////рекурсивный вызов самой себя 
        
    }
    strcpy(PORT,port); // переписываю в глобальные переменные
    strcpy(path_index,path);
    
   //printf("\tlen_port-%d\n\tlen_path-%d\n",strlen(port),strlen(path)); //поидеи мусор
    printf("\ncfg read:\n\tport-%s\n\tpath-%s\n",port,path);
}


int get_searcher(char *s,int fd_new)
{
   char line[256];
   char *read_file_buffer;
   char buffer[256];
   char buffer2[256];
   FILE *mf;
    char str1[LEN]="HTTP/1.1 200 OK\r\nServer: Web Server in C\r\n",
    str2[]="www/index.html",
    eror[LEN]="HTTP/1.1 204 No Content\r\nServer: Web Server in C\r\n";
   // read_file_buffer=file_open_and_read(char *s)

   // if (send(new_fd, str1, strlen(str1), 0) == -1)
  //  perror("EROR in send in get_searcher");


    for (int i=0; extensions[i].ext!=NULL;i++)
    {
        char *lo=strstr(s,extensions[i].ext);
        
        
        if(lo!=NULL)
        {  //printf("for ====%d",(int)(lo-s));
            for (int bf=5,fb=0; bf<(int)(lo-s);fb++,bf++)
        {
            
            buffer[fb]=s[bf];
            
        }

            printf("buffer size= %d",strlen(buffer));
            int size=strlen(buffer);
            while(1)   /// чистим хвосты у файлов,там какие то кракозабры приклеивались
            {  
                char *istr = strstr (s,buffer);

                //Вывод результата поиска на консоль
                if ( istr == NULL)
                {
               // printf ("Строка не найдена\n");
                buffer[size]='\0';
                size--;
                }
                else
                {   
              //      printf ("Искомая строка начинается с символа \n");
                    break;
                }
            }

        
        //if(lo!=NULL)
        //{  char *tok=strtok(s,".");
        //    for (int bf=5,fb=0; s[bf]=='\0';fb++,bf++)
        //{
        //    
        //    buffer[fb]=s[bf];
        //    
        //}
        if (extensions[i].number==12)
        {   
            strcpy(buffer,path_index);
            strcat(buffer,"index.html");

           // printf(RED"file |%s|  size send html file%d\n"RESET,buffer,strlen(read_file_buffer));
            read_file_buffer = file_open_and_read(buffer);
            if (send(fd_new,str1 , strlen(str1), 0) == -1)
            perror("EROR in send in get_searcher");
            if (send(fd_new, extensions[i].mediatype , strlen(extensions[i].mediatype), 0) == -1)
            perror("EROR in send in get_searcher");
            if (send(fd_new, read_file_buffer , size_file_open_and_read, 0) == -1)
            perror("EROR in send in get_searcher");

            memset(buffer, '\0', 256); 
            return 1;
        }
        else 
            {
            strcpy(buffer2,path_index);
            printf(YEL"file |%s|  size  send any file %d\n"RESET,buffer);
            strcat(buffer,extensions[i].ext);
            strcat(buffer2,buffer);
            printf(BLU"file |%s|  size  send any file %d\n"RESET,buffer,size_file_open_and_read);
            //path_index
            read_file_buffer = file_open_and_read(buffer2);
            if (strstr(read_file_buffer,"EROR204")!=NULL) //обрабатываем момент что файла нету в дериктории
            {   
                printf(RED"file EROR sen 204 No Content\n"RESET);
                if (send(fd_new,    eror , strlen(eror), 0) == -1)
                perror("EROR in send in get_searcher");
            }
            printf(BLU"file |%s|  size  send any file %d\n"RESET,buffer,size_file_open_and_read);
            if (send(fd_new,    str1 , strlen(str1), 0) == -1)
            perror("EROR in send in get_searcher");
            if (send(fd_new, extensions[i].mediatype , strlen(extensions[i].mediatype), 0) == -1)
            perror("EROR in send in get_searcher");
            if (send(fd_new, read_file_buffer , size_file_open_and_read, 0) == -1)
            perror("EROR in send in get_searcher");
            memset(buffer, '\0', 256);  
            return 1;
            }     
        }
        
    }
    
   close(fd_new);
 
}
// server: got connection from 192.ххх.0.ххх
// request:
// GET / HTTP/1.1
// Host: dilirink.dlinkddns.com:3491
// Connection: keep-alive
// Cache-Control: max-age=0
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
// Accept-Encoding: gzip, deflate
// Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7
// 
// 
// server: got connection from 192.168.0.87
// request:
// GET /favicon.ico HTTP/1.1
// Host: dilirink.dlinkddns.com:3491
// Connection: keep-alive
// Pragma: no-cache
// Cache-Control: no-cache
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36
// Accept: image/webp,image/apng,image/*,*/*;q=0.8
// Referer: http://dilirink.dlinkddns.com:3491/
// Accept-Encoding: gzip, deflate
// Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7


