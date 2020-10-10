#include "baseconversor.h"

BaseConversor::BaseConversor()
{
    bits = 0;
}

int BaseConversor::mapInput(char i){
    if(i>='0' && i<='9') return i-48;
    else if (i>='a' && i<='z') return i-87;
    else if (i>='A' && i<='Z') return i-55;
    else return -1;
}

BaseConversor& BaseConversor::input(QString digitsQ, int base)
{
    long long unsigned current, exp, decimalResult = 0;
    std::string digits = digitsQ.toStdString();

    for(int i=digits.size()-1; i>=0; i--){
        current = mapInput(digits[i]);
        exp=digits.size()-i-1;
        if(current>=0 && current<base)
            decimalResult += current * pow(base, exp);

     }
    bits = decimalResult;
    return *this;
}

long long unsigned BaseConversor::getBits()
{
    return bits;
}

QString BaseConversor::output(int base)
{
    std::string outputResult = "";
    char mapOutput[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    long long unsigned decimalInput = getBits();

    do{
        outputResult.insert(outputResult.begin(), (mapOutput[decimalInput % base]));
        decimalInput /= base;
    } while(decimalInput != 0);

    return QString::fromStdString(outputResult);
}

QString BaseConversor::inputValidation(int baseIn, int baseOut, QString digits)
{
    if (baseIn < 2 || baseIn > 36)
        return "Base de entrada deve ser entre 2 e 36!";
    if (baseOut < 2 || baseOut > 36)
        return "Base de saída deve ser entre 2 e 36!";

    std::string inputValor = digits.toStdString();
    int current;
    for(int i=0; i<inputValor.size(); i++){
        current = mapInput(inputValor[i]);
        if(current<0 || current>=baseIn)
            return "Entrada contem digitos inválidos!";
    }

    return "";
}
