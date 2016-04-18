#ifndef SIMHASH_H
#define SIMHASH_H

#include <iostream>
#include <string>
#include <string.h>
#include "Constants.h"
#include "SplitContents.h"

class SimHash
{
    public:
        SimHash();
        virtual ~SimHash();

        static SIMHASH_TYPE CalcSimHash(std::vector<SplitedHits> vec_SplitedHits);
    protected:
    private:
};

#endif // SIMHASH_H
