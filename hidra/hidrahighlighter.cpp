#include "hidrahighlighter.h"

HidraHighlighter::HidraHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
}

HidraHighlighter::HidraHighlighter::HidraHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{

}

void HidraHighlighter::highlightBlock(const QString & text)
{
    QTextCharFormat myClassFormat;
    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);
    QString pattern = "^lda$";

    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
    }
}
