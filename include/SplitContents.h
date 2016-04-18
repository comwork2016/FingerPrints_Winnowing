#ifndef SPLITCONTENTS_H
#define SPLITCONTENTS_H

#include <iostream>
#include <vector>
#include <string.h>

#include "DataStrcture.h"

extern "C"
{
    #include "friso/friso.h"
    #include "friso/friso_API.h"
}

class SplitContents
{
    public:
        SplitContents();
        virtual ~SplitContents();
        static std::wstring ConvertCharArraytoWString(const char* pch);
        static std::vector<SplitedHits> SplitContentsToVector(std::string str_contents);

    protected:
    private:
};

#endif // SPLITCONTENTS_H
