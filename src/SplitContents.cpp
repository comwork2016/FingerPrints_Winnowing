#include "SplitContents.h"

SplitContents::SplitContents()
{
}

//将字符串转换为wstring类型
std::wstring SplitContents::ConvertCharArraytoWString(const char*& pch, int length)
{
    if(length==0)
    {
      length = strlen(pch);
    }
    wchar_t* wc = new wchar_t[length+1];
    mbstowcs(wc, pch, length+1);
    std::wstring wstr = wc;
    return wstr;
}

//计算文本的hash值
SIMHASH_TYPE CalcWstringHash(std::wstring str)
{
    SIMHASH_TYPE l_Hash = 0;
    for(int i=str.length()-1; i>=0; i--)
    {
        wchar_t c = str.at(i);
        l_Hash = ( BASE * l_Hash +  int(c)*BASE) % MODNUM;
    }
    if(l_Hash>MODNUM)
    {
        std::wcout<<str<<L" over flow"<<std::endl;
    }
    return l_Hash;
}

//将文本分词，存入列表中返回
std::vector<SplitedHits> SplitContents::SplitContentsToWords(const std::string& str_contents)
{
    std::vector<SplitedHits> vec_SplitedHits;
    int i;
    friso_t friso; // friso实例
    friso_config_t config; //friso配置类
    friso_task_t task; //分词任务类

    //friso中文分词配置文件地址
    static fstring frisoIniPath = fstring("/home/andin/soft/friso-1.6.0/friso.ini");

    //friso初始化
    friso = friso_new();
    config = friso_new_config();
    if ( friso_init_from_ifile(friso, config, frisoIniPath) != 1 )//初始化失败
    {
        std::cout<<"fail to initialize friso and config."<<std::endl;
        friso_free_config(config);
        friso_free(friso);
        return vec_SplitedHits;
    }
    // 设置任务信息
    task = friso_new_task();
    fstring text = fstring(str_contents.c_str());
    friso_set_text( task, text);
    //循环读取分词内容
    while ( ( friso_next( friso, config, task ) ) != NULL )
    {
        const char* pch_HitsWord = task->hits->word;
        std::wstring wstr_words = ConvertCharArraytoWString(pch_HitsWord);
        int n_offset = task->hits->offset;
        int n_length = task->hits->length;
        //计算分词片段的hash值
        SIMHASH_TYPE hashValue = CalcWstringHash(wstr_words);
        SplitedHits sh_hits =
        {
            wstr_words,
            n_offset,
            n_length,
            hashValue
        };
        vec_SplitedHits.push_back(sh_hits);

//        std::wcout<<sh_hits.words<<"["<<sh_hits.offset<<","<<sh_hits.length<<","<<sh_hits.hashValue<<"]"<<std::endl;

    }
    //任务结束，销毁friso实例
    friso_free_task( task );
    friso_free_config(config);
    friso_free(friso);
    return vec_SplitedHits;
}

SplitContents::~SplitContents()
{
    //dtor
}
