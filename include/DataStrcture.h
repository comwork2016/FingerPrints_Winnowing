#ifndef DATASTRCTURE_H_INCLUDED
#define DATASTRCTURE_H_INCLUDED

#include <string>
#include <vector>
#include "Constants.h"

//定义结构体，用来存储分词之后的片段信息
struct SplitedHits
{
    std::wstring words;
    int offset;
    int length;
    SIMHASH_TYPE hashValue;
};

//定义结构体，用来存储k-gram组合信息
struct KGramHash
{
    SIMHASH_TYPE hashValue; //组合的hash值
    int n_splitedHitsIndex; //第一个分词在文档中的索引号
    std::vector<SplitedHits> vec_splitedHits; //组合包含的分词信息
};

//文档之间的相似度和相同文本
struct FingerPrintsSimilarDocument
{
    std::string str_SearchDoc;//查询的文档
    std::string str_DBDoc;//数据库中的文档
    float f_similarity; //文档相似度
    std::vector<KGramHash> vec_SearchDocSimilarKGramHash;//待比对的文档中相同指纹向量
    std::vector<KGramHash> vec_DBDocSimilarKGramHash;//数据库的文档中相同指纹向量
};

#endif // DATASTRCTURE_H_INCLUDED
