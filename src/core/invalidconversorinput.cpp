#include "invalidconversorinput.h"

InvalidConversorInput::InvalidConversorInput(QString message):
    stdMessage(message.toStdString()),
    message(message)
{
}

QString InvalidConversorInput::getMessage() const
{
    return this->message;
}

char const *InvalidConversorInput::what() const noexcept
{
    return this->stdMessage.c_str();
}
