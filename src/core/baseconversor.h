#ifndef BASECONVERSOR_H
#define BASECONVERSOR_H

#include <QString>

class BaseConversor
{
public:
    BaseConversor();
    BaseConversor& input(QString digits, int base);
    long long unsigned getBits();
    QString output(int base);

private:
    long long unsigned bits;
};

#endif // BASECONVERSOR_H
