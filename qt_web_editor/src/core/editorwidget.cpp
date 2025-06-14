#include "editorwidget.h"
#include "../utils/syntaxhighlighter.h"
#include "application.h"
#include "settings.h"

#include <QPainter>
#include <QTextBlock>
#include <QTextStream>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QScrollBar>
#include <QRegularExpression>
#include <QStringConverter>

EditorWidget::EditorWidget(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(new LineNumberArea(this))
{
    setupEditor();
    setupConnections();
    
    // Set default font
    setFont(Application::instance()->settings()->editorFont());
    
    // Set tab stop width (4 spaces)
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    
    // Set line wrap mode
    setLineWrapMode(QPlainTextEdit::NoWrap);
    
    // Set up line number area
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

EditorWidget::~EditorWidget()
{
    delete m_lineNumberArea;
}

void EditorWidget::setupEditor()
{
    // Set up the editor
    setFrameStyle(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Set up the document
    QTextDocument *doc = document();
    doc->setDocumentMargin(0);
    
    // Set up the viewport margins to make room for line numbers
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void EditorWidget::setupConnections()
{
    // Connect document changes
    connect(document(), &QTextDocument::modificationChanged, 
            this, &EditorWidget::modificationChanged);
    
    // Connect cursor position changes
    connect(this, &EditorWidget::cursorPositionChanged, 
            this, &EditorWidget::highlightCurrentLine);
    
    // Connect text changes
    connect(this, &EditorWidget::textChanged, 
            this, &EditorWidget::updateExtraSelections);
    
    // Connect block count changes
    connect(document(), &QTextDocument::blockCountChanged, 
            this, &EditorWidget::updateLineNumberAreaWidth);
    
    // Connect update request
    connect(this, &EditorWidget::updateRequest, 
            this, &EditorWidget::updateLineNumberArea);
}

bool EditorWidget::load(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    
    // Set the encoding to UTF-8
    in.setEncoding(QStringConverter::Utf8);
    
    // Read the content
    QString content = in.readAll();
    
    // Set the content
    setPlainText(content);
    
    // Set the file path and name
    setFilePath(filePath);
    
    // Set syntax highlighting
    setSyntaxForFile(filePath);
    
    // Mark as unmodified
    document()->setModified(false);
    
    return true;
}

bool EditorWidget::save()
{
    if (m_filePath.isEmpty()) {
        return false;
    }
    
    return saveAs(m_filePath);
}

bool EditorWidget::saveAs(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << toPlainText();
    
    bool success = file.flush();
    file.close();
    
    if (success) {
        setFilePath(filePath);
        document()->setModified(false);
        return true;
    }
    
    return false;
}

void EditorWidget::setFilePath(const QString &filePath)
{
    if (m_filePath != filePath) {
        m_filePath = filePath;
        m_fileName = QFileInfo(filePath).fileName();
        
        // Update syntax highlighting based on file extension
        setSyntaxForFile(filePath);
        
        emit filePathChanged(filePath);
    }
}

void EditorWidget::setSyntaxForFile(const QString &filePath)
{
    // Syntax highlighting is disabled for now
    Q_UNUSED(filePath);
    
    // Remove existing highlighter if any
    if (m_highlighter) {
        m_highlighter->deleteLater();
        m_highlighter = nullptr;
    }
}

void EditorWidget::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void EditorWidget::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        
        QColor lineColor = QColor(QRgb(0xE8F2FE));
        if (Application::instance()->settings()->theme() == Settings::Theme::Dark) {
            lineColor = QColor(QRgb(0x2A2A2A));
        }
        
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    
    setExtraSelections(extraSelections);
}

void EditorWidget::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }
    
    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void EditorWidget::updateExtraSelections()
{
    // This can be used for additional syntax highlighting, like matching brackets, etc.
    // For now, we'll just update the current line highlight
    highlightCurrentLine();
}

void EditorWidget::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    
    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void EditorWidget::keyPressEvent(QKeyEvent *e)
{
    // Handle tab key for indentation
    if (e->key() == Qt::Key_Tab) {
        if (textCursor().hasSelection()) {
            // Indent selected lines
            QTextCursor cursor = textCursor();
            int start = cursor.selectionStart();
            int end = cursor.selectionEnd();
            
            cursor.setPosition(start);
            int startBlock = cursor.blockNumber();
            
            cursor.setPosition(end, QTextCursor::KeepAnchor);
            int endBlock = cursor.blockNumber();
            
            cursor.beginEditBlock();
            
            cursor.setPosition(start);
            cursor.movePosition(QTextCursor::StartOfLine);
            
            for (int i = 0; i <= (endBlock - startBlock); ++i) {
                cursor.insertText("\t");
                cursor.movePosition(QTextCursor::NextBlock);
            }
            
            cursor.endEditBlock();
            
            // Reselect the text
            cursor.setPosition(start);
            cursor.movePosition(QTextCursor::StartOfLine);
            
            int numSelected = end - start + (endBlock - startBlock + 1);
            
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, numSelected);
            setTextCursor(cursor);
            
            return;
        } else {
            // Insert tab or spaces
            if (Application::instance()->settings()->useSpacesForTabs()) {
                int spaces = Application::instance()->settings()->tabSize();
                insertPlainText(QString(spaces, ' '));
            } else {
                insertPlainText("\t");
            }
            return;
        }
    }
    
    // Handle Shift+Tab for unindent
    if (e->key() == Qt::Key_Backtab) {
        QTextCursor cursor = textCursor();
        
        if (cursor.hasSelection()) {
            int start = cursor.selectionStart();
            int end = cursor.selectionEnd();
            
            cursor.setPosition(start);
            int startBlock = cursor.blockNumber();
            
            cursor.setPosition(end, QTextCursor::KeepAnchor);
            int endBlock = cursor.blockNumber();
            
            cursor.beginEditBlock();
            
            cursor.setPosition(start);
            cursor.movePosition(QTextCursor::StartOfLine);
            
            for (int i = 0; i <= (endBlock - startBlock); ++i) {
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                
                if (cursor.selectedText() == "\t") {
                    cursor.removeSelectedText();
                } else {
                    cursor.clearSelection();
                    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                    
                    if (cursor.selectedText() == " ") {
                        cursor.removeSelectedText();
                        
                        // Remove additional spaces if needed
                        int spaces = Application::instance()->settings()->tabSize() - 1;
                        for (int j = 0; j < spaces; ++j) {
                            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
                            if (cursor.selectedText() != " ") {
                                break;
                            }
                            cursor.removeSelectedText();
                        }
                    }
                }
                
                cursor.movePosition(QTextCursor::NextBlock);
            }
            
            cursor.endEditBlock();
            
            // Reselect the text
            cursor.setPosition(start);
            cursor.movePosition(QTextCursor::StartOfLine);
            
            int numSelected = end - start - (endBlock - startBlock + 1);
            
            if (numSelected > 0) {
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, numSelected);
                setTextCursor(cursor);
            }
            
            return;
        }
    }
    
    // Handle Enter key for auto-indentation
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        QTextCursor cursor = textCursor();
        int pos = cursor.position();
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        
        QString line = cursor.selectedText();
        int leadingSpaces = 0;
        
        // Count leading spaces or tabs
        while (leadingSpaces < line.length() && 
              (line[leadingSpaces] == ' ' || line[leadingSpaces] == '\t')) {
            leadingSpaces++;
        }
        
        // Insert newline and indent
        cursor.setPosition(pos);
        cursor.insertText("\n" + line.left(leadingSpaces));
        
        // Auto-indent for opening braces
        if (line.trimmed().endsWith('{')) {
            cursor.insertText("\t");
        }
        
        return;
    }
    
    // Handle backspace at the beginning of a line to match previous line's indentation
    if (e->key() == Qt::Key_Backspace) {
        QTextCursor cursor = textCursor();
        
        if (!cursor.hasSelection() && cursor.positionInBlock() == 0) {
            // At the beginning of a line, check previous line's indentation
            cursor.movePosition(QTextCursor::PreviousBlock);
            cursor.movePosition(QTextCursor::EndOfLine);
            
            QString prevLine = cursor.block().text();
            int leadingSpaces = 0;
            
            while (leadingSpaces < prevLine.length() && 
                  (prevLine[leadingSpaces] == ' ' || prevLine[leadingSpaces] == '\t')) {
                leadingSpaces++;
            }
            
            // If previous line ends with an opening brace, reduce indentation
            if (prevLine.trimmed().endsWith('{')) {
                int tabSize = Application::instance()->settings()->tabSize();
                int spacesToRemove = Application::instance()->settings()->useSpacesForTabs() ? tabSize : 1;
                
                cursor.movePosition(QTextCursor::NextCharacter);
                
                for (int i = 0; i < spacesToRemove && i < leadingSpaces; ++i) {
                    cursor.deletePreviousChar();
                }
                
                return;
            }
        }
    }
    
    // Default behavior for other keys
    QPlainTextEdit::keyPressEvent(e);
}

void EditorWidget::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(240, 240, 240));
    
    if (Application::instance()->settings()->theme() == Settings::Theme::Dark) {
        painter.fillRect(event->rect(), QColor(45, 45, 45));
    }
    
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int)blockBoundingRect(block).height();
    
    QFont font = this->font();
    font.setPointSize(font.pointSize() - 1);
    painter.setFont(font);
    
    QColor textColor = palette().text().color();
    if (Application::instance()->settings()->theme() == Settings::Theme::Dark) {
        textColor = QColor(150, 150, 150);
    }
    
    painter.setPen(textColor);
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, m_lineNumberArea->width() - 5, fontMetrics().height(),
                            Qt::AlignRight, number);
        }
        
        block = block.next();
        top = bottom;
        bottom = top + (int)blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int EditorWidget::lineNumberAreaWidth() const
{
    if (!Application::instance()->settings()->lineNumbers()) {
        return 0;
    }
    
    int digits = 1;
    int max = qMax(1, blockCount());
    
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    
    int space = 13 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    
    return space;
}
