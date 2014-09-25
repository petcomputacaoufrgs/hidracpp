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

private:
    QString name;
    bool value;
};

#endif // FLAG_H
