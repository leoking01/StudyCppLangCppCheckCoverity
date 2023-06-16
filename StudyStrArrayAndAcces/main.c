#include <stdio.h>
char testArr[8][3]={
    "","\0","a\0",
    "a","ab","ab\0",
    "abc","abc\0"
};

void info(char *  testArr){
    printf("\t info: \n");
    printf("\t testArr=%s\n",testArr);
    printf("\t strlen(testArr)=%d\n",strlen(testArr) );
}

void setThird(char *  testArr){
    printf("\t setThird: \n");
   if(0) testArr[2]='\0';

   if(strlen(testArr)>=2 )testArr[2]='\0';
  else if(strlen(testArr)==1 )testArr[1]='\0';
  else if(strlen(testArr)==0 )testArr[0]='\0';
}

int chekIfEnd(char *  testArr){
    int res = 0;
    while(*testArr!='\0'){
        res++;
        testArr+=1;
    }
    printf("\t chekIfEnd, res=%d\n",res);
    return res;
}

int main()
{
    printf("Hello World!\n");
    for(int i=0;i<8;i++){
        printf("---> i=%d\n",i);
        info(testArr[i]);
//        setThird(testArr[i]);
        chekIfEnd(testArr[i]);
        info(testArr[i]);
    }
    return 0;
}
