#include "hidrahighlighter.h"

#define DEBUG_INT(value) qDebug(QString::number(value).toStdString().c_str());
#define DEBUG_STRING(value) qDebug(value.toStdString().c_str());

//////////////////////////////////////////////////
// HighlightRule methods
//////////////////////////////////////////////////

HighlightRule::HighlightRule(QString pattern, QTextCharFormat format)
{
    this->regExp = QRegExp(pattern);
    this->regExp.setCaseSensitivity(Qt::CaseInsensitive);
    this->format = format;
}

QRegExp HighlightRule::getRegExp()
{
    return regExp;
}

QTextCharFormat HighlightRule::getFormat() const
{
    return format;
}



//////////////////////////////////////////////////
// HidraHighlighter methods
//////////////////////////////////////////////////

HidraHighlighter::HidraHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent)
{
}

HidraHighlighter::HidraHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{

}

void HidraHighlighter::highlightBlock(const QString &originalText)
{
    QString text = originalText;
    text.replace("'''", "___"); // Remove literal quotes before applying rules

    foreach (HighlightRule highlightRule, highlightRulesList)
    {
        QRegExp regExp = highlightRule.getRegExp();

        int index = text.indexOf(regExp); // Search first occurrence of pattern

        while (index >= 0) // While there are occurrences of pattern
        {
            int length = regExp.cap(1).length();
            setFormat(regExp.pos(1), length, highlightRule.getFormat()); // Set text format
            index = text.indexOf(regExp, index + regExp.matchedLength()); // Search next occurrence
        }
    }
}

void HidraHighlighter::initializeInstructionHighlightRule(Machine &machine)
{
    // Create new regular expression pattern containing instructions
    QString instructionsPattern = "(";
    foreach (Instruction *instructionPtr, machine.getInstructions())
    {
        instructionsPattern.append("\\b" + instructionPtr->getMnemonic()+"\\b|");
    }
    instructionsPattern.chop(1);
    instructionsPattern.append(")");

    // Set corresponding format
    QTextCharFormat instructionsFormat;

    instructionsFormat.setFontWeight(QFont::Bold);
    //instructionsFormat.setForeground(Qt::darkMagenta);
    instructionsFormat.setForeground(QColor(237, 175, 2));

    // Append to list of rules
    highlightRulesList.push_back(HighlightRule(instructionsPattern, instructionsFormat));
}

void HidraHighlighter::initializeDirectivesHighlightRule()
{
    // Create new regular expression pattern containing directives
    QString directivesPattern = "(";

    directivesPattern.append("\\b" + QString("org") + "\\b|");
    directivesPattern.append("\\b" + QString("db") + "\\b|");
    directivesPattern.append("\\b" + QString("dw") + "\\b|");
    directivesPattern.append("\\b" + QString("dab") + "\\b|");
    directivesPattern.append("\\b" + QString("daw") + "\\b|");

    directivesPattern.chop(1);
    directivesPattern.append(")");

    // Set corresponding format
    QTextCharFormat directivesFormat;

    directivesFormat.setFontWeight(QFont::Bold);
    directivesFormat.setForeground(QColor(68, 119, 176));

    // Append to list of rules
    highlightRulesList.push_back(HighlightRule(directivesPattern, directivesFormat));
}

void HidraHighlighter::initializeCommentsHighlightRule()
{
    const QString CHARS_EXCEPT_QUOTE_SEMICOLON = "[^';]*";
    const QString STRING = "(?:'[^']*')";
    const QString COMMENT = "(;.*)$";

    // Create new regular expression pattern
    QString commentsPattern = "^(?:" + CHARS_EXCEPT_QUOTE_SEMICOLON + STRING + "?)*" + COMMENT;

    // Set corresponding format
    QTextCharFormat commentsFormat;

    commentsFormat.setFontWeight(QFont::Normal);
    commentsFormat.setForeground(Qt::darkGreen);
    commentsFormat.setFontItalic(true);

    // Append to list of rules
    highlightRulesList.push_back(HighlightRule(commentsPattern, commentsFormat));
}

void HidraHighlighter::initializeHighlighter(Machine &machine)
{
    highlightRulesList.clear();

    initializeInstructionHighlightRule(machine);
    initializeDirectivesHighlightRule();
    initializeCommentsHighlightRule();
}
