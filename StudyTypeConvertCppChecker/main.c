//#include <iostream>
//using namespace std;

#include <stdio.h>
typedef  struct  StruA{
    int age;
}tpStruA;

void funcA(const tpStruA *  var,const char * name  ){
    printf("name = %s, var->age= %d\n", name, var->age);
}

int main()
{
//    cout << "Hello World!" << endl;
    tpStruA var ;
    var.age = 3;
    char * name = "LiSan";
    funcA(  &var,name ) ;
    return 0;
}
