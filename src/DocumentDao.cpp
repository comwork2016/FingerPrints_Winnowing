#include "DocumentDao.h"

DocumentDao::DocumentDao()
{
    //ctor
    this->m_Host = "localhost:27017";
    this->m_DBName = "FP.docs";
    this->m_Conn.connect(this->m_Host);
}

//插入一个文档到数据库中
int DocumentDao::Insert(Document*& doc)
{
    mongo::BSONObjBuilder b;
    b.append("filename",doc->GetstrDocName());
    b.append("filepath", doc->GetstrDocPath());
    const char* pch_Contents= doc->GetstrContents().c_str();
    b.append("filelength",static_cast<int>(SplitContents::ConvertCharArraytoWString(pch_Contents).length()));
    b.appendNumber("simhash",static_cast<long long>(doc->GetlSimHash()));
    b.appendNumber("fingersize",doc->GetFingerPrints().size());
    std::vector<KGramHash> vec_FingerPrints = doc->GetFingerPrints();
    mongo::BSONObjBuilder bb;
    for(std::vector<KGramHash>::iterator it = vec_FingerPrints.begin(); it!=vec_FingerPrints.end(); it++)
    {
        KGramHash kgramHash = *it;
        SIMHASH_TYPE l_Hash = kgramHash.hashValue;
        std::stringstream ss;
        ss << l_Hash;
        std::string str_Hash = ss.str();
        mongo::BSONObjBuilder bb_offset;
        bb_offset.append("begin",kgramHash.offset_begin);
        bb_offset.append("end",kgramHash.offset_end);
        bb.append(str_Hash,bb_offset.obj());
    }
    b.append("fingerprints",bb.obj());
    this->m_Conn.insert(this->m_DBName,b.obj());
    return 0;
}

//插入一个文档到数据库中
int DocumentDao::DeleteAll()
{
    this->m_Conn.dropCollection(this->m_DBName);
    return 0;
}

//计算海明距离
bool IsSimHashSimilar(const SIMHASH_TYPE& l_num1, const SIMHASH_TYPE& l_num2)
{
    int hd = 0;
    SIMHASH_TYPE x = l_num1^l_num2;
    while (x && hd<=HAMMINGDIST)
    {
        hd += 1;
        x = x&(x-1);//减一之后二进制的数字里面会减少一个1
    }
    if(hd<=HAMMINGDIST)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//从数据库中查询simhash值相似的文件名称，没有则返回""
std::string DocumentDao::QuerySIMSimilarity(Document*& doc)
{
    mongo::BSONObj bo_columns = BSON("simhash"<<1<<"filelength"<<1<<"filename"<<1);
    mongo::auto_ptr<mongo::DBClientCursor> cursor = this->m_Conn.query(this->m_DBName,mongo::Query(),0,0,&bo_columns);
    while (cursor->more())
    {
        mongo::BSONObj p = cursor->next();
        SIMHASH_TYPE l_SimHash = p.getField("simhash").numberLong();
        if(IsSimHashSimilar(doc->GetlSimHash(),l_SimHash))
        {
            //两个近似网页的文章长度差距应在20%以内
            int n_DBFileLength = p.getIntField("filelength");
            const char* pch_Contents= doc->GetstrContents().c_str();
            int n_DocFileLength = SplitContents::ConvertCharArraytoWString(pch_Contents).length();
            float f_LengthSim = (float)(n_DBFileLength-n_DocFileLength)/(n_DocFileLength>n_DBFileLength?n_DocFileLength:n_DBFileLength);
            if( f_LengthSim >= 0.2)
            {
                return "";
            }
            std::string str_SimilarDoc = std::string(p.getStringField("filename"));
            return str_SimilarDoc;
        }
    }
    return "";
}

//从数据库中查询指纹相似的文档
std::vector<FingerPrintsSimilarDocument> DocumentDao::GetFingerPrintsSimilarDocument(Document*& doc)
{
    std::vector<FingerPrintsSimilarDocument> vec_SimilarDocument;
    int n_DocFingerSize = doc->GetFingerPrints().size();
    // 查询数据库
    mongo::BSONObj bo_columns = BSON("fingersize"<<1<<"fingerprints"<<1<<"filepath"<<1);
    mongo::auto_ptr<mongo::DBClientCursor> cursor = this->m_Conn.query(this->m_DBName,mongo::Query(),0,0,&bo_columns);
    while (cursor->more())
    {
        mongo::BSONObj p = cursor->next();
        int n_DBDocFingerSize = p.getIntField("fingersize");
        mongo::BSONObj bson_FingerPrints = p.getObjectField("fingerprints"); //数据库中一个文档的指纹信息
        std::vector<TextRange> vec_SearchDocSimilarTextRange;//待比对的文档中相同指纹范围
        std::vector<TextRange> vec_DBDocSimilarTextRange;//数据库的文档中相同指纹范围
        //遍历待比对的文档指纹
        std::vector<KGramHash> docFingerPrints = doc->GetFingerPrints();
        int n_SameFingerPrintsCount = 0;
        for(std::vector<KGramHash>::iterator it = docFingerPrints.begin(); it!= docFingerPrints.end(); it++)
        {
            KGramHash kgramHash_SearchDoc = *it;
            std::stringstream ss;
            ss << kgramHash_SearchDoc.hashValue;
            mongo::BSONObj bson_fingerPos = bson_FingerPrints.getObjectField(ss.str().c_str());
            //数据库文档中存在该指纹
            if(!bson_fingerPos.isEmpty())
            {
                n_SameFingerPrintsCount++;
                TextRange textrange_SearchDoc;
                TextRange textrange_DBDoc;
                textrange_SearchDoc.offset_begin = kgramHash_SearchDoc.offset_begin;
                textrange_SearchDoc.offset_end = kgramHash_SearchDoc.offset_end;
                textrange_DBDoc.offset_begin = bson_fingerPos.getIntField("begin");
                textrange_DBDoc.offset_end = bson_fingerPos.getIntField("end");
                //在存入到数据库之前，检查两个相邻指纹之间是否有重复的内容存在。
                if(!vec_SearchDocSimilarTextRange.empty() && !vec_DBDocSimilarTextRange.empty())
                {
                    TextRange textrange_SearchDocLast = vec_SearchDocSimilarTextRange.back(); // 上一个相同文本的范围
                    TextRange textrange_DBDocLast = vec_DBDocSimilarTextRange.back(); // 上一个相同文本的范围
                    //两个相同文本范围有重复，则合并处理
                    if(kgramHash_SearchDoc.offset_begin <= textrange_SearchDocLast.offset_end && textrange_DBDoc.offset_begin <= textrange_DBDocLast.offset_end)
                    {
                        //修改相同文本范围
                        textrange_SearchDoc.offset_begin = textrange_SearchDocLast.offset_begin;
                        textrange_SearchDoc.offset_end = kgramHash_SearchDoc.offset_end;
                        //同时删除向量中的上一条记录。
                        vec_SearchDocSimilarTextRange.pop_back();
                        //修改相同文本范围
                        textrange_DBDoc.offset_begin = textrange_DBDocLast.offset_begin;
                        //同时删除向量中的上一条记录。
                        vec_DBDocSimilarTextRange.pop_back();
                    }
                }
                vec_SearchDocSimilarTextRange.push_back(textrange_SearchDoc);
                vec_DBDocSimilarTextRange.push_back(textrange_DBDoc);
            }
        }
        //遍历完成之后计算相似度
        int n_MaxFingerSize = n_DBDocFingerSize > doc->GetFingerPrints().size()? n_DBDocFingerSize : doc->GetFingerPrints().size();
        float f_similarity = float(n_SameFingerPrintsCount)/n_MaxFingerSize;
        if(f_similarity!=0)
        {
            /*遍历重复内容的范围值
            std::wcout<<n_SameFingerPrintsCount<<"-----"<<vec_SearchDocSimilarTextRange.size()<<"-----"<<vec_DBDocSimilarTextRange.size()<<std::endl;
            for(int i = 0; i<vec_SearchDocSimilarTextRange.size(); i++)
            {
                TextRange textrange_SearchDoc = vec_SearchDocSimilarTextRange[i];
                TextRange textrange_DBDoc = vec_DBDocSimilarTextRange[i];
                std::wcout<<textrange_SearchDoc.offset_begin<<"==="<<textrange_SearchDoc.offset_end<<"          "<<textrange_DBDoc.offset_begin<<"-----"<<textrange_DBDoc.offset_end<<std::endl;
            }*/
            //相似度不为0时，保存相似文档信息
            FingerPrintsSimilarDocument similarDoc;
            similarDoc.str_SearchDoc = doc->GetstrDocPath();
            std::string str_DocPathInDB = p.getStringField("filepath");
            similarDoc.str_DBDoc = str_DocPathInDB;
            similarDoc.f_similarity = f_similarity;
            similarDoc.vec_SearchDocSimilarTextRange = vec_SearchDocSimilarTextRange;
            similarDoc.vec_DBDocSimilarTextRange = vec_DBDocSimilarTextRange;
            //将相似文档信息添加到向量中
            vec_SimilarDocument.push_back(similarDoc);
        }
    }
    return vec_SimilarDocument;
}

DocumentDao::~DocumentDao()
{
    //dtor
}
