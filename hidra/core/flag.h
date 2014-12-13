#ifndef FLAG_H
#define FLAG_H

#include <QString>

class Flag
{
public:
    Flag(QString name);
    Flag(QString name, bool defaultValue);

    QString getName() const;
    bool getValue() const;
    void setValue(bool value);
    void resetValue();

private:
    QString name;
    bool value;
    bool defaultValue;

};

#endif // FLAG_H
