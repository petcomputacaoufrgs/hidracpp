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

private slots:
    void on_cancelButton_clicked();

    void on_findButton_clicked();

    void on_replaceButton_clicked();

private:
    Ui::FindReplaceDialog *ui;
    HidraCodeEditor *editor;
};

#endif // FINDREPLACEDIALOG_H
