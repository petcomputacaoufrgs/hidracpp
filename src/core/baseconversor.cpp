#include "baseconversor.h"

BaseConversor::BaseConversor()
{
    bits = 0;
    negativeSignal = false;
    signalMagnitude = false;
}

long long unsigned BaseConversor::getBits()
{
    return bits;
}

void BaseConversor::setBits(long long unsigned input){
    bits = input;
}

bool BaseConversor::getNegativeSignal(){
    return negativeSignal;
}

int BaseConversor::mapInput(char i)
{
    if(i>='0' && i<='9') return i-48;
    else if (i>='a' && i<='z') return i-87;
    else if (i>='A' && i<='Z') return i-55;
    else return -1;
}

char BaseConversor::mapOutput(int i)
{
    char map[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return map[i];
}


// Input functions

long long unsigned BaseConversor::getDec(QString digitsQ, int base)
{
    long long unsigned current, exp, decimalResult = 0;
    std::string digits = digitsQ.toStdString();

    for(int i=digits.size()-1; i>=0; i--){
        current = mapInput(digits[i]);
        exp=digits.size()-i-1;
        decimalResult += current * pow(base, exp);
     }

    return decimalResult;
}

long long unsigned BaseConversor::getComplement(QString digitsQ, int base)
{
    std::string digits=digitsQ.toStdString();
    unsigned long long inputValue = getDec(digitsQ, base);
    unsigned int length = digits.length();

    // Case the base is even just check msb
    if(base % 2 == 0) {
        if(mapInput(digits[0]) < base/2){
            bits = inputValue;
            negativeSignal = false;
        }
        else {
            bits = pow(base, length) - 1 - inputValue;
            negativeSignal  = true;
        }
    }

    // Case the base is odd, check each digit starting from the msb
    // until find one that's is greater or less than base/2
    else {
        unsigned int i=0;
        do{
            if(mapInput(digits[i]) < base/2 ||
                    (i == length-1 && mapInput(digits[i]) == base/2)){
                bits = inputValue;
                negativeSignal = false;
                break;
            }
            else if(mapInput(digits[i]) > base/2){
                bits = pow(base, length) - 1 - inputValue;
                negativeSignal  = true;
                break;
            }

            i++;
            if (i == length) break;

        }while(1);
    }
    return bits;
}

BaseConversor& BaseConversor::inputPositive(QString digitsQ, int base)
{
    bits = getDec(digitsQ, base);
    negativeSignal = false;
    return *this;
}

BaseConversor& BaseConversor::inputSignMagnitude(QString digitsQ, int base)
{
    long long unsigned current, exp, decimalResult = 0;
    std::string digits=digitsQ.toStdString();

    for(int i=digits.size()-1; i>0; i--){
        current = mapInput(digits[i]);
        exp=digits.size()-i-1;
        decimalResult += current * pow(base, exp);
     }

    if(digits[0] == '1')
        negativeSignal = true;
    else
        negativeSignal = false;

    bits = decimalResult;
    signalMagnitude = true;
    return *this;
}


BaseConversor& BaseConversor::inputOnesComplement(QString digitsQ, int base)
{
   bits =getComplement(digitsQ, base);
   return *this;
}


BaseConversor& BaseConversor::inputTwosComplement(QString digitsQ, int base)
{
    bits = getComplement(digitsQ, base);
    if(negativeSignal == true)
        bits += 1;

    return *this;
}




// Output functions

QString BaseConversor::outputPositive(int base)
{
    std::string outputResult = "";
    long long unsigned decimalInput = getBits();

    do{
        outputResult.insert(outputResult.begin(), (mapOutput(decimalInput % base)));
        decimalInput /= base;
    } while(decimalInput != 0);

    return QString::fromStdString(outputResult);
}

QString BaseConversor::outputSignMagnitude(int base)
{
    QString outputResultQ = outputPositive(base);
    std::string outputResult = outputResultQ.toStdString();

    if(negativeSignal == true)
        outputResult.insert(outputResult.begin(), '-');
    else
        outputResult.insert(outputResult.begin(), '+');

    return QString::fromStdString(outputResult);
}

QString BaseConversor::outputOnesComplement(int base)
{
    QString stringValue = outputPositive(base);
    if(negativeSignal == true) {
        unsigned long long maxValue = pow(base, stringValue.length());
        //if(signalMagnitude != false)
          //  maxValue += base;
        bits = maxValue - 1 - bits;
        stringValue = outputPositive(base);
    }
    else{
        stringValue.insert(0, "0");
    }
    return stringValue;
}


QString BaseConversor::outputTwosComplement(int base)
{
    QString stringValue = outputPositive(base);
    if(negativeSignal == true) {
        unsigned long long maxValue = pow(base, stringValue.length());
        bits = maxValue - bits;
        stringValue = outputPositive(base);
    }
    else {
        stringValue.insert(0, "0");
    }
    return stringValue;
}




// Validation functions

QString BaseConversor::validate(int baseIn, int baseOut, QString digits)
{
    if (baseIn < 2 || baseIn > 36)
        return "Base de entrada deve ser entre 2 e 36!";
    if (baseOut < 2 || baseOut > 36)
        return "Base de saída deve ser entre 2 e 36!";

    std::string inputValor = digits.toStdString();
    int current;
    for(unsigned int i=0; i<inputValor.size(); i++){
        current = mapInput(inputValor[i]);
        if(current<0 || current>=baseIn)
            return "Entrada contem digitos inválidos!";
    }

    return "";
}

QString BaseConversor::validateSignMagnitude(int baseIn, int baseOut, QString digits)
{
    QString error = validate(baseIn, baseOut, digits);

    if(digits[0] != '1' && digits[0] != '0')
        error = "Em sinal magnitude o primeiro digito deve ser 0(+) ou 1(-)";
    else if(digits.length() < 2)
        error = "É necessário ter no mínimo dois dígitos (um para o sinal)";

    return error;
}
