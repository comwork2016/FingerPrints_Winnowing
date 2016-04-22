#include "DocumentDao.h"
#include "DocumentOperation.h"
#include <time.h>

int main()
{
    clock_t start,finish;
    start = clock();
    //设置全局环境本地化
    std::locale::global(std::locale(""));
    std::string str_InputDir = "./in/";
    //将一个目录中的文件加入到数据库中
    //DocumentOperation::AddDirectoryDocuments(str_InputDir);
    DocumentOperation::AddDocument("./in/Winnowing");
    //查询数据泄露
    std::string str_LeakDoc = "./test/leak.txt";
    //DocumentOperation::SearchLeak(str_LeakDoc);
    finish = clock();
    double duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::wcout<<std::endl<<std::endl<<L"run program in "<<duration<<L" secs"<<std::endl<<std::endl;
    return 0;
}
