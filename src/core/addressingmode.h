#ifndef ADDRESSINGMODE_H
#define ADDRESSINGMODE_H

#include <QString>
#include <QRegExp>

class AddressingMode
{
public:
    enum AddressingModeCode
    {
        DIRECT, INDIRECT, IMMEDIATE, INDEXED_BY_X, INDEXED_BY_PC,
        REGISTER, INDIRECT_REGISTER,
        AFTER_INCREMENTED, PRE_DECREMENTED, AFTER_INCREMENTED_INDIRECT, PRE_DECREMENTED_INDIRECT,
        INDEXED_BY_REG, INDIRECT_INDEXED_BY_REG
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
