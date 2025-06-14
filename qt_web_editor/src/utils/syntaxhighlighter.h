#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);
    
protected:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    
    QVector<HighlightingRule> highlightingRules;
    
    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat tagFormat;
    QTextCharFormat attributeFormat;
    QTextCharFormat valueFormat;
    
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    
    virtual void highlightBlock(const QString &text) override;
    virtual void setupRules();
    
private:
    void setupFormats();
};

class HtmlSyntaxHighlighter : public SyntaxHighlighter
{
    Q_OBJECT
    
public:
    explicit HtmlSyntaxHighlighter(QTextDocument *parent = nullptr);
    
protected:
    void setupRules() override;
};

class CssSyntaxHighlighter : public SyntaxHighlighter
{
    Q_OBJECT
    
public:
    explicit CssSyntaxHighlighter(QTextDocument *parent = nullptr);
    
protected:
    void setupRules() override;
};

class JsSyntaxHighlighter : public SyntaxHighlighter
{
    Q_OBJECT
    
public:
    explicit JsSyntaxHighlighter(QTextDocument *parent = nullptr);
    
protected:
    void setupRules() override;
};

class JsonSyntaxHighlighter : public SyntaxHighlighter
{
    Q_OBJECT
    
public:
    explicit JsonSyntaxHighlighter(QTextDocument *parent = nullptr);
    
protected:
    void setupRules() override;
};

class XmlSyntaxHighlighter : public SyntaxHighlighter
{
    Q_OBJECT
    
public:
    explicit XmlSyntaxHighlighter(QTextDocument *parent = nullptr);
    
protected:
    void setupRules() override;
};

class MarkdownSyntaxHighlighter : public SyntaxHighlighter
{
    Q_OBJECT
    
public:
    explicit MarkdownSyntaxHighlighter(QTextDocument *parent = nullptr);
    
protected:
    void setupRules() override;
};

#endif // SYNTAXHIGHLIGHTER_H
