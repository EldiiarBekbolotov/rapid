#ifndef EDITORWIDGET_H
#define EDITORWIDGET_H

#include <QPlainTextEdit>
#include <QPointer>

class QSyntaxHighlighter;

class EditorWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit EditorWidget(QWidget *parent = nullptr);
    ~EditorWidget() override;

    // File operations
    bool load(const QString &filePath);
    bool save();
    bool saveAs(const QString &filePath);
    
    // Getters
    QString filePath() const { return m_filePath; }
    bool isModified() const { return document()->isModified(); }
    
    // Setters
    void setFilePath(const QString &filePath);
    
    // Syntax highlighting
    void setSyntaxForFile(const QString &filePath);

signals:
    void filePathChanged(const QString &filePath);

public slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void updateExtraSelections();
    
protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
    
private:
    class LineNumberArea;
    
    LineNumberArea *m_lineNumberArea;
    QPointer<QSyntaxHighlighter> m_highlighter;
    QString m_filePath;
    QString m_fileName;
    
    void setupEditor();
    void setupConnections();
    void updateLineNumberAreaGeometry();
    int lineNumberAreaWidth() const;
    void lineNumberAreaPaintEvent(QPaintEvent *event);
};

class EditorWidget::LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(EditorWidget *editor) : QWidget(editor), m_editor(editor) {}
    
    QSize sizeHint() const override {
        return QSize(m_editor->lineNumberAreaWidth(), 0);
    }
    
protected:
    void paintEvent(QPaintEvent *event) override {
        m_editor->lineNumberAreaPaintEvent(event);
    }
    
private:
    EditorWidget *m_editor;
};

#endif // EDITORWIDGET_H
