#ifndef HIDRAHIGHLIGHTER_H
#define HIDRAHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include "machine.h"



//////////////////////////////////////////////////
// HighlightRule class
//////////////////////////////////////////////////

class HighlightRule
{
public:
    HighlightRule(QString pattern, QTextCharFormat format);
    QRegExp getRegExp();
    QTextCharFormat getFormat() const;

private:
    QRegExp regExp;
    QTextCharFormat format;
};



//////////////////////////////////////////////////
// HidraHighlighter class
//////////////////////////////////////////////////

class HidraHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit HidraHighlighter(QObject *parent = 0);
    HidraHighlighter ( QTextDocument * parent );

    void initializeHighlighter(Machine &machine);

    void initializeInstructionHighlightRule(Machine &machine);
    void initializeDirectivesHighlightRule();
    void initializeCommentsHighlightRule();

signals:

public slots:
    void highlightBlock(const QString &textOriginal);

private:
    Machine *machine;

    QList<HighlightRule> highlightRulesList;
};

#endif // HIDRAHIGHLIGHTER_H
