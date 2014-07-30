#ifndef HIDRACLI_H
#define HIDRACLI_H

#include <QObject>
#include <iostream>
#include <string>
#include "neandermachine.h"
#include "ahmesmachine.h"
#include "ramsesmachine.h"

class HidraCli : public QObject
{
    Q_OBJECT
public:
    explicit HidraCli(QStringList argv, int argc, QObject *parent = 0);

    void handleCommandPrompt();
signals:
    
public slots:

private:
    Machine *machine;
    QVector<Byte*> auxMemory;
};

#endif // HIDRACLI_H
