#include <stdio.h>  // Для printf
#include <string.h> // Для strstr
int main (void)
{    
   // Массив со строкой для поиска
   char s [11]="0123456789";
   // Набор символов, которые должны входить в искомый сегмент
   char buffer [10]="345a";
   // Переменная, в которую будет занесен адрес первой найденной строки
   //char *istr;

   //Поиск строки
      printf("buffer size= %d",strlen(buffer));
            int size=strlen(buffer);
        while(1)
        {  
           char *istr = strstr (s,buffer);

            //Вывод результата поиска на консоль
            if ( istr == NULL)
            {
            printf ("Строка не найдена\n");
            buffer[size]='\0';
            size--;
            }
            else
            {   
                printf ("Искомая строка начинается с символа \n");
                break;
            }
        }

   return 0;
}