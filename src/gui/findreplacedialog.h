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

    int replaceSizeDiff();

    void find();
    void replace();
    void replaceAll();
};

#endif // FINDREPLACEDIALOG_H
