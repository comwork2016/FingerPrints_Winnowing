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
    std::stringstream ss;
    ss<<ifs_Doc.rdbuf();
    this->m_strContents = ss.str();
    ifs_Doc.close();
    return OK_READFILE;
}
void Document::SplitContentsToWords()
{
    //对文档内容进行分词处理
    this->m_vecSplitedHits = SplitContents::SplitContentsToWords(this->m_strContents);
    /*  遍历输出分词结果
        std::wcout<<this->m_vecSplitedHits.size()<<std::endl;
        for(std::vector<SplitedHits>::iterator it=this->m_vecSplitedHits.begin();it!=this->m_vecSplitedHits.end();it++)
        {
            SplitedHits sh_hits = *it;
            std::wcout<<sh_hits.words<<"["<<sh_hits.offset<<","<<sh_hits.length<<","<<sh_hits.hashValue<<"]"<<std::endl;
        }
    */
    /*输出分词结果到文件中
    std::wofstream wofs_Doc;
    std::string out_Doc = "./tmp/分词_"+this->m_strDocName;
    wofs_Doc.open(out_Doc.c_str(),std::ios::out);
    for(std::vector<SplitedHits>::iterator it=this->m_vecSplitedHits.begin();it!=this->m_vecSplitedHits.end();it++)
        {
            SplitedHits sh_hits = *it;
            wofs_Doc<<sh_hits.words<<"["<<sh_hits.offset<<":::"<<sh_hits.length<<":::"<<sh_hits.hashValue<<"]"<<std::endl;

        }
    wofs_Doc.close();
    */
}

void Document::CalcSimHash()
{
    //利用分词结果 计算文档的SimHash值
    this->m_lSimHash = SimHash::CalcSimHash(this->m_vecSplitedHits);
    /*输出文档的simhash值
        std::cout<<this->m_strDocName<<"---"<<this->m_lSimHash<<std::endl;
    */
}

//挑选文件指纹
void Document::PickFingerPrints()
{
    this->m_FingerPrints = WinNowing::PickFingerPrints(this->m_vecSplitedHits);
    /*输出选出的指纹信息
    for(int i=0;i<this->m_FingerPrints.size();i++)
    {
        KGramHash kgram_SearchDoc = this->m_FingerPrints[i];
                SplitedHits hits_SearchDocFirst = this->m_vecSplitedHits[kgram_SearchDoc.n_splitedHitsIndex];
                SplitedHits hits_SearchDocLast = this->m_vecSplitedHits[kgram_SearchDoc.n_splitedHitsIndex + KGRAM-1];
                std::wcout<<L"["<<hits_SearchDocFirst.offset<<","<<hits_SearchDocLast.offset + hits_SearchDocLast.length<<L"]";
                int n_OriginLength = hits_SearchDocLast.offset + hits_SearchDocLast.length - hits_SearchDocFirst.offset;
                const char* pch_OriginWord = this->m_strContents.substr(hits_SearchDocFirst.offset, n_OriginLength).c_str();
                std::wcout<<SplitContents::ConvertCharArraytoWString(pch_OriginWord)<<std::endl;
    }
    std::wcout<<std::endl<<std::endl<<std::endl;*/
    /*输出文件指纹结果到文件中
    std::wofstream wofs_Doc;
    std::string out_Doc = "./tmp/指纹_"+this->m_strDocName;
    wofs_Doc.open(out_Doc.c_str(),std::ios::out);

    for(int i=0; i<this->m_FingerPrints.size(); i++)
    {
        KGramHash kgram_SearchDoc = this->m_FingerPrints[i];
        SplitedHits hits_SearchDocFirst = this->m_vecSplitedHits[kgram_SearchDoc.n_splitedHitsIndex];
        SplitedHits hits_SearchDocLast = this->m_vecSplitedHits[kgram_SearchDoc.n_splitedHitsIndex + KGRAM-1];
        wofs_Doc<<L"["<<kgram_SearchDoc.hashValue<<":"<<hits_SearchDocFirst.offset<<":"<<hits_SearchDocLast.offset + hits_SearchDocLast.length - hits_SearchDocFirst.offset<<L"]";
        for(int k=kgram_SearchDoc.n_splitedHitsIndex; k<kgram_SearchDoc.n_splitedHitsIndex+KGRAM; k++)
        {
            SplitedHits hits_SearchDoc = this->m_vecSplitedHits[k]; //待比对文档中的分词信息
            wofs_Doc<<hits_SearchDoc.words<<L" ";
        }
    wofs_Doc<<std::endl;
    }
    wofs_Doc.close();
    */
}

Document::~Document()
{
    //dtor
}
