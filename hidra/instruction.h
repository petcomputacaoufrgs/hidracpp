#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QString>

class Instruction
{
public:
    Instruction();
    Instruction(QString, int, int);

    QString getMnemonic() const;
    void setMnemonic(const QString &value);

    int getValue() const;
    void setValue(int value);

    int getNumberOfArguments() const;
    void setNumberOfArguments(int value);

private:
    QString mnemonic;
    int value;
    int numberOfArguments;
};

#endif // INSTRUCTION_H
