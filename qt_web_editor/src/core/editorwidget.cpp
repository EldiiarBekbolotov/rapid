/**
 * @file editorwidget.cpp
 * @brief Implementation of the EditorWidget class.
 * 
 * This file contains the implementation of the EditorWidget class which provides
 * a code editing widget with line numbers, syntax highlighting, and file I/O capabilities.
 */

#include "editorwidget.h"
#include "../utils/syntaxhighlighter.h"
#include "application.h"
#include "settings.h"

#include <QDir>
#include <QFileInfo>
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

/**
 * @brief Constructs an EditorWidget with the given parent.
 * 
 * Initializes the editor with default settings, sets up the line number area,
 * and configures the auto-update timer for content changes.
 * 
 * @param parent The parent widget.
 */
EditorWidget::EditorWidget(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(new LineNumberArea(this))
    , m_updateTimer(new QTimer(this))
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
    
    // Set up auto-update timer for preview
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(500); // 500ms delay after typing stops
    connect(m_updateTimer, &QTimer::timeout, this, [this]() {
        emit contentChanged();
    });
    
    // Connect text changes to the timer
    connect(document(), &QTextDocument::contentsChanged, this, [this]() {
        m_updateTimer->start();
    });
}

/**
 * @brief Destroys the EditorWidget and cleans up resources.
 * 
 * Ensures proper cleanup of the line number area and other resources.
 */
EditorWidget::~EditorWidget()
{
    delete m_lineNumberArea;
}

/**
 * @brief Initializes the editor's appearance and basic settings.
 * 
 * Configures the editor's frame style, scroll bars, document margins,
 * and viewport margins to accommodate the line number area.
 */
void EditorWidget::setupEditor()
{
    // Set up the editor's frame and scroll bars
    setFrameStyle(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Configure the document
    QTextDocument *doc = document();
    doc->setDocumentMargin(0);
    
    // Adjust viewport margins to make room for line numbers
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

/**
 * @brief Sets up signal-slot connections for the editor.
 * 
 * Establishes connections between editor events and their corresponding
 * handler methods to manage document changes, cursor movements, and UI updates.
 */
void EditorWidget::setupConnections()
{
    // Connect document modification state changes
    connect(document(), &QTextDocument::modificationChanged, 
            this, &EditorWidget::modificationChanged);
    
    // Update current line highlight when cursor moves
    connect(this, &EditorWidget::cursorPositionChanged, 
            this, &EditorWidget::highlightCurrentLine);
    
    // Update extra selections (like search results) when text changes
    connect(this, &EditorWidget::textChanged, 
            this, &EditorWidget::updateExtraSelections);
    
    // Adjust line number area width when block count changes
    connect(document(), &QTextDocument::blockCountChanged, 
            this, &EditorWidget::updateLineNumberAreaWidth);
    
    // Update line number area when scrolled or resized
    connect(this, &EditorWidget::updateRequest, 
            this, &EditorWidget::updateLineNumberArea);
}

/**
 * @brief Loads content from a file into the editor.
 * 
 * Attempts to open and read the specified file, loading its contents into the editor.
 * Sets up appropriate syntax highlighting based on the file extension.
 * 
 * @param filePath Path to the file to load.
 * @return true if the file was loaded successfully, false otherwise.
 */
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

/**
 * @brief Saves the current content to the current file.
 * 
 * If no file is currently associated with the editor, does nothing.
 * 
 * @return true if the save was successful, false otherwise.
 * @see saveAs()
 */
bool EditorWidget::save()
{
    if (m_filePath.isEmpty()) {
        return false;
    }
    return saveAs(m_filePath);
}

/**
 * @brief Saves the current content to the specified file.
 * 
 * Saves the editor's content to the given file path and updates the editor's
 * state accordingly. Handles file I/O errors and provides user feedback.
 * 
 * @param filePath The path where to save the file.
 * @return true if the save was successful, false otherwise.
 */
bool EditorWidget::saveAs(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), 
                           tr("Could not save file %1: %2")
                           .arg(QDir::toNativeSeparators(filePath), file.errorString()));
        return false;
    }

    // Write content to file with UTF-8 encoding
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << toPlainText();
    file.close();

    // Update the file path and mark as unmodified
    setFilePath(filePath);
    document()->setModified(false);
    
    return true;
}

/**
 * @brief Sets the file path associated with the editor.
 * 
 * Updates the editor's state with the new file path and name, and adjusts
 * syntax highlighting based on the file's extension.
 * 
 * @param filePath The new file path to associate with this editor.
 */
void EditorWidget::setFilePath(const QString &filePath)
{
    if (m_filePath != filePath) {
        m_filePath = filePath;
        m_fileName = QFileInfo(filePath).fileName();
        emit filePathChanged(m_filePath);
        emit fileNameChanged(m_fileName);
        
        // Configure syntax highlighting based on file extension
        setSyntaxForFile(filePath);
    }
}

/**
 * @brief Configures syntax highlighting based on the file extension.
 * 
 * Creates or updates the syntax highlighter with rules appropriate for the
 * file type indicated by the given file path's extension.
 * 
 * @param filePath The file path used to determine the syntax rules.
 */
void EditorWidget::setSyntaxForFile(const QString &filePath)
{
    // Clear existing highlighter if no file is specified
    if (filePath.isEmpty()) {
        if (m_highlighter) {
            m_highlighter->deleteLater();
            m_highlighter = nullptr;
        }
        return;
    }
    
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    
    // Create or update syntax highlighter
    if (!m_highlighter) {
        m_highlighter = new SyntaxHighlighter(document());
    }
    
    // Set syntax based on file extension
    if (suffix == "cpp" || suffix == "h" || suffix == "hpp" || suffix == "cxx" || suffix == "cc") {
        m_highlighter->setLanguage("cpp");
    } else if (suffix == "js") {
        m_highlighter->setLanguage("javascript");
    } else if (suffix == "html" || suffix == "htm") {
        m_highlighter->setLanguage("html");
    } else if (suffix == "css") {
        m_highlighter->setLanguage("css");
    } else {
        // Default to no syntax highlighting
        m_highlighter->setLanguage("");
    }
}

/**
 * @brief Updates the width of the line number area.
 * 
 * Called when the number of blocks in the document changes to ensure the
 * line number area has sufficient width to display all line numbers.
 * 
 * @param newBlockCount The new number of blocks (unused parameter).
 */
void EditorWidget::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    // Adjust viewport margins to accommodate the line number area
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

/**
 * @brief Highlights the current line in the editor.
 * 
 * Applies a subtle background highlight to the line containing the cursor
 * to improve text editing visibility. The highlight is only applied when
 * the editor is not in read-only mode.
 */
void EditorWidget::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        // Use a subtle yellow highlight that works in both light and dark themes
        QColor lineColor = QColor(Qt::yellow).lighter(160);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

/**
 * @brief Updates the line number area when the editor is scrolled or resized.
 * 
 * Ensures the line number area remains synchronized with the editor's content
 * during scrolling or resizing operations.
 * 
 * @param rect The area of the editor that needs updating.
 * @param dy The amount of vertical scrolling that occurred (0 if no scrolling).
 */
void EditorWidget::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        m_lineNumberArea->scroll(0, dy);
    } else {
        // If not scrolling, just update the line number area
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }

    // Update the viewport margins if needed
    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

/**
 * @brief Updates the extra selections in the editor.
 * 
 * This method is responsible for applying extra text formatting such as
 * current line highlighting and search result highlighting.
 */
void EditorWidget::updateExtraSelections()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    
    // Highlight current line
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        QColor lineColor = QColor(Qt::yellow).lighter(160);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    
    // Apply the extra selections
    setExtraSelections(extraSelections);
}

/**
 * @brief Handles resize events for the editor widget.
 * 
 * Overrides the base class implementation to ensure the line number area
 * is properly resized along with the editor.
 * 
 * @param event The resize event containing size information.
 */
void EditorWidget::resizeEvent(QResizeEvent *event)
{
    // Call base class implementation first
    QPlainTextEdit::resizeEvent(event);
    
    // Update line number area geometry to match new editor size
    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

/**
 * @brief Handles key press events for the editor widget.
 * 
 * Provides custom handling for certain keys, such as Tab, Shift+Tab, Enter,
 * and Backspace, to implement features like auto-indentation and line wrapping.
 * 
 * @param e The key event containing the pressed key information.
 */
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

/**
 * @brief Handles painting of the line number area.
 * 
 * Renders line numbers in the line number area, ensuring they align with
 * the corresponding lines in the editor. Only visible line numbers are drawn
 * for performance reasons.
 * 
 * @param event The paint event containing the area to be painted.
 */
void EditorWidget::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    // Set up painter with line number area background
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), QColor(240, 240, 240));
    
    if (Application::instance()->settings()->theme() == Settings::Theme::Dark) {
        painter.fillRect(event->rect(), QColor(45, 45, 45));
    }
    
    // Get the first visible block and its geometry
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    
    // Use the same font as the editor for consistency
    QFont font = this->font();
    painter.setFont(font);
    
    // Draw line numbers for all visible blocks
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            // Convert to 1-based line numbers for display
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            // Draw the line number right-aligned with some right padding
            painter.drawText(0, top, m_lineNumberArea->width() - 3, fontMetrics().height(),
                           Qt::AlignRight, number);
        }
        
        // Move to next block
        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

/**
 * @brief Calculates the required width for the line number area.
 * 
 * Determines the necessary width to display all line numbers based on
 * the current font metrics and the number of lines in the document.
 * 
 * @return The calculated width in pixels for the line number area.
 */
int EditorWidget::lineNumberAreaWidth() const
{
    if (!Application::instance()->settings()->lineNumbers()) {
        return 0;
    }
    
    // Calculate number of digits needed for the highest line number
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    
    // Calculate width needed for the digits plus some padding
    int space = 13 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    
    return space;
}
