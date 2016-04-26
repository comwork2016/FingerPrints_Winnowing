#ifndef HASHUTIL_H
#define HASHUTIL_H

#include <string>
#include <vector>
#include "Constants.h"
#include "StringUtil.h"
#include "DataStrcture.h"

class HashUtil
{
    public:
        HashUtil();
        virtual ~HashUtil();
        static bool IsSimHashSimilar(const SIMHASH_TYPE& l_num1, const SIMHASH_TYPE& l_num2);
        static SIMHASH_TYPE CalcWstringHash(const std::wstring& str);
        static SIMHASH_TYPE CalcParaSimHash(const std::vector<SplitedHits>& vec_SimHash);
        static SIMHASH_TYPE CalcDocSimHash(const std::vector<Paragraph>& vec_SimHash);
    protected:
        template <typename T>
        static SIMHASH_TYPE CalcSimHash(const std::vector<T>& vec_SimHash);
    private:
};

#endif // HASHUTIL_H
