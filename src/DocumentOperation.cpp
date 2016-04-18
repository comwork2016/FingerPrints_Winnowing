#include "DocumentOperation.h"

DocumentOperation::DocumentOperation()
{
    //ctor
}

//向数据库中添加一个文档信息
int DocumentOperation::AddDocument(std::string str_DocPath)
{
    Document* doc = new Document(str_DocPath);
    //通过文件路径读取文件内容，并进行分词处理。
    DocumentDao* docDao = new DocumentDao();
    //与数据库中的文件SimHash比较,如果不相同,计算文档指纹并存入数据库中
    std::string str_SimilarDoc = docDao->QuerySIMSimilarity(doc);
    if(str_SimilarDoc=="")
    {
        //挑选指纹信息并存入
        doc->PickFingerPrints();
        docDao->Insert(doc);
        std::cout<<doc->GetstrDocName() <<" inserted"<<std::endl;
    }
    else
    {
        std::cout<<"xxxxxx "<<doc->GetstrDocName() <<" is similar to "<<str_SimilarDoc<<std::endl;
    }
    return 0;
}

//将整个目录中的文件添加到数据库中
int DocumentOperation::AddDirectoryDocuments(std::string str_InputDir)
{
    DocumentDao* daoDelete = new DocumentDao();
    daoDelete->DeleteAll();

    //读取目录下所有的文件
    struct dirent *ptr;
    DIR *dir;
    dir=opendir((char *)str_InputDir.c_str());
    if(!dir)
    {
        std::cout<<"read input dir error"<<std::endl;
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
    closedir(dir);
    return 0;
}

int DocumentOperation::SearchLeak(std::string str_DocPath)
{
    Document* doc = new Document(str_DocPath);
    DocumentDao* docDao = new DocumentDao();
    //与数据库中的文件SimHash比较,如果不相同,再通过文档指纹查询泄露信息
    std::string str_SimilarDoc = docDao->QuerySIMSimilarity(doc);
    if(str_SimilarDoc=="")
    {
        doc->PickFingerPrints();
        //查询相同的指纹
        std::vector<FingerPrintsSimilarDocument> vec_SimilarDocument = docDao->GetFingerPrintsSimilarDocument(doc);
        std::wcout<<L"*********************************************"<<vec_SimilarDocument.size()<<std::endl;
        /*遍历输出相同指纹*/
        for(int i=0; i<vec_SimilarDocument.size(); i++)
        {
            FingerPrintsSimilarDocument similarDoc = vec_SimilarDocument[i];
            std::wcout<<L"similarity between "<<SplitContents::ConvertCharArraytoWString(doc->GetstrDocPath().c_str())<<L" and "<<SplitContents::ConvertCharArraytoWString(similarDoc.str_DBDoc.c_str())<<L" is "<<similarDoc.f_similarity<<std::endl;
            int n_SameSize = similarDoc.vec_SearchDocSimilarKGramHash.size();
            for(int j=0; j<n_SameSize; j++)
            {
                KGramHash kgram_SearchDoc = similarDoc.vec_SearchDocSimilarKGramHash[j]; //待比对文档中相同的hash
                KGramHash kgram_DBDoc = similarDoc.vec_DBDocSimilarKGramHash[j]; //数据库中文档的相同的hash;
                Document* docDB = new Document(similarDoc.str_DBDoc);// 数据库中的文档信息
                /*输出分词之后的文档内容
                                for(int k=kgram_SearchDoc.n_splitedHitsIndex; k<kgram_SearchDoc.n_splitedHitsIndex+KGRAM; k++)
                                {
                                    SplitedHits hits_SearchDoc = doc->GetvecSplitedHits()[k]; //待比对文档中的分词信息
                                    std::wcout<<hits_SearchDoc.words<<L" ";

                                }
                */
                SplitedHits hits_SearchDocFirst = doc->GetvecSplitedHits()[kgram_SearchDoc.n_splitedHitsIndex];
                SplitedHits hits_SearchDocLast = doc->GetvecSplitedHits()[kgram_SearchDoc.n_splitedHitsIndex + KGRAM-1];
                std::wcout<<L"["<<hits_SearchDocFirst.offset<<","<<hits_SearchDocLast.offset + hits_SearchDocLast.length<<L"]";
                int n_OriginLength = hits_SearchDocLast.offset + hits_SearchDocLast.length - hits_SearchDocFirst.offset;
                const char* pch_OriginWord = doc->GetstrContents().substr(hits_SearchDocFirst.offset, n_OriginLength).c_str();
                std::wcout<<SplitContents::ConvertCharArraytoWString(pch_OriginWord);
                std::wcout<<L"------------";
                /*输出分词之后的文档内容
                               for(int k=kgram_DBDoc.n_splitedHitsIndex; k<kgram_DBDoc.n_splitedHitsIndex+KGRAM; k++)
                               {
                                   SplitedHits hits_DBDoc = docDB->GetvecSplitedHits()[k]; //数据库中文档中的分词信息
                                   std::wcout<<hits_DBDoc.words<<L" ";
                               }
                 */
                SplitedHits hits_DBDocFirst = docDB->GetvecSplitedHits()[kgram_DBDoc.n_splitedHitsIndex];
                SplitedHits hits_DBDocLast = docDB->GetvecSplitedHits()[kgram_DBDoc.n_splitedHitsIndex + KGRAM-1];
                std::wcout<<L"["<<hits_DBDocFirst.offset<<","<<hits_DBDocLast.offset+hits_DBDocLast.length<<L"]";
                int n_DBLength = hits_DBDocLast.offset + hits_DBDocLast.length - hits_DBDocFirst.offset;
                const char* pch_DBWord = docDB->GetstrContents().substr(hits_DBDocFirst.offset, n_DBLength).c_str();
                std::wcout<<SplitContents::ConvertCharArraytoWString(pch_DBWord);
                std::wcout<<std::endl;
            }
        }
    }
    else
    {
        std::cout<<"xxxxxx "<<doc->GetstrDocName() <<" is similar to "<<str_SimilarDoc<<std::endl;
    }
    return 0;
}

DocumentOperation::~DocumentOperation()
{
    //dtor
}
