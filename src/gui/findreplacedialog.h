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

private slots:
    void on_cancelButton_clicked();

    void on_findButton_clicked();

    void on_replaceButton_clicked();

    void on_replaceAllButton_clicked();

protected:
     virtual void closeEvent(QCloseEvent *evt);

private:
    Ui::FindReplaceDialog *ui;
    HidraCodeEditor *editor;
    bool selected;

    void find();
    bool replace();
};

#endif // FINDREPLACEDIALOG_H
