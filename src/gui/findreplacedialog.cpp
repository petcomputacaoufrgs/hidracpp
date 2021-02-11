#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(HidraCodeEditor *editor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);
    this->editor = editor;
    selected = false;
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

void FindReplaceDialog::clearState()
{
    selected = false;
    ui->findTextEdit->clear();
    ui->replaceTextEdit->clear();
}

void FindReplaceDialog::find()
{
    if (editor->find(ui->findTextEdit->toPlainText())) {
        selected = true;
    } else {
        editor->moveCursor(QTextCursor::Start);
        selected = editor->find(ui->findTextEdit->toPlainText());
    }
}

bool FindReplaceDialog::replace()
{
    if (!selected) {
        this->find();
    }

    if (selected) {
        editor->insertPlainText(ui->replaceTextEdit->toPlainText());
        selected = false;
        return true;
    } else {
        return false;
    }
}

void FindReplaceDialog::closeEvent(QCloseEvent *evt)
{
    QDialog::closeEvent(evt);
    this->clearState();
}

void FindReplaceDialog::on_findButton_clicked()
{
    find();
}

void FindReplaceDialog::on_cancelButton_clicked()
{
    this->close();
}

void FindReplaceDialog::on_replaceButton_clicked()
{
    editor->insertPlainText(ui->replaceTextEdit->toPlainText());
}

void FindReplaceDialog::on_replaceAllButton_clicked()
{
    while (this->replace()) {}
}
