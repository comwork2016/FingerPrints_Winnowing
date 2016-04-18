#include "SimHash.h"

SimHash::SimHash()
{
    //ctor
}

//计算文档分词后的simhash值
SIMHASH_TYPE SimHash::CalcSimHash(std::vector<SplitedHits> vec_SplitedHits)
{
    //初始化表示simhash每一位的权重数组
    short v[SIMHASHBITS];
    for(int i=0; i<SIMHASHBITS; i++)
    {
        v[i] = 0;
    }
    //遍历分词列表
    for(std::vector<SplitedHits>::iterator it=vec_SplitedHits.begin();it!=vec_SplitedHits.end();it++)
    {
        SplitedHits sh_hits = *it;
//        std::wcout<<sh_hits.words<<"["<<sh_hits.offset<<","<<sh_hits.length<<","<<sh_hits.hashValue<<"]"<<std::endl;
        SIMHASH_TYPE l_Hash = it->hashValue;
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
    /*二进制输出
    std::cout<<std::endl;
    std::bitset<64> bs(l_SimHash);
    std::cout<<bs<<std::endl;
    */
    return l_SimHash;
}

SimHash::~SimHash()
{
    //dtor
}
