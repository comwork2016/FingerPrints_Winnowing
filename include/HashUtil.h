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
        static SIMHASH_TYPE CalcWstringHash(const std::wstring& str);
        static SIMHASH_TYPE CalcParaSimHash(const std::vector<SplitedHits>& vec_SimHash);
        static SIMHASH_TYPE CalcDocSimHash(const std::vector<Paragraph>& vec_SimHash);
    protected:
    private:
};

#endif // HASHUTIL_H
