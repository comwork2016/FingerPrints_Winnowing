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
        Document(const std::string& str_DocPath);
        virtual ~Document();
        std::string GetstrDocPath() const { return m_strDocPath; }
        std::string GetstrDocName() const { return m_strDocName; }
        std::string GetstrContents() const { return m_strContents; }
        std::vector<SplitedHits> GetvecSplitedHits() const { return m_vecSplitedHits; }
        SIMHASH_TYPE GetlSimHash() const { return m_lSimHash; }
        std::vector<KGramHash> GetFingerPrints() const { return m_FingerPrints; }
        void PickFingerPrints();
        void SplitContentsToWords();
        void CalcSimHash();
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
