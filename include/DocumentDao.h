#ifndef DOCUMENTDAO_H
#define DOCUMENTDAO_H

#include "mongo/client/dbclient.h"
#include "Document.h"
#include "Constants.h"

class DocumentDao
{
    public:
        DocumentDao();
        virtual ~DocumentDao();

        int Insert(const Document* doc);
        int DeleteAll();
        std::string QuerySIMSimilarity(const Document* doc);
        std::vector<FingerPrintsSimilarDocument> GetFingerPrintsSimilarDocument(const Document* doc);
    protected:
    private:
        mongo::DBClientConnection m_Conn;
        std::string m_Host;
        std::string m_DBName;
};

#endif //DOCUMENTDAO_H
