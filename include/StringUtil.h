#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>

class StringUtil
{
    public:
        StringUtil();
        virtual ~StringUtil();
        static std::wstring ConvertCharArraytoWString(const std::string& str);
        static std::vector<std::wstring> SplitString(const std::wstring wstr_Source, const std::wstring wstr_Delims);

    protected:
    private:
};

#endif // STRINGUTIL_H
