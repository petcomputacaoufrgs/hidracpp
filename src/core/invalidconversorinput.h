#ifndef INVALIDCONVERSORINPUT_H
#define INVALIDCONVERSORINPUT_H

#include <exception>
#include <string>
#include <QString>

class InvalidConversorInput: std::exception
{
public:
    InvalidConversorInput(QString message);

    QString getMessage();

    virtual char const *what() const noexcept;

private:
    std::string stdMessage;
    QString message;
};

#endif // INVALIDCONVERSORINPUT_H
