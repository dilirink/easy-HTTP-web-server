
#include <stdio.h>  // Для printf
#include <time.h>   // Для time, ctime
#include <string.h>

int main (void)
{  
   char str[100];
   
  long int ttime = time (NULL);
strcpy(str,ctime (&ttime));
    
    for(int i = 0; str[i] != '\0'; i++)
    {
        if(str[i]== ' '|| str[i]== '\n')
        {
            str[i]='_';
        }
        
    }
       
   strcat(str,"configs.txt");
 
    printf("Получили: %s\n",str);
    
    if (-1 == rename ("configs.txt",str))
      printf ("Ошибка переименования файла, удалите configs.txt \n");
   else 
      printf ("Выполнено переименование\n");
    return 0;
}
