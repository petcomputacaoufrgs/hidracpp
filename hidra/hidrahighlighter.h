#ifndef HIDRAHIGHLIGHTER_H
#define HIDRAHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include "machine.h"

class HidraHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit HidraHighlighter(QObject *parent = 0);
    HidraHighlighter ( QTextDocument * parent );

    Machine *getTargetMachine() const;
    void setTargetMachine(Machine *value);

signals:

public slots:
    void highlightBlock(const QString &text);

private:
    Machine *targetMachine;
};

#endif // HIDRAHIGHLIGHTER_H
