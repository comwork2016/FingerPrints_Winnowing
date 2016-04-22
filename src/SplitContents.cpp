#include "SplitContents.h"

SplitContents::SplitContents()
{
}

//将字符串转换为wstring类型
std::wstring SplitContents::ConvertCharArraytoWString(const char* pch, int length)
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

/*
    str_Source: string to be splited
    str_Delims: split separator
    return splited vector
*/
std::vector<std::wstring> splitString(const std::wstring wstr_Source, const std::wstring wstr_Delims)
{
    std::wstring delims = wstr_Delims;
    std::wstring wstr;
    std::vector<std::wstring> vec_wstrSplited;
    if(delims.empty())
    {
        delims = L"/n/r";
    }
    std::wstring::size_type pos=0, length = wstr_Source.size();
    while(pos < length )
    {
        wstr=L"";
        //pos指到第一个不是delims中的字符的位置
        while( (delims.find(wstr_Source[pos]) != std::wstring::npos) && (pos < length) )
        {
            ++pos;
        }
        if(pos==length)
        {
            return vec_wstrSplited;
        }
        //pos指到下一个是delims中的字符的位置
        while( (delims.find(wstr_Source[pos]) == std::wstring::npos) && (pos < length) )
        {
            wstr += wstr_Source[pos++];
        }
        if(!wstr.empty())
        {
            vec_wstrSplited.push_back(wstr);
        }
    }
    return vec_wstrSplited;
}

//计算文本的hash值
SIMHASH_TYPE CalcWstringHash(const std::wstring& str)
{
    SIMHASH_TYPE l_Hash = 0;
    //判断是否有近义词
    std::wstring wstr_Delims = L"|";
    if(str.find(wstr_Delims)!=std::wstring::npos) // 没有近义词
    {
        std::vector<std::wstring> vec_wstrSplited = splitString(str,wstr_Delims);
        for(int i=0;i<vec_wstrSplited.size();i++)
        {
            std::wstring wstri = vec_wstrSplited[i];
            SIMHASH_TYPE l_HashI = CalcWstringHash(wstri);
            l_Hash += l_HashI;
        }
    }
    else
    {
        for(int i=str.length()-1; i>=0; i--)
        {
            wchar_t c = str.at(i);
            l_Hash = ( BASE * l_Hash +  int(c)*BASE) % MODNUM;
        }
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
