#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);
    
    // Set the document language (html, css, javascript)
    void setLanguage(const QString &language);
    
protected:
    // Highlight the given text block
    void highlightBlock(const QString &text) override;
    
    // Structure for syntax highlighting rules
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    
    // Add a highlighting rule
    void addRule(const QString &pattern, const QTextCharFormat &format);
    
    // Add multiple keywords with the same format
    void addKeywords(const QStringList &keywords, const QTextCharFormat &format);
    
    // Language-specific rule setup
    void setupHtmlRules();
    void setupCssRules();
    void setupJsRules();
    
    // Text formats for different syntax elements
    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_tagFormat;
    QTextCharFormat m_attributeFormat;
    QTextCharFormat m_valueFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_functionFormat;
    
    // List of highlighting rules
    QVector<HighlightingRule> m_rules;
    
    // For multi-line comments
    QRegularExpression m_commentStartExpression;
    QRegularExpression m_commentEndExpression;
    
    // Current language
    QString m_language;
};

#endif // SYNTAXHIGHLIGHTER_H