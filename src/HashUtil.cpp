#include "HashUtil.h"

HashUtil::HashUtil()
{
    //ctor
}


//计算海明距离
bool HashUtil::IsSimHashSimilar(const SIMHASH_TYPE& l_num1, const SIMHASH_TYPE& l_num2)
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

//计算文本的hash值
SIMHASH_TYPE HashUtil::CalcWstringHash(const std::wstring& str)
{
    SIMHASH_TYPE l_Hash = 0;
    //判断是否有近义词
    std::wstring wstr_Delims = L"|";
    if(str.find(wstr_Delims)!=std::wstring::npos) // 没有近义词
    {
        std::vector<std::wstring> vec_wstrSplited = StringUtil::SplitString(str,wstr_Delims);
        for(int i=0; i<vec_wstrSplited.size(); i++)
        {
            std::wstring wstri = vec_wstrSplited[i];
            SIMHASH_TYPE l_HashI = CalcWstringHash(wstri);
            l_Hash += l_HashI;
        }
    }
    else
    {
        for(int i=str.length()-1; i>=0; i--)
        {
            wchar_t c = str.at(i);
            l_Hash = ( BASE * l_Hash +  int(c)*BASE) % MODNUM;
        }
    }
    return l_Hash;
}

//计算文档分词后的simhash值
template <typename T>
SIMHASH_TYPE HashUtil::CalcSimHash(const std::vector<T>& vec_SimHash)
{
    //初始化表示simhash每一位的权重数组
    short v[SIMHASHBITS];
    for(int i=0; i<SIMHASHBITS; i++)
    {
        v[i] = 0;
    }
    //遍历分词列表
    for(int i=0;i<vec_SimHash.size();i++)
    {
//        std::wcout<<sh_hits.words<<"["<<sh_hits.offset<<","<<sh_hits.length<<","<<sh_hits.hashValue<<"]"<<std::endl;
        SIMHASH_TYPE l_Hash = vec_SimHash[i].hashValue;
        //计算对hash值的每一位，如果为1，则权重数组的相应位+1，为0则-1
        for (int j = 0; j < SIMHASHBITS; j++)
        {
            SIMHASH_TYPE bitmask = 1 << j; //位的掩码:向左移j位
            SIMHASH_TYPE bit = l_Hash&bitmask;
            if (bit != 0)
            {
                v[j] += 1;
            }
            else
            {
                v[j] -= 1;
            }
        }
    }
    //根据权重数组计算文档的simhash值
    SIMHASH_TYPE l_SimHash = 0;
    for (int i = 0; i < SIMHASHBITS; i++)
    {
        if (v[i] > 0)
        {
            SIMHASH_TYPE n_IBit = (SIMHASH_TYPE)1<<(SIMHASHBITS-1 - i);
            l_SimHash += n_IBit;
        }
    }
    return l_SimHash;
}

SIMHASH_TYPE HashUtil::CalcParaSimHash(const std::vector<SplitedHits>& vec_SimHash)
{
    return CalcSimHash(vec_SimHash);
}

SIMHASH_TYPE HashUtil::CalcDocSimHash(const std::vector<Paragraph>& vec_SimHash)
{
    return CalcSimHash(vec_SimHash);
}

HashUtil::~HashUtil()
{
    //dtor
}
