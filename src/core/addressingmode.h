#ifndef ADDRESSINGMODE_H
#define ADDRESSINGMODE_H

#include <QString>
#include <QRegExp>

class AddressingMode
{
public:
    enum AddressingModeCode
    {
        DIRECT, INDIRECT, IMMEDIATE, INDEXED_BY_X, INDEXED_BY_PC
    };

    static const QString NO_PATTERN;

    AddressingMode();
    AddressingMode(QString bitPattern, AddressingModeCode addressingModeCode, QString assemblyPattern);

    QString getBitPattern() const;
    int getBitCode() const;
    AddressingModeCode getAddressingModeCode() const;
    QString getAssemblyPattern() const;
    QRegExp getAssemblyRegExp() const;

private:
    QString bitPattern;
    AddressingModeCode addressingModeCode;
    QString assemblyPattern;
    QRegExp assemblyRegExp;
};

#endif // ADDRESSINGMODE_H
