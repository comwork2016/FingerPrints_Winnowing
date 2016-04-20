#include "WinNowing.h"


WinNowing::WinNowing()
{
    //ctor
}

std::vector<KGramHash> CalcRabinHash(const std::vector<SplitedHits>& vec_SplitedHits)
{
    std::vector<KGramHash> vec_KGramHash;
    //初始化kgramhash
    int n_kcount = 0;
    KGramHash kgram_Now;
    kgram_Now.hashValue = 0;
    kgram_Now.vec_splitedHits.clear();
    KGramHash kgram_Last;
    kgram_Last.hashValue = 0;
    kgram_Last.vec_splitedHits.clear();
    SIMHASH_TYPE l_SimHash = 0;
    //遍历分词列表
    int i=0;
    for(i=0; i<vec_SplitedHits.size(); i++)
    {
        SplitedHits hits = vec_SplitedHits[i];//词语单位
        //H(c 1 . . . ck ) = c1 ∗ b^k + c2 ∗ b^k−1 ∗ . . . + ck−1 ∗ b^2 + ck*b
        if(n_kcount<KGRAM)
        {
            kgram_Now.vec_splitedHits.push_back(hits);
            kgram_Last.vec_splitedHits.push_back(hits);
            /*
            (a + b)%M = (a%M + b%M)%M
            (ab)%M = [(a%M)(b%M)]%M
            */
            kgram_Now.hashValue = ( kgram_Now.hashValue * BASE + hits.hashValue * BASE) % MODNUM;
            n_kcount++;
        }
        else
        {
            // 计算偏移信息并保存kgram的信息
            kgram_Now.offset_begin = kgram_Now.vec_splitedHits[0].offset;
            kgram_Now.offset_end = kgram_Now.vec_splitedHits[KGRAM-1].offset + kgram_Now.vec_splitedHits[KGRAM-1].length+1;
            vec_KGramHash.push_back(kgram_Now);
            // 更新kgram_now and kgram_now，即删除kgram_now中的第一个元素，添加下一个元素，同时，第一个分词索引下移
            std::vector<SplitedHits>::iterator it_first = kgram_Now.vec_splitedHits.begin();
            kgram_Now.vec_splitedHits.erase(it_first);
            kgram_Now.vec_splitedHits.push_back(hits);
            //为防止数据溢出，需要对运算进行等价处理
            SIMHASH_TYPE l_LastCharWeight=kgram_Last.vec_splitedHits[0].hashValue;
            //the value of l_LastCharWeight would be "kgram_Last.vec_splitedHits[0].hashValue * pow(BASE, K) % MODNUM"
            for(int i = 0; i < KGRAM; i++)
            {
                l_LastCharWeight = ( l_LastCharWeight * BASE ) % MODNUM;
            }
            kgram_Now.hashValue = ((kgram_Now.hashValue - l_LastCharWeight + hits.hashValue)*BASE)%MODNUM;
            if(kgram_Now.hashValue<0)
            {
                kgram_Now.hashValue+=MODNUM;
            }
            kgram_Last = kgram_Now;
        }
    }
    if(n_kcount<KGRAM)//分词个数小于K值，则作为一个KGRAM
    {
        kgram_Now.offset_begin = kgram_Now.vec_splitedHits[0].offset;
        kgram_Now.offset_end = kgram_Now.vec_splitedHits[n_kcount-1].offset + kgram_Now.vec_splitedHits[n_kcount-1].length+1;
        //SplitedHits hits  = kgram_Now.vec_splitedHits[n_kcount-1];
        //std::wcout<<hits.words<<":["<<hits.offset<<";"<<hits.length<<"]"<<std::endl;
    }
    vec_KGramHash.push_back(kgram_Now);
    return vec_KGramHash;
}

//利用WinNowing算法挑选文档指纹
std::vector<KGramHash>  WinNowing::PickFingerPrints(const std::vector<SplitedHits>& vec_SplitedHits)
{
    std::vector<KGramHash> vec_FingerPrints;
    //以词组为单位进行k-gram组合，即一个组合种有k和词
    std::vector<KGramHash> vec_KGramHash = CalcRabinHash(vec_SplitedHits);

    /*遍历k-gram词组的hash值和文本范围
    std::wcout<<vec_KGramHash.size()<<std::endl;
    for(int i=0;i<vec_KGramHash.size();i++)
    {
        for(int j=0;j<vec_KGramHash[i].vec_splitedHits.size();j++)
        {
            std::wcout<<vec_KGramHash[i].vec_splitedHits[j].words<<" ";
        }
        std::wcout<<"["<<vec_KGramHash[i].offset_begin<<","<<vec_KGramHash[i].offset_end<<"]:::"<<vec_KGramHash[i].hashValue<<std::endl;
    }*/

    KGramHash kgramhash_Window[WINDOWSIZE];//存放哈希值的窗口，窗口是循环使用的
    int n_MIN=0;     //记录当前最小值在窗口中的位置
    int n_POS=n_MIN; //记录最小值在原序列中出现的位置
    // init pn_Window array
    int i=0;
    for(i=0; i<WINDOWSIZE && i<vec_KGramHash.size(); i++)
    {
        KGramHash kgramHash = vec_KGramHash[i];
        kgramhash_Window[i] = kgramHash;
        if(kgramhash_Window[n_MIN].hashValue > kgramhash_Window[i].hashValue)
        {
            n_MIN = i;
            n_POS = n_MIN;
        }
    }
    // add first min hash to vector
    KGramHash kgramHash =  kgramhash_Window[n_MIN];
    //std::wcout<<"["<<kgramHash.offset_begin<<","<<kgramHash.offset_end<<"]:::"<<kgramHash.hashValue<<std::endl;
    vec_FingerPrints.push_back(kgramHash);

    if(i>=vec_KGramHash.size())      //当窗口值大于指纹总数时执行
    {
        //delete[] kgramhash_Window;
        return vec_FingerPrints;
    }
    int n_RightIndex=WINDOWSIZE-1;     //循环遍历数组h，按顺序将原序列的哈希值添加到数组中
    int n_HashValue = 0; // temp hash value that read from hash.txt
    i = WINDOWSIZE;
    while(i<vec_KGramHash.size())
    {
        n_RightIndex=(n_RightIndex+1)%WINDOWSIZE; // 滑动到下一个窗口
        kgramhash_Window[n_RightIndex]=vec_KGramHash[i];   //添加下一个哈希值
        if(n_MIN==n_RightIndex) //如果条件成立，说明前一个窗口的最小值将被移除窗口
        {
            n_POS+=WINDOWSIZE;
            for(int j=(n_RightIndex-1+WINDOWSIZE)%WINDOWSIZE; j!=n_RightIndex; j=(j-1+WINDOWSIZE)%WINDOWSIZE)   //向后循环查找最小值，选取窗口中最小且最右的哈希值
            {
                if(kgramhash_Window[j].hashValue<kgramhash_Window[n_MIN].hashValue)
                {
                    n_MIN=j;
                }
            }
            KGramHash kgramHash =  kgramhash_Window[n_MIN];
            //std::wcout<<"["<<kgramHash.offset_begin<<","<<kgramHash.offset_end<<"]:::"<<kgramHash.hashValue<<std::endl;
            vec_FingerPrints.push_back(kgramHash);
        }
        else //min!=r就说明当前最小的哈希值依然包含在窗口中
        {
            if(kgramhash_Window[n_RightIndex].hashValue<kgramhash_Window[n_MIN].hashValue) //那么只需要比较新添加进的哈希值即可，条件中不能含有等于号，否则不是健壮的winnowing方法。
            {
                n_POS+=(n_RightIndex+WINDOWSIZE-n_MIN)%WINDOWSIZE;
                n_MIN=n_RightIndex;
                KGramHash kgramHash =  kgramhash_Window[n_MIN];
                //std::wcout<<"["<<kgramHash.offset_begin<<","<<kgramHash.offset_end<<"]:::"<<kgramHash.hashValue<<std::endl;
                vec_FingerPrints.push_back(kgramHash);
            }
        }
        i++;
    }
    //delete[] kgramhash_Window;
    return vec_FingerPrints;
}

WinNowing::~WinNowing()
{
    //dtor
}
