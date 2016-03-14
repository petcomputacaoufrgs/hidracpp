#ifndef FLAG_H
#define FLAG_H

#include <QString>

class Flag
{
public:
    enum FlagCode
    {
        NEGATIVE, ZERO, CARRY, BORROW, OVERFLOW_FLAG, CARRY_NOT_BORROW
    };

    Flag();
    Flag(FlagCode flagCode, QString name);
    Flag(FlagCode flagCode, QString name, bool defaultValue);

    FlagCode getFlagCode() const;
    QString getName() const;
    bool getValue() const;
    void setValue(bool value);
    void resetValue();

private:
    FlagCode flagCode;
    QString name;
    bool value;
    bool defaultValue;

};

#endif // FLAG_H
