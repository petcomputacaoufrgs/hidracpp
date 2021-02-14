#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include "hidracodeeditor.h"

namespace Ui {
class FindReplaceDialog;
}

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(HidraCodeEditor *editor, QWidget *parent = nullptr);
    ~FindReplaceDialog();

    void clearState();

public slots:
    void onSelectionChange();

private slots:
    void on_cancelButton_clicked();
    void on_findButton_clicked();
    void on_replaceButton_clicked();
    void on_replaceAllButton_clicked();

protected:
    virtual void closeEvent(QCloseEvent *evt);
    virtual void showEvent(QShowEvent *evt);

private:
    Ui::FindReplaceDialog *ui;
    HidraCodeEditor *editor;
    bool selected;
    int changingCount;
    int current;
    int foundCount;

    void updateCounters();

    int replaceSizeDiff();

    void find();
    void replace();
    void replaceAll();

    QTextDocument::FindFlags findFlags();

    /*
     * This is a RAII guard, in order to prevent the onSelectionChange method
     * from performing its actions when the FindReplaceDialog itself is changing
     * the editor.
     *
     * This increments the counter on construction, decrements when out of
     * scope (destroyed).
     *
     * To use this, just place
     *
     * ChangingGuard guard = this->changingGuard();
     *
     * on the top of the scope. It automatically is deleted at the end of the
     * scope.
     */
    class ChangingGuard {
    public:
        ChangingGuard(int &count);
        ~ChangingGuard();
    private:
        int &count;
    };

    ChangingGuard changingGuard();
};

#endif // FINDREPLACEDIALOG_H
