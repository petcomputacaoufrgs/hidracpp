#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QString>

class Instruction
{
public:
    Instruction();
    Instruction(QString, int, int, int);

    QString getMnemonic() const;
    void setMnemonic(const QString &value);

    int getValue() const;
    void setValue(int value);

    int getNumberOfArguments() const;
    void setNumberOfArguments(int value);

    int getSize() const;
    void setSize(int size);

private:
    QString mnemonic;
    int value;
    int numberOfArguments;
    int size;
};

#endif // INSTRUCTION_H
