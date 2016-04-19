#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "SplitContents.h"
#include "SimHash.h"
#include "WinNowing.h"

#include "Constants.h"

// to delete
#include <string.h>

class Document
{
    public:
        Document(std::string str_DocPath);
        virtual ~Document();
        std::string GetstrDocPath() { return m_strDocPath; }
        std::string GetstrDocName() { return m_strDocName; }
        std::string GetstrContents() { return m_strContents; }
        std::vector<SplitedHits> GetvecSplitedHits() { return m_vecSplitedHits; }
        SIMHASH_TYPE GetlSimHash() { return m_lSimHash; }
        std::vector<KGramHash> GetFingerPrints() { return m_FingerPrints; }
        void PickFingerPrints();
    protected:
        int ReadDocument();

    private:
        std::string m_strDocPath;
        std::string m_strDocName;
        std::string m_strContents;
        std::vector<SplitedHits> m_vecSplitedHits;
        SIMHASH_TYPE m_lSimHash;
        std::vector<KGramHash> m_FingerPrints;
};

#endif // DOCUMENT_H
