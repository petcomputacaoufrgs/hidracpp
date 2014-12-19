#include "hidracodeeditor.h"

HidraCodeEditor::HidraCodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    // Set font to Courier (monospaced)
    this->setFont(QFont("Courier", 10));

    lineNumberArea = new LineNumberArea(this);
    lineNumberArea->setFont(QFont("Courier", 10));

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    //connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    //highlightCurrentLine();
}



int HidraCodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = fontMetrics().width(QLatin1Char('9')) * digits + 6;

    return space;
}



void HidraCodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void HidraCodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void HidraCodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void HidraCodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(QColor(240, 240, 240));

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}



void HidraCodeEditor::highlightPCLine(int pcLine)
{
    bool blockFound = false;
    QTextBlock block = firstVisibleBlock();

    // Find PC line's block
    while (!blockFound && block.isValid() && block.isVisible())
    {
        if (block.blockNumber() == pcLine)
            blockFound = true;
        else
            block = block.next();
    }

    // Highlight block, if found
    if (blockFound)
    {
        QList<QTextEdit::ExtraSelection> extraSelections;

        if (!isReadOnly())
        {
            QTextEdit::ExtraSelection selection;

            QColor lineColor = QColor(255, 240, 0); // Yellow

            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = QTextCursor(block); // PC Block
            selection.cursor.clearSelection();
            extraSelections.append(selection);
        }

        setExtraSelections(extraSelections);
    }
    else
    {
        disableLineHighlight();
    }
}

int HidraCodeEditor::getBreakpointLine()
{
    if (breakpointBlock.isValid())
        return this->breakpointBlock.blockNumber();
    else
        return -1;
}

void HidraCodeEditor::toggleBreakpointOnCursor()
{
    int oldBreakpointLine = breakpointBlock.blockNumber();
    int newBreakpointLine = textCursor().blockNumber();

    // If previous breakpoint was here, remove it
    if (breakpointBlock.isValid() && oldBreakpointLine == newBreakpointLine)
    {
        breakpointBlock = QTextBlock(); // Invalidate breakpoint
    }
    else
    {
        // Create new breakpoint
        breakpointBlock = textCursor().block();
    }

    this->repaint();
}

void HidraCodeEditor::disableLineHighlight()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    setExtraSelections(extraSelections);
}

void HidraCodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(240, 240, 240)); // Light gray

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    // Iterate over all visible text blocks (lines)
    while (block.isValid() && top <= event->rect().bottom())
    {
        // If on the same block as breakpoint block
        if (blockNumber == breakpointBlock.blockNumber())
        {
            // If block is breakpoint's block, paint number area
            if (block == breakpointBlock)
            {
                painter.fillRect(0, top, lineNumberArea->width(), bottom - top, QColor(255, 64, 64)); // Red
            }
            else
            {
                // Invalidate block
                breakpointBlock = QTextBlock();
            }
        }

        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(128, 128, 128)); // Dark gray
            painter.drawText(0, top, lineNumberArea->width() - 1, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

LineNumberArea::LineNumberArea(HidraCodeEditor *editor) : QWidget(editor)
{
    codeEditor = editor;
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    codeEditor->lineNumberAreaPaintEvent(event);
}
