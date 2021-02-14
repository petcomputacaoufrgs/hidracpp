#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(HidraCodeEditor *editor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);
    this->editor = editor;
    selected = false;
    current = 0;
    foundCount = 0;
    changingCount = 0;
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
    ui->caseCheckBox->setChecked(false);
    ui->regexCheckBox->setChecked(false);
}

void FindReplaceDialog::onSelectionChange()
{
    if (changingCount == 0) {
        selected = false;
        this->updateCounters();
    }
}

void FindReplaceDialog::updateCounters()
{
    /* RAII guard */
    ChangingGuard guard = this->changingGuard();

    QTextCursor cursor = editor->textCursor();
    int originalPos = cursor.selectionStart();
    int originalLength = cursor.selectionEnd() - originalPos;

    cursor.setPosition(0, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);

    QTextDocument::FindFlags flags = this->findFlags();
    QString findText = ui->findTextEdit->toPlainText();

    foundCount = 0;
    current = 0;

    while (editor->find(findText, flags)) {
        foundCount++;
        cursor = editor->textCursor();

        if (cursor.selectionStart() <= originalPos) {
            current++;
        }
    }

    cursor.setPosition(originalPos, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, originalLength);
    editor->setTextCursor(cursor);

    ui->labelCurrent->setText(QString::number(current));
    ui->labelFound->setText(QString::number(foundCount));
}

int FindReplaceDialog::replaceSizeDiff()
{
    return ui->replaceTextEdit->toPlainText().length() - ui->findTextEdit->toPlainText().length();
}

void FindReplaceDialog::find()
{
    /* RAII guard */
    ChangingGuard guard = this->changingGuard();

    QTextDocument::FindFlags flags = this->findFlags();
    QString findText = ui->findTextEdit->toPlainText();

    if (editor->find(findText, flags)) {
        selected = true;
    } else {
        editor->moveCursor(QTextCursor::Start);
        selected = editor->find(findText, flags);
    }
}

void FindReplaceDialog::replace()
{
    /* RAII guard */
    ChangingGuard guard = this->changingGuard();

    if (!selected) {
        this->find();
    }

    if (selected) {
        editor->insertPlainText(ui->replaceTextEdit->toPlainText());
        selected = false;
        this->find();
    }
}

void FindReplaceDialog::replaceAll()
{
    /* RAII guard */
    ChangingGuard guard = this->changingGuard();

    QTextCursor cursor = editor->textCursor();
    int originalPos = cursor.selectionStart();

    cursor.setPosition(0, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);

    QTextDocument::FindFlags flags = this->findFlags();
    QString findText = ui->findTextEdit->toPlainText();
    QString replaceText = ui->replaceTextEdit->toPlainText();

    int sizeDiff = replaceText.length() - findText.length();

    while (editor->find(findText, flags)) {
        cursor = editor->textCursor();

        if (cursor.selectionEnd() <= originalPos) {
            originalPos += sizeDiff;
        } else if (cursor.selectionStart() <= originalPos) {
            originalPos = cursor.selectionStart();
        }

        editor->insertPlainText(replaceText);
    }

    cursor.setPosition(originalPos, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);
}

QTextDocument::FindFlags FindReplaceDialog::findFlags()
{
    QTextDocument::FindFlags flags;

    if (ui->caseCheckBox->isChecked()) {
        flags.setFlag(QTextDocument::FindCaseSensitively);
    }

    return flags;
}

void FindReplaceDialog::closeEvent(QCloseEvent *evt)
{
    QDialog::closeEvent(evt);
    this->clearState();
    editor->unbindFindReplaceDialog();
}

void FindReplaceDialog::showEvent(QShowEvent *evt)
{
    QDialog::showEvent(evt);
    editor->bindFindReplaceDialog(this);
    this->updateCounters();
}

void FindReplaceDialog::on_findButton_clicked()
{
    this->find();
    this->updateCounters();
}

void FindReplaceDialog::on_cancelButton_clicked()
{
    this->close();
}

void FindReplaceDialog::on_replaceButton_clicked()
{
    this->replace();
    this->updateCounters();
}

void FindReplaceDialog::on_replaceAllButton_clicked()
{
    this->replaceAll();
    this->updateCounters();
}


FindReplaceDialog::ChangingGuard FindReplaceDialog::changingGuard()
{
    return FindReplaceDialog::ChangingGuard(changingCount);
}

FindReplaceDialog::ChangingGuard::ChangingGuard(int &count) :
    count(count)
{
    this->count++;
}

FindReplaceDialog::ChangingGuard::~ChangingGuard()
{
    this->count--;
}
