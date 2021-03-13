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
    this->clearCounters();
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
    this->clearCounters();
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

    ui->labelCurrent->setText("?");
    ui->labelFound->setText("?");
}

void FindReplaceDialog::updateCounters()
{
    /* RAII guard */
    ChangingGuard guard(this->changingGuard());

    bool prevSelected = selected;

    /* Saves original position to restore later. */
    QTextCursor cursor = editor->textCursor();
    int originalPos = cursor.selectionStart();
    int originalLength = cursor.selectionEnd() - originalPos;

    /* Moves to the beginning of the document. */
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);

    foundCount = 0;
    current = 0;

    while (this->findRaw()) {
        foundCount++;
        cursor = editor->textCursor();

        /* Counts this search as behind original selection. */
        if (cursor.selectionStart() <= originalPos) {
            current++;
        }
    }

    /* Restores original selection. */
    cursor.setPosition(originalPos, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, originalLength);
    editor->setTextCursor(cursor);

    /* Updates UI display. */
    ui->labelCurrent->setText(QString::number(current));
    ui->labelFound->setText(QString::number(foundCount));

    selected = prevSelected;
}

bool FindReplaceDialog::findRaw()
{
    /* RAII guard */
    ChangingGuard guard(this->changingGuard());

    if (ui->regexCheckBox->isChecked()) {
        QRegExp findRegex(this->findRegex());
        QTextDocument::FindFlags flags = this->findFlags();

        /*
         * We might restore cursor position depending on whether a valid
         * search result is found.
         */
        int selectionStart = editor->textCursor().selectionStart();

        selected = editor->find(findRegex, flags);

        /*
         * Attention:
         *
         * The stuff below is required since a Regex can find a zero-length
         * string. If we do not deal with this, an infinite loop may happen in
         * other methods.
         */

        QTextCursor cursor(editor->textCursor());

        /* If the search resulted in a valid selection. */
        bool hasSelection = cursor.hasSelection();
        /* If the search hit the end of the document. */
        bool atEnd = cursor.atEnd();

        if (selected && !hasSelection && !atEnd) {
            /* If found, but selection empty and not at the end, moves one
             * character in order to make this not an infinite loop.
             */
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            editor->setTextCursor(cursor);
            selected = editor->find(findRegex, flags);
        } else if (selected && !hasSelection && atEnd) {
            /*
             * If found, selection empty but at the end, restores position and
             * registers this search as a failure.
             */
            cursor = editor->textCursor();
            cursor.setPosition(selectionStart);
            editor->setTextCursor(cursor);
            selected = false;
        }
    } else {
        QString findText(ui->findTextEdit->toPlainText());
        selected = editor->find(findText, this->findFlags());
    }

    return selected;
}

int FindReplaceDialog::replaceRaw()
{
    /* RAII guard */
    ChangingGuard guard(this->changingGuard());

    int diff = 0;

    if (ui->regexCheckBox->isChecked()) {
        QRegExp findRegex(this->findRegex());
        QString selection(editor->textCursor().selectedText());

        /*
         * Matches the selection with the regular expression and gets captured
         * groups.
         */
        findRegex.indexIn(selection);
        QStringList matches(findRegex.capturedTexts());

        /*
         * Expression given by the user to replace the match, using $i to refer
         * to captured groups, $0 to refer to the whole match.
         */
        QString replaceExpr(ui->replaceTextEdit->toPlainText());

        /* The first piece will not contain a '$'. */
        QStringList pieces(replaceExpr.split('$'));
        QString replaceText(pieces[0]);

        /* Keep track if previous is between "$$". */
        bool prevEmpty = false;

        for (int i = 1; i < pieces.length(); i++) {
            if (prevEmpty) {
                /* Previous empty? Ignore '$' and just append things. */
                replaceText += pieces[i];
                prevEmpty = false;
            } else if (pieces[i].length() == 0) {
                /* Current empty? Between "$$". */
                replaceText += '$';
                prevEmpty = true;
            } else {
                /* Otherwise, potential parameter. */
                this->replaceRegexParam(replaceText, pieces[i], matches);
            }
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

void FindReplaceDialog::replaceRegexParam(QString &replaceText, QString const &piece, QStringList const &matches)
{
    int cut = 0;
    int group = 0;
    bool inBounds = true;
    bool hasGroup = false;

    /*
     * Attempts to parse a captured group index.
     *
     * Iterate while inside piece, current is digit, and computed group is in
     * bounds of the number of captured groups.
     *
     * It becomes out of bounds if a newly computed group is out of bounds.
     */
    while (cut < piece.length() && piece[cut].isDigit() && inBounds) {
        int newGroup = group * 10 + piece[cut].digitValue();
        inBounds = newGroup < matches.length();
        if (inBounds) {
            group = newGroup;
            /* Save the fact that at least one digit was parsed. */
            hasGroup = true;
            cut++;
        }
    }

    if (hasGroup) {
        replaceText += matches[group];
    }
    replaceText += piece.rightRef(piece.length() - cut);
}

void FindReplaceDialog::find()
{
    /* RAII guard */
    ChangingGuard guard(this->changingGuard());

    if (!this->findRaw()) {
        editor->moveCursor(QTextCursor::Start);
        this->findRaw();
    }
}

void FindReplaceDialog::replace()
{
    /* RAII guard */
    ChangingGuard guard(this->changingGuard());

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
    ChangingGuard guard(this->changingGuard());

    /* Saves original position. */
    QTextCursor cursor = editor->textCursor();
    int originalPos = cursor.selectionStart();

    /* Moves to the start. */
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);

    while (this->findRaw()) {
        cursor = editor->textCursor();

        /*
         * Replaces and gets the difference between replaced text and found
         * text.
         */
        int sizeDiff = this->replaceRaw();

        /*
         * Takes size difference into account in order to maintain original
         * position correct.
         */
        if (cursor.selectionEnd() <= originalPos) {
            originalPos += sizeDiff;
        } else if (cursor.selectionStart() <= originalPos) {
            originalPos = cursor.selectionStart();
        }
    }

    /* Restore original position. */
    cursor.setPosition(originalPos, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);
}

void FindReplaceDialog::replaceSelection()
{
    /* RAII guard */
    ChangingGuard guard(this->changingGuard());

    /* Gets our bounds. */
    QTextCursor cursor = editor->textCursor();
    int selectionStart = cursor.selectionStart();
    int selectionEnd = cursor.selectionEnd();

    /* Moves to the beginning of the selection. */
    cursor.setPosition(selectionStart, QTextCursor::MoveAnchor);
    editor->setTextCursor(cursor);

    /* Keeps track if we still are inside the selection. */
    bool insideSelection = true;

    while (this->findRaw() && insideSelection) {
        /* Are we inside the selection? */
        cursor = editor->textCursor();
        insideSelection = cursor.selectionEnd() <= selectionEnd;

        if (insideSelection) {
            /* If so, replaces and takes difference into account. */
            int sizeDiff = this->replaceRaw();
            selectionEnd += sizeDiff;
        }
    }

    /* Restores the cursor to the beginning of the selection. */
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

QRegExp FindReplaceDialog::findRegex()
{
    QRegExp regex(ui->findTextEdit->toPlainText());
    if (ui->caseCheckBox->isChecked()) {
        regex.setCaseSensitivity(Qt::CaseSensitive);
    } else {
        regex.setCaseSensitivity(Qt::CaseInsensitive);
    }
    return regex;
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
