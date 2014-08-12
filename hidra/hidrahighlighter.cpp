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
    QVector<Instruction *> instructions = targetMachine->getInstructions();
    QString pattern = "(";
    foreach (Instruction *tmp, instructions) {
        pattern.append(tmp->getMnemonic()+"|");
    }
    pattern.chop(1);
    pattern.append(")");

    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
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

