#ifndef SPLITCONTENTS_H
#define SPLITCONTENTS_H

#include <iostream>
#include <vector>
#include <string.h>

#include "DataStrcture.h"
#include "StringUtil.h"
#include "HashUtil.h"

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
        std::vector<SplitedHits> SplitContentsToWords(const std::string& str_contents);

    protected:
    private:
        //friso中文分词配置文件地址
        friso_t friso; // friso实例
        friso_config_t config; //friso配置类
        friso_task_t task; //分词任务类
};

#endif // SPLITCONTENTS_H
