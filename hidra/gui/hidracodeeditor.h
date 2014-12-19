#ifndef HIDRACODEEDITOR_H
#define HIDRACODEEDITOR_H

#include <QtGui>
#include <QPlainTextEdit>
#include <QObject>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QWidget>

class LineNumberArea;

class HidraCodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    HidraCodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void highlightPCLine(int pcLine);
    int getBreakpointLine();
    void toggleBreakpointOnCursor();
    void disableLineHighlight();

public slots:

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
    QTextBlock breakpointBlock;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(HidraCodeEditor *editor);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    HidraCodeEditor *codeEditor;
};

#endif // HIDRACODEEDITOR_H
