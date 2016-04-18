#include "DocumentDao.h"

DocumentDao::DocumentDao()
{
    //ctor
    this->m_Host = "localhost:27017";
    this->m_DBName = "FP.docs";
    this->m_Conn.connect(this->m_Host);
}

//插入一个文档到数据库中
int DocumentDao::Insert(Document* doc)
{
    mongo::BSONObjBuilder b;
    b.append("filename",doc->GetstrDocName());
    b.append("filepath", doc->GetstrDocPath());
    b.appendNumber("simhash",static_cast<long long>(doc->GetlSimHash()));
    b.appendNumber("fingersize",doc->GetFingerPrints().size());
    std::vector<KGramHash> vec_FingerPrints = doc->GetFingerPrints();
    mongo::BSONObjBuilder bb;
    for(std::vector<KGramHash>::iterator it = vec_FingerPrints.begin(); it!=vec_FingerPrints.end(); it++)
    {
        KGramHash kgramHash = *it;
        SIMHASH_TYPE l_Hash = kgramHash.hashValue;
        int n_splitedHitsIndex = kgramHash.n_splitedHitsIndex;
        std::stringstream ss;
        ss << l_Hash;
        std::string str_Hash = ss.str();
        bb.append(str_Hash,n_splitedHitsIndex);
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
bool IsSimHashSimilar(long l_num1, long l_num2)
{
    int hd = 0;
    int x = l_num1^l_num2;
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
std::string DocumentDao::QuerySIMSimilarity(Document* doc)
{
    mongo::auto_ptr<mongo::DBClientCursor> cursor = this->m_Conn.query(this->m_DBName,mongo::BSONObj());
    while (cursor->more())
    {
        mongo::BSONObj p = cursor->next();
        SIMHASH_TYPE l_SimHash = p.getField("simhash").numberLong();
        if(IsSimHashSimilar(doc->GetlSimHash(),l_SimHash))
        {
            std::string str_SimilarDoc = std::string(p.getStringField("filename"));
            return str_SimilarDoc;
        }
    }
    return "";
}

//从数据库中查询指纹相似的文档
std::vector<FingerPrintsSimilarDocument> DocumentDao::GetFingerPrintsSimilarDocument(Document* doc)
{
    std::vector<FingerPrintsSimilarDocument> vec_SimilarDocument;
    int n_DocFingerSize = doc->GetFingerPrints().size();
    // 查询数据库
    mongo::auto_ptr<mongo::DBClientCursor> cursor = this->m_Conn.query(this->m_DBName,mongo::BSONObj());
    while (cursor->more())
    {
        mongo::BSONObj p = cursor->next();
        int n_DBDocFingerSize = p.getIntField("fingersize");
        mongo::BSONObj bson_FingerPrints = p.getObjectField("fingerprints"); //数据库中一个文档的指纹信息
        std::vector<KGramHash> vec_SearchDocSimilarKGramHash;//待比对的文档中相同指纹向量
        std::vector<KGramHash> vec_DBDocSimilarKGramHash;//数据库的文档中相同指纹向量
        //遍历待比对的文档指纹
        std::vector<KGramHash> docFingerPrints = doc->GetFingerPrints();
        for(std::vector<KGramHash>::iterator it = docFingerPrints.begin(); it!= docFingerPrints.end(); it++)
        {
            KGramHash kgramHash_SearchDoc = *it;
            std::stringstream ss;
            ss << kgramHash_SearchDoc.hashValue;
            int n_KGramIndexFounded = bson_FingerPrints.getIntField(ss.str().c_str());
            //数据库文档中存在该指纹
            if(n_KGramIndexFounded != INT_MIN)
            {
                vec_SearchDocSimilarKGramHash.push_back(kgramHash_SearchDoc);
                KGramHash kgramHash_DBDoc;
                kgramHash_DBDoc.hashValue = kgramHash_SearchDoc.hashValue;
                kgramHash_DBDoc.n_splitedHitsIndex = n_KGramIndexFounded;
                vec_DBDocSimilarKGramHash.push_back(kgramHash_DBDoc);
            }
        }
        //遍历完成之后计算相似度
        int n_MaxFingerSize = n_DBDocFingerSize > doc->GetFingerPrints().size()? n_DBDocFingerSize : doc->GetFingerPrints().size();
        float f_similarity = float(vec_DBDocSimilarKGramHash.size())/n_MaxFingerSize;
        if(f_similarity!=0)
        {
            //相似度不为0时，保存相似文档信息
            FingerPrintsSimilarDocument similarDoc;
            similarDoc.str_SearchDoc = doc->GetstrDocPath();
            std::string str_DocPathInDB = p.getStringField("filepath");
            similarDoc.str_DBDoc = str_DocPathInDB;
            similarDoc.f_similarity = f_similarity;
            similarDoc.vec_SearchDocSimilarKGramHash = vec_SearchDocSimilarKGramHash;
            similarDoc.vec_DBDocSimilarKGramHash = vec_DBDocSimilarKGramHash;
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
