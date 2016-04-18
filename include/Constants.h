#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

typedef unsigned long long SIMHASH_TYPE;

const int ERROR_OPENFILE = -1;
const int OK_READFILE = 0;

const int HAMMINGDIST = 3;

const int SIMHASHBITS = 64;

const int KGRAM = 3;
const int BASE = 12;
const SIMHASH_TYPE MODNUM =((SIMHASH_TYPE)1 << (SIMHASHBITS-1))- 1;
const int WINDOWSIZE = 5;

#endif // CONSTANTS_H_INCLUDED
