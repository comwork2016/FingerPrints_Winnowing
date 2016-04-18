#include "RabinHash.h"

RabinHash::RabinHash()
{
    //ctor
}

RabinHash::CalcRabinHash(string str_Content)
{

    int n_mul=1; //the value of n_mul would be "pow(BASE, K) % MODNUM"
    for(int i = 0; i < KGRAM; i++)
    {
        n_mul = ( n_mul * BASE ) % MODNUM;
    }
    int n_kcount = 0;
    char* pch_NowStr; //当前要计算KRHash值的字符串
    char* pch_LastStr; //保存上一个字符串
    pch_LastStr = (char*)malloc(sizeof(char)*(KGRAM+1));
    pch_NowStr = (char*)malloc(sizeof(char)*(KGRAM+1));
    int n_HashNow=0; //当前字符串的哈希值
    long l_SimHash = 0;
    int position = 0;//保存hash值的位置
    while(!ifs_Doc.eof())
    {
        char c;
        c=ifs_Doc.get();
        if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c>='0' && c<='9'))
        {
            if(c>='A'&&c<='Z')
            {
                c=c+32;
            }
            //H(c 1 . . . ck ) = c1 ∗ b^k−1 + c2 ∗ b^k−2 ∗ . . . + ck−1 ∗ b + ck
            if(n_kcount<KGRAM)
            {
                pch_NowStr[n_kcount] = c;
                pch_LastStr[n_kcount] = c;
                n_HashNow = ( BASE * n_HashNow + c*BASE) % MODNUM;
                n_kcount++;
            }
            else
            {
                pch_NowStr[KGRAM] = '\0';
                pch_LastStr[KGRAM] = '\0';
                // save k-gram string and hash value
                map<string,int> PosHashMap;
                PosHashMap["pos"]=position;
                PosHashMap["hash"]=n_HashNow;
                this->m_HashPosList.push_back(PosHashMap);
                // update kstr_now, kstr_now and hv_now
                strncpy(pch_NowStr,pch_LastStr+1,KGRAM-1);
                pch_NowStr[KGRAM-1] = c;
                pch_NowStr[KGRAM] = '\0';
                n_HashNow = ((n_HashNow - pch_NowStr[0]*n_mul + pch_NowStr[KGRAM-1])*BASE)%MODNUM;
                if(n_HashNow<0)
                {
                    n_HashNow += MODNUM;
                }
                strcpy(pch_LastStr,pch_NowStr);
            }
            position++;
        }
    }
    map<string,int> PosHashMap;
    PosHashMap["pos"]=position;
    PosHashMap["hash"]=n_HashNow;
    this->m_HashPosList.push_back(PosHashMap);
}

RabinHash::~RabinHash()
{
    //dtor
}
