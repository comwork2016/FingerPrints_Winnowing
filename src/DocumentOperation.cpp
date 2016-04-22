#include "DocumentOperation.h"

DocumentOperation::DocumentOperation()
{
    //ctor
}

//向数据库中添加一个文档信息
int DocumentOperation::AddDocument(const std::string& str_DocPath)
{
    //通过文件路径读取文件内容，并进行分词处理，计算simhash值。
    Document* doc = new Document(str_DocPath);
    doc->CalcParaAndDocSimHash();
    DocumentDao* docDao = new DocumentDao();
    //与数据库中的文件SimHash比较,如果不相同,计算文档指纹并存入数据库中
    const std::string str_SimilarDoc = docDao->QuerySIMSimilarity(doc);
    if(str_SimilarDoc=="")
    {
        //挑选指纹信息并存入
        doc->PickParaFingerPrints();

        doc->Dispaly();

        docDao->Insert(doc);
        const char* pch_DocName = doc->GetstrDocName().c_str();
        std::wcout<<StringUtil::ConvertCharArraytoWString(pch_DocName) <<L" inserted"<<std::endl;
    }
    else
    {
        const char* pch_DocName = doc->GetstrDocName().c_str();
        const char* pch_SimDocName = str_SimilarDoc.c_str();
        std::wcout<<L"xxxxxx "<<StringUtil::ConvertCharArraytoWString(pch_DocName) <<L" is similar to "<<StringUtil::ConvertCharArraytoWString(pch_SimDocName)<<std::endl;
    }
    return 0;
}

//将整个目录中的文件添加到数据库中
int DocumentOperation::AddDirectoryDocuments(const std::string& str_InputDir)
{
    /*
        DocumentDao* daoDelete = new DocumentDao();
        daoDelete->DeleteAll();

        //读取目录下所有的文件
        struct dirent *ptr;
        DIR *dir;
        dir=opendir((char *)str_InputDir.c_str());
        if(!dir)
        {
            std::wcout<<L"read input dir error"<<std::endl;
            return 1;
        }
        while((ptr=readdir(dir))!=NULL)
        {
            //跳过'.'和'..'两个目录
            if(ptr->d_name[0] == '.' || ptr->d_name[strlen(ptr->d_name)-1] == '~')
            {
                continue;
            }
            std::string str_DocPath = str_InputDir + ptr->d_name;
            AddDocument(str_DocPath);
        }
        closedir(dir);*/
    return 0;
}

int DocumentOperation::SearchLeak(const std::string& str_DocPath)
{
    /*
       Document* doc = new Document(str_DocPath);
       doc->SplitContentsToWords();
       doc->CalcSimHash();
       DocumentDao* docDao = new DocumentDao();
       //与数据库中的文件SimHash比较,如果不相同,再通过文档指纹查询泄露信息
       std::string str_SimilarDoc = docDao->QuerySIMSimilarity(doc);
       if(str_SimilarDoc=="")
       {
           doc->PickFingerPrints();
    */
    /*输出选出的指纹信息
            for(int i=0; i<doc->GetFingerPrints().size(); i++)
            {
                KGramHash kgram_SearchDoc = doc->GetFingerPrints()[i];
                std::wcout<<L"["<<kgram_SearchDoc.offset_begin<<","<<kgram_SearchDoc.offset_end<<L"]";
                int n_OriginLength = kgram_SearchDoc.offset_end + kgram_SearchDoc.offset_begin;
                std::string str_OriginWord = doc->GetstrContents().substr(kgram_SearchDoc.offset_begin, n_OriginLength);
                const char* pch_OriginWord = str_OriginWord.c_str();
                std::wcout<<SplitContents::ConvertCharArraytoWString(pch_OriginWord)<<std::endl;
            }
            std::wcout<<std::endl;
    *//*
        //查询相同的指纹
        std::vector<FingerPrintsSimilarDocument> vec_SimilarDocument = docDao->GetFingerPrintsSimilarDocument(doc);
        /*遍历输出相同指纹*/
    /*       for(int i=0; i<vec_SimilarDocument.size(); i++)
           {
               FingerPrintsSimilarDocument similarDoc = vec_SimilarDocument[i];
               const char* pch_DocPath = doc->GetstrDocPath().c_str();
               const char* pch_SimDocPath = similarDoc.str_DBDoc.c_str();
               std::wcout<<std::endl<<std::endl<<L"*************************************************************************************************"<<std::endl;
               std::wcout<<L"similarity between "<<SplitContents::ConvertCharArraytoWString(pch_DocPath)<<L" and "<<SplitContents::ConvertCharArraytoWString(pch_SimDocPath)<<L" is "<<100*similarDoc.f_similarity<<"%"<<std::endl;
               std::wcout<<L"*************************************************************************************************"<<std::endl;
               int n_SameSize = similarDoc.vec_SearchDocSimilarTextRange.size();
               for(int j=0; j<n_SameSize; j++)
               {
                   std::wcout<<L"==============================="<<std::endl;
                   TextRange textrange_SearchDoc = similarDoc.vec_SearchDocSimilarTextRange[j]; //待比对文档中相同的hash
                   TextRange textrange_DBDoc = similarDoc.vec_DBDocSimilarTextRange[j]; //数据库中文档的相同的hash;
                   // 搜索文档的内容和位置
                   int n_OriginLength = textrange_SearchDoc.offset_end - textrange_SearchDoc.offset_begin;
                   std::string str_OriginWord = doc->GetstrContents().substr(textrange_SearchDoc.offset_begin, n_OriginLength);
                   const char* pch_OriginWord = str_OriginWord.c_str();
                   std::wcout<<L"["<<textrange_SearchDoc.offset_begin<<","<<textrange_SearchDoc.offset_end<<L","<<n_OriginLength<<L"]"<<std::endl;
                   std::wstring wstr_SameTextSearch = SplitContents::ConvertCharArraytoWString(pch_OriginWord,n_OriginLength);
                   std::wcout<<wstr_SameTextSearch<<std::endl<<std::endl;
                   //数据库中文档的内容和位置
                   Document* docDB = new Document(similarDoc.str_DBDoc);// 数据库中的文档信息
                   int n_DBLength = textrange_DBDoc.offset_end - textrange_DBDoc.offset_begin;
                   std::string str_DBWord = docDB->GetstrContents().substr(textrange_DBDoc.offset_begin, n_DBLength);
                   const char* pch_DBWord = str_DBWord.c_str();
                   std::wcout<<L"["<<textrange_DBDoc.offset_begin<<","<<textrange_DBDoc.offset_end<<L","<<n_DBLength<<L"]"<<std::endl;
                   std::wstring wstr_SameTextDB = SplitContents::ConvertCharArraytoWString(pch_DBWord);
                   std::wcout<<wstr_SameTextDB<<std::endl;
               }
           }
       }
       else
       {
           const char* pch_DocName =doc->GetstrDocName().c_str();
           const char* pch_SimilarDocName =str_SimilarDoc.c_str();
           std::wcout<<L"xxxxxx "<<SplitContents::ConvertCharArraytoWString(pch_DocName) <<L" is similar to "<<pch_SimilarDocName<<std::endl;
       }*/
    return 0;
}

DocumentOperation::~DocumentOperation()
{
    //dtor
}
