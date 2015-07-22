#include "addressingmode.h"

const QString AddressingMode::NO_PATTERN = "";

AddressingMode::AddressingMode()
{
}

AddressingMode::AddressingMode(QString bitPattern, AddressingMode::AddressingModeCode addressingModeCode, QString assemblyPattern)
{
    this->bitPattern = bitPattern;
    this->addressingModeCode = addressingModeCode;
    this->assemblyPattern = assemblyPattern;
}

QString AddressingMode::getBitPattern() const
{
    return bitPattern;
}

AddressingMode::AddressingModeCode AddressingMode::getAddressingModeCode() const
{
    return addressingModeCode;
}

QString AddressingMode::getAssemblyPattern() const
{
    return assemblyPattern;
}



