#ifndef DOCUMENTOPERATION_H
#define DOCUMENTOPERATION_H

#include <string>

#include "DocumentDao.h"

#include <stdlib.h>
#include <dirent.h>

// to delete
#include "SplitContents.h"

class DocumentOperation
{
    public:
        DocumentOperation();
        virtual ~DocumentOperation();
        static int AddDocument(const std::string& str_DocPath);
        static int AddDirectoryDocuments(const std::string& str_InputDir);
        static int SearchLeak(const std::string& str_DocPath);
    protected:
    private:
};

#endif // DOCUMENTOPERATION_H
