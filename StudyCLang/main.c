#include <stdio.h>

int main()
{
    printf("Hello World!\n");

    char car[5]="car\0\0";
printf("car=%s\n", car );
printf("sizeof(car )=%d \n", sizeof(car ) );
printf("strlen(car )=%d \n", strlen(car ) );

int i=0;
while(car[i++]!='\0');
printf("i =%d \n", i );

char carb[5]="cartt";
i=0;
while(carb[i++]!='\0');
printf("i =%d \n", i );
    return 0;
}
