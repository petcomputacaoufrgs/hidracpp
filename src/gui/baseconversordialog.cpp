#include "baseconversordialog.h"
#include "ui_baseconversordialog.h"

using namespace std;


long  long Map(long  long i){
    if(i>='0' && i<='9') return i-48;
    else if (i>='a' && i<='z') return i-87;
    else return -1;
}

string baseConversor(string inputValor, long long inputBase, long long outputBase){
    char mapOutput[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    long long current, exp, decimalResult = 0;
    string outputResult="";

    for(int i=inputValor.size()-1; i>=0; i--){
        current = Map(inputValor[i]);
        exp=inputValor.size()-i-1;
        if(current>=0 && current<inputBase){
            decimalResult += current * pow(inputBase, exp);
        } //else return "Invalid input";
    }

    do{
        outputResult.insert(outputResult.begin(), (mapOutput[decimalResult % outputBase]));

        decimalResult /= outputBase;
    } while(decimalResult != 0);

        return outputResult;
}


BaseConversorDialog::BaseConversorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BaseConversorDialog)
{
    ui->setupUi(this);
}

BaseConversorDialog::~BaseConversorDialog()
{
    delete ui;
}

void BaseConversorDialog::on_pushButton_clicked()
{
    string inputValue = ui->lineEditInputValor->text().toStdString();
    string inputBaseString = ui->lineEditInputBase->text().toStdString();
    string outputBaseString = ui->lineEditOutputBase->text().toStdString();
    int inputBase = stoll(inputBaseString);
    int outputBase = stoll(outputBaseString);


    ui->lineEditOutputValor->setText(QString::fromStdString(baseConversor(inputValue, inputBase, outputBase)));
}

void BaseConversorDialog::on_pushButton_2_clicked()
{
    QString inputBaseStringQ = ui->lineEditInputBase->text();
    ui->lineEditInputBase->setText(ui->lineEditOutputBase->text());
    ui->lineEditOutputBase->setText(inputBaseStringQ);
}
