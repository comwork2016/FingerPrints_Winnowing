#include "Document.h"

//文档构造函数
Document::Document(std::string str_DocPath)
{
    this->m_strDocPath = str_DocPath;
    int n_SeparatorIndex = str_DocPath.find_last_of("/");
    this->m_strDocName = str_DocPath.substr(n_SeparatorIndex+1);
    //读取文档内容
    int n_ReadStats = ReadDocument();
    if(n_ReadStats == ERROR_OPENFILE)
    {
        std::cout<<"open file "<<this->m_strDocPath<<" error"<<std::endl;
        return;
    }
    //对文档内容进行分词处理
    this->m_vecSplitedHits = SplitContents::SplitContentsToVector(this->m_strContents);
/*  遍历输出分词结果
    std::wcout<<this->m_vecSplitedHits.size()<<std::endl;
    for(std::vector<SplitedHits>::iterator it=this->m_vecSplitedHits.begin();it!=this->m_vecSplitedHits.end();it++)
    {
        SplitedHits sh_hits = *it;
        std::wcout<<sh_hits.words<<"["<<sh_hits.offset<<","<<sh_hits.length<<","<<sh_hits.hashValue<<"]"<<std::endl;
    }
*/
    //利用分词结果 计算文档的SimHash值
    this->m_lSimHash = SimHash::CalcSimHash(this->m_vecSplitedHits);
/*输出文档的simhash值
    std::cout<<this->m_strDocName<<"---"<<this->m_lSimHash<<std::endl;
*/
}

//读取文件内容
int Document::ReadDocument()
{
    std::ifstream ifs_Doc;
    ifs_Doc.open((char *)this->m_strDocPath.c_str(),std::ios_base::in);
    if(!ifs_Doc.is_open())
    {
        return ERROR_OPENFILE;
    }
    std::stringstream ss;
    ss<<ifs_Doc.rdbuf();
    this->m_strContents = ss.str();
    ifs_Doc.close();
    return OK_READFILE;
}

//挑选文件指纹
void Document::PickFingerPrints()
{
    this->m_FingerPrints = WinNowing::PickFingerPrints(this->m_vecSplitedHits);
    /*输出选出的指纹信息
    std::wcout<<"**************************"<<this->m_FingerPrints.size()<<std::endl;
    for(int i=0;i<this->m_FingerPrints.size();i++)
    {
        KGRAMHASH kgramHash = this->m_FingerPrints[i];
        std::wcout<<kgramHash.vec_splitedHits[0].offset<<":::"<<kgramHash.hashValue<<std::endl;
    }*/
}

Document::~Document()
{
    //dtor
}
