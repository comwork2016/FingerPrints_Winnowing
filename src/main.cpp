#include "DocumentDao.h"
#include "DocumentOperation.h"

int main()
{
    //设置全局环境本地化
    std::locale::global(std::locale(""));
    std::string str_InputDir = "./in/";
    //将一个目录中的文件加入到数据库中
    DocumentOperation::AddDirectoryDocuments(str_InputDir);
    //查询数据泄露
    std::string str_LeakDoc = "./test/leak.txt";
    DocumentOperation::SearchLeak(str_LeakDoc);
    return 0;
}
