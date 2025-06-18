/**
 * @file editorwidget.h
 * @brief Defines the EditorWidget class which provides a code editing widget with line numbers and syntax highlighting.
 * 
 * This widget extends QPlainTextEdit to provide features like line numbers, syntax highlighting,
 * and file I/O operations specifically designed for code editing.
 */

#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QPlainTextEdit>
#include <QPointer>
#include <QTimer>

// Forward declarations
class QSyntaxHighlighter;
class SyntaxHighlighter;

/**
 * @class EditorWidget
 * @brief A text editing widget with features for code editing.
 * 
 * This class provides a code editor with syntax highlighting, line numbers, and file I/O capabilities.
 * It's designed to be used as the main editing component in the application.
 */
class EditorWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an EditorWidget with the given parent.
     * @param parent The parent widget.
     */
    explicit EditorWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Destroys the EditorWidget and cleans up resources.
     */
    ~EditorWidget() override;

    // File operations
    /**
     * @brief Loads content from a file into the editor.
     * @param filePath Path to the file to load.
     * @return true if the file was loaded successfully, false otherwise.
     */
    bool load(const QString &filePath);
    
    /**
     * @brief Saves the current content to the current file.
     * @return true if the save was successful, false otherwise.
     * @note If no file is associated, this will return false.
     */
    bool save();
    
    /**
     * @brief Saves the content to a specified file path.
     * @param filePath The path where to save the file.
     * @return true if the save was successful, false otherwise.
     */
    bool saveAs(const QString &filePath);
    
    /**
     * @brief Gets the current file path.
     * @return The path of the currently open file, or an empty string if none.
     */
    QString filePath() const { return m_filePath; }
    
    /**
     * @brief Checks if the document has been modified since the last save.
     * @return true if the document has been modified, false otherwise.
     */
    bool isModified() const { return document()->isModified(); }
    
    /**
     * @brief Sets the file path and updates related properties.
     * @param filePath The new file path to set.
     */
    void setFilePath(const QString &filePath);
    
    /**
     * @brief Configures syntax highlighting based on the file extension.
     * @param filePath The file path used to determine the syntax rules.
     */
    void setSyntaxForFile(const QString &filePath);

signals:
    /**
     * @brief Emitted when the modified state of the document changes.
     * @param modified The new modified state.
     */
    void fileModified(bool modified);
    
    /**
     * @brief Emitted when the file path changes.
     * @param filePath The new file path.
     */
    void filePathChanged(const QString &filePath);
    
    /**
     * @brief Emitted when the file name changes.
     * @param fileName The new file name.
     */
    void fileNameChanged(const QString &fileName);
    
    /**
     * @brief Emitted when the content of the document changes.
     */
    void contentChanged();

public slots:
    /**
     * @brief Updates the width of the line number area.
     * @param newBlockCount The new number of blocks (unused, kept for signal compatibility).
     */
    void updateLineNumberAreaWidth(int newBlockCount);
    
    /**
     * @brief Highlights the current line in the editor.
     */
    void highlightCurrentLine();
    
    /**
     * @brief Updates the line number area when the editor is scrolled.
     * @param rect The area that needs to be updated.
     * @param dy The vertical scroll amount.
     */
    void updateLineNumberArea(const QRect &rect, int dy);
    
    /**
     * @brief Updates extra selections like current line highlight.
     */
    void updateExtraSelections();
    
protected:
    /**
     * @brief Handles resize events to update the line number area.
     * @param event The resize event.
     */
    void resizeEvent(QResizeEvent *event) override;
    
    /**
     * @brief Handles key press events for custom key bindings.
     * @param e The key event.
     */
    void keyPressEvent(QKeyEvent *e) override;
    
private:
    /**
     * @brief The LineNumberArea class provides the line number area for the editor.
     */
    class LineNumberArea;
    
    LineNumberArea *m_lineNumberArea;  ///< Widget that displays line numbers
    QPointer<SyntaxHighlighter> m_highlighter;  ///< Syntax highlighter instance
    QString m_filePath;  ///< Current file path
    QString m_fileName;  ///< Current file name
    QTimer *m_updateTimer;  ///< Timer for delayed updates
    
    /**
     * @brief Initializes editor settings and appearance.
     */
    void setupEditor();
    
    /**
     * @brief Sets up signal-slot connections.
     */
    void setupConnections();
    
    /**
     * @brief Updates the geometry of the line number area.
     */
    void updateLineNumberAreaGeometry();
    
    /**
     * @brief Calculates the required width for the line number area.
     * @return The required width in pixels.
     */
    int lineNumberAreaWidth() const;
    
    /**
     * @brief Handles painting of the line number area.
     * @param event The paint event.
     */
    void lineNumberAreaPaintEvent(QPaintEvent *event);
};

/**
 * @class EditorWidget::LineNumberArea
 * @brief A widget that displays line numbers for the editor.
 */
class EditorWidget::LineNumberArea : public QWidget
{
public:
    /**
     * @brief Constructs a LineNumberArea for the given editor.
     * @param editor The parent editor widget.
     */
    explicit LineNumberArea(EditorWidget *editor) : QWidget(editor), m_editor(editor) {}
    
    /**
     * @brief Returns the preferred size of the line number area.
     * @return A size with width based on line count and 0 height.
     */
    QSize sizeHint() const override {
        return QSize(m_editor->lineNumberAreaWidth(), 0);
    }
    
protected:
    /**
     * @brief Handles paint events by delegating to the editor.
     * @param event The paint event.
     */
    void paintEvent(QPaintEvent *event) override {
        m_editor->lineNumberAreaPaintEvent(event);
    }
    
private:
    EditorWidget *m_editor;  ///< The editor this line number area belongs to
};

#endif // EDITORWIDGET_H
