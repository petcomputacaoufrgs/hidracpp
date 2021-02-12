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

void FindReplaceDialog::onSelectionChange()
{
    selected = false;
}

int FindReplaceDialog::replaceSizeDiff()
{
    return ui->replaceTextEdit->toPlainText().length() - ui->findTextEdit->toPlainText().length();
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

void FindReplaceDialog::replace()
{
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
    /*
     * Difference between sizes of replace text and find text, required to
     * recompute positions when a substitution happens.
     */
    int sizeDiff = this->replaceSizeDiff();

    /* Position where we started. */
    QTextCursor cursor = editor->textCursor();
    int originalPos = cursor.selectionStart();

    /* Selects something if not selected. */
    if (!selected) {
        this->find();
    }

    /* Start and end of the first selection. */
    cursor = editor->textCursor();
    int lastPosStart = cursor.selectionStart();
    int lastPosEnd = cursor.selectionEnd();

    /* We need to keep track if the selection is reaching original position. */
    bool inBounds = true;

    while (selected && inBounds) {
        /*
         * Adjust original position due to replace below.
         *
         * It is ok to perform this if no selection is found after the
         * replacement. We can compute inBounds below, and even if it is
         * true, the object's selected attribute will be false next iteration,
         * and all we do until then is to compute inBounds.
         */
        if (originalPos > lastPosStart) {
            originalPos += sizeDiff;
        }

        /*
         * Required since we are replacing text in position given by lastPos.
         */
        lastPosEnd += sizeDiff;

        this->replace();

        /* Position of the selection AFTER the replacement above. */
        cursor = editor->textCursor();
        int currPosStart = cursor.selectionStart();
        int currPosEnd = cursor.selectionEnd();

        /* We assume it is false by default. */
        inBounds = false;

        /* Only true if one of the following conditions hold. */
        if (currPosStart > lastPosStart && lastPosStart >= originalPos) {
            inBounds = true;
        } else if (lastPosStart >= originalPos && originalPos > currPosStart) {
            inBounds = true;
        } else if (originalPos > currPosStart && currPosStart > lastPosStart) {
            inBounds = true;
        }

        /*
         * inBounds will only be true if one of the conditions above happens,
         * and if any of the ones below is true, inBounds will be false again.
         */
        if (currPosEnd > originalPos && originalPos > currPosStart) {
            inBounds = false;
        } else if (currPosEnd > lastPosEnd && lastPosEnd > currPosStart) {
            inBounds = false;
        } else if (currPosEnd > lastPosStart && lastPosStart > currPosStart) {
            inBounds = false;
        }

        /* Now the last selection is the current one. */
        lastPosStart = currPosStart;
        lastPosEnd = currPosEnd;
    }
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
}

void FindReplaceDialog::on_findButton_clicked()
{
    this->find();
}

void FindReplaceDialog::on_cancelButton_clicked()
{
    this->close();
}

void FindReplaceDialog::on_replaceButton_clicked()
{
    this->replace();
}

void FindReplaceDialog::on_replaceAllButton_clicked()
{
    this->replaceAll();
}
