#include "addressingmode.h"

#include "byte.h"

const QString AddressingMode::NO_PATTERN = "";

AddressingMode::AddressingMode()
{
}

AddressingMode::AddressingMode(QString bitPattern, AddressingMode::AddressingModeCode addressingModeCode, QString assemblyPattern)
{
    this->bitPattern = bitPattern;
    this->addressingModeCode = addressingModeCode;
    this->assemblyPattern = assemblyPattern;
    this->assemblyRegExp = QRegExp(assemblyPattern);
}

QString AddressingMode::getBitPattern() const
{
    return bitPattern;
}

int AddressingMode::getBitCode() const
{
    return Conversion::stringToValue(bitPattern);
}

AddressingMode::AddressingModeCode AddressingMode::getAddressingModeCode() const
{
    return addressingModeCode;
}

QString AddressingMode::getAssemblyPattern() const
{
    return assemblyPattern;
}

QRegExp AddressingMode::getAssemblyRegExp() const
{
    return assemblyRegExp;
}
