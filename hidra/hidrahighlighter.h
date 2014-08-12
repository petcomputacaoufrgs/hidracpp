#ifndef HIDRAHIGHLIGHTER_H
#define HIDRAHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class HidraHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit HidraHighlighter(QObject *parent = 0);
    HidraHighlighter ( QTextDocument * parent );
signals:

public slots:
    void highlightBlock(const QString &text);
};

#endif // HIDRAHIGHLIGHTER_H
