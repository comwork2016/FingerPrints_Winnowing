#include "Document.h"

//文档构造函数
Document::Document(const std::string& str_DocPath)
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
    int offset=0;
    int n_ParaIndex = 0;
    while(!ifs_Doc.eof())
    {
        Paragraph para;
        para.index = n_ParaIndex;
        para.offset_begin = offset;
        char line[LINE_LENGTH];
        ifs_Doc.getline(line,LINE_LENGTH);
        int n_LineLength = strlen(line);
        offset+=n_LineLength;
        this->m_strContents.append(line,strlen(line));
        para.offset_end = offset;
        // 不是文章结尾时将原文档的换行符加回去
        if(!ifs_Doc.eof())
        {
            this->m_strContents.append(1,'\n');
            offset++;
        }
        if(n_LineLength != 0) //空白行不计为段落
        {
            this->m_vecParagraph.push_back(para);
            n_ParaIndex++;
        }
    }
    ifs_Doc.close();
    return OK_READFILE;
}

//计算段落和全文的simhash
void Document::CalcParaAndDocSimHash()
{
    SplitContents* splitContents = new SplitContents();
    for(int i=0;i<this->m_vecParagraph.size();i++)
    {
        Paragraph& para = this->m_vecParagraph[i];
        std::string str_para = this->m_strContents.substr(para.offset_begin,para.offset_end-para.offset_begin);
        para.vec_splitedHits = splitContents->SplitContentsToWords(str_para);
        para.hashValue = HashUtil::CalcParaSimHash(para.vec_splitedHits);
    }
    this->m_lSimHash = HashUtil::CalcDocSimHash(this->m_vecParagraph);
}

void Document::PickParaFingerPrints()
{
    for(int i=0;i<this->m_vecParagraph.size();i++)
    {
        Paragraph& para = this->m_vecParagraph[i];
        para.vec_ParaFingerPrints = WinNowing::PickFingerPrints(para.vec_splitedHits);
    }
}

void Document::Dispaly()
{
     /*输出段落信息*/
    for(int i=0;i<this->m_vecParagraph.size();i++)
    {
        Paragraph para = this->m_vecParagraph[i];
        std::string str_para = this->m_strContents.substr(para.offset_begin,para.offset_end-para.offset_begin);
        //段落号，开始偏移值，结束偏移值，simhash值
        std::wcout<<L"["<<para.index<<":"<<para.offset_begin<<","<<para.offset_end<<","<<para.hashValue<<"]"<<std::endl;
        // 段落文本
        // std::wcout<<StringUtil::ConvertCharArraytoWString(str_para)<<std::endl;
        //段落分词
        for(int j=0;j<para.vec_splitedHits.size();j++)
        {
            std::wcout<<para.vec_splitedHits[j].words<<" ";
        }
        std::wcout<<std::endl;
        //段落指纹信息
        for(int j=0;j<para.vec_ParaFingerPrints.size();j++)
        {
            KGramHash finger = para.vec_ParaFingerPrints[j];
            std::wcout<<"["<<finger.offset_begin<<","<<finger.offset_end<<","<<finger.hashValue<<"]" ;
        }
        std::wcout<<std::endl<<std::endl<<std::endl;
    }
}

Document::~Document()
{
    //dtor
}
