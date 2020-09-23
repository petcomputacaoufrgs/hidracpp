#include "baseconversor.h"

BaseConversor::BaseConversor()
{
    bits = 0;
}

BaseConversor& BaseConversor::input(QString digits, int base)
{
    //TODO bits = ?

    return *this;
}

long long unsigned BaseConversor::getBits()
{
    return bits;
}

QString BaseConversor::output(int base)
{
    // TODO
}
