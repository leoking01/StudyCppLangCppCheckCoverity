#include <iostream>
#include <string>
using namespace std;

//int main()
//{
//    cout << "Hello World!" << endl;
//    return 0;
//}


#include "logfile.h"


int main(void)
{
    cout << "saveto : " <<"testlogfile.log"<<  endl;
    LogFile lf("testlogfile.log", 1024, 8, false);
    for (auto i = 0; i < 200; i++) {
        lf.Append(std::to_string(i) + ": this is a very very very very very very very very very very very very very very vvery very very very very ery long log");
    }
    cout << "finish. " << endl;
}
