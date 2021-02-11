#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(HidraCodeEditor *editor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);
    this->editor = editor;
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

void FindReplaceDialog::on_findButton_clicked()
{
     editor->find(ui->findTextEdit->toPlainText());
}

void FindReplaceDialog::on_cancelButton_clicked()
{
    this->close();
}

void FindReplaceDialog::on_replaceButton_clicked()
{
    editor->insertPlainText(ui->replaceTextEdit->toPlainText());
}
