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
        this->clearCounters();
    }
}

void FindReplaceDialog::clearCounters()
{
    foundCount = 0;
    current = 0;

    ui->labelCurrent->setText(QString::number(current));
    ui->labelFound->setText(QString::number(foundCount));
}

void FindReplaceDialog::updateCounters()
{
    /* RAII guard */
    ChangingGuard guard = this->changingGuard();

    bool prevSelected = selected;

    QTextCursor cursor = editor->textCursor();
    int originalPos = cursor.selectionStart();
    int originalLength = cursor.selectionEnd() - originalPos;

    cursor.setPosition(0, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);

    foundCount = 0;
    current = 0;

    while (this->findRaw()) {
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

    selected = prevSelected;
}

bool FindReplaceDialog::findRaw()
{
    /* RAII guard */
    ChangingGuard guard = this->changingGuard();

    if (ui->regexCheckBox->isChecked()) {
        QRegularExpression findRegex(ui->findTextEdit->toPlainText());
        selected = editor->find(findRegex, this->findFlags());
    } else {
        QString findText(ui->findTextEdit->toPlainText());
        selected = editor->find(findText, this->findFlags());
    }

    return selected;
}

int FindReplaceDialog::replaceRaw()
{
    /* RAII guard */
    ChangingGuard guard = this->changingGuard();

    int diff = 0;

    if (ui->regexCheckBox->isChecked()) {
        QRegularExpression findRegex(ui->findTextEdit->toPlainText());
        QString selection(editor->textCursor().selectedText());
        QStringList matches(findRegex.match(selection).capturedTexts());
        QString replaceExpr(ui->replaceTextEdit->toPlainText());

        QStringList pieces(replaceExpr.split('$'));
        QString replaceText(pieces[0]);

        for (int i = 1; i < pieces.length(); i++) {
            this->appendRegexPiece(replaceText, pieces[i], matches);
        }

        editor->insertPlainText(replaceText);
        diff = replaceText.length() - selection.length();
    } else {
        QString findText(ui->findTextEdit->toPlainText());
        QString replaceText(ui->replaceTextEdit->toPlainText());
        editor->insertPlainText(replaceText);
        diff = replaceText.length() - findText.length();
    }

    selected = false;
    return diff;
}

void FindReplaceDialog::appendRegexPiece(QString &replaceText, QString const &piece, QStringList const &matches)
{
    if (piece.length() == 0) {
        replaceText += '$';
    } else {
        int cut = 0;
        int group = 0;
        bool inBounds = true;
        bool hasGroup = false;

        while (cut < piece.length() && piece[cut].isDigit() && inBounds) {
            int newGroup = group * 10 + piece[cut].digitValue();
            inBounds = newGroup < matches.length();
            if (inBounds) {
                group = newGroup;
                hasGroup = true;
                cut++;
            }
        }

        if (hasGroup) {
            replaceText += matches[group];
        }
        replaceText += piece.rightRef(piece.length() - cut);
    }
}

void FindReplaceDialog::find()
{
    /* RAII guard */
    ChangingGuard guard = this->changingGuard();

    if (!this->findRaw()) {
        editor->moveCursor(QTextCursor::Start);
        this->findRaw();
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
        this->replaceRaw();
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

    while (this->findRaw()) {
        cursor = editor->textCursor();

        int sizeDiff = this->replaceRaw();

        if (cursor.selectionEnd() <= originalPos) {
            originalPos += sizeDiff;
        } else if (cursor.selectionStart() <= originalPos) {
            originalPos = cursor.selectionStart();
        }
    }

    cursor.setPosition(originalPos, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);
}

void FindReplaceDialog::replaceSelection()
{
    /* RAII guard */
    ChangingGuard guard = this->changingGuard();

    QTextCursor cursor = editor->textCursor();
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();

    cursor.setPosition(selectionStart, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);

    bool insideSelection = true;

    while (this->findRaw() && insideSelection) {
        cursor = editor->textCursor();
        insideSelection = cursor.selectionEnd() <= selectionEnd;

        if (insideSelection) {
            int sizeDiff = this->replaceRaw();
            selectionEnd += sizeDiff;
        }
    }

    cursor.setPosition(selectionStart, QTextCursor::MoveAnchor);
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
    this->clearCounters();
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

void FindReplaceDialog::on_replaceSelected_clicked()
{
    this->replaceSelection();
    this->updateCounters();
}
