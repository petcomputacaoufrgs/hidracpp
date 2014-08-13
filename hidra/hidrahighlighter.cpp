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
    QTextCharFormat mnemonicsFormat;
    mnemonicsFormat.setFontWeight(QFont::Bold);
    mnemonicsFormat.setForeground(Qt::darkMagenta);
    QVector<Instruction *> instructions = targetMachine->getInstructions();
    QString pattern = "(";
    foreach (Instruction *tmp, instructions) {
        pattern.append("\\b" + tmp->getMnemonic()+"\\b|");
    }
    pattern.chop(1);
    pattern.append(")");

    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, mnemonicsFormat);
        index = text.indexOf(expression, index + length);
    }

    QTextCharFormat commentsFormat;
    commentsFormat.setFontWeight(QFont::Normal);
    commentsFormat.setFontItalic(true);
    commentsFormat.setForeground(Qt::lightGray);
    pattern = ";.*";

    expression = QRegExp(pattern);
    index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, commentsFormat);
        index = text.indexOf(expression, index + length);
    }

}
Machine *HidraHighlighter::getTargetMachine() const
{
    return targetMachine;
}

void HidraHighlighter::setTargetMachine(Machine *value)
{
    targetMachine = value;
}

