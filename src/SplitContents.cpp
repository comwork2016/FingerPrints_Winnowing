#include "SplitContents.h"

SplitContents::SplitContents()
{
    //friso初始化
    friso = friso_new();
    config = friso_new_config();
    static const fstring frisoIniPath = fstring("/home/andin/soft/friso-1.6.0/friso.ini");
    if ( friso_init_from_ifile(friso, config, frisoIniPath) != 1 )//初始化失败
    {
        std::cout<<"fail to initialize friso and config."<<std::endl;
    }
}

//将文本分词，存入列表中返回
std::vector<SplitedHits> SplitContents::SplitContentsToWords(const std::string& str_contents)
{
    std::vector<SplitedHits> vec_SplitedHits;
    int i;
    // 设置任务信息
    task = friso_new_task();
    fstring text = fstring(str_contents.c_str());
    friso_set_text( task, text);
    //循环读取分词内容
    while ( ( friso_next( friso, config, task ) ) != NULL )
    {
        const char* pch_HitsWord = task->hits->word;
        std::wstring wstr_words = StringUtil::ConvertCharArraytoWString(pch_HitsWord);
        int n_offset = task->hits->offset;
        int n_length = task->hits->length;
        //计算分词片段的hash值
        SIMHASH_TYPE hashValue = HashUtil::CalcWstringHash(wstr_words);
        SplitedHits sh_hits =
        {
            wstr_words,
            n_offset,
            n_length,
            hashValue
        };
        vec_SplitedHits.push_back(sh_hits);

    }
    return vec_SplitedHits;
}

SplitContents::~SplitContents()
{
    //dtor
    //任务结束，销毁friso实例
    friso_free_task( task );
    friso_free_config(config);
    friso_free(friso);
}
