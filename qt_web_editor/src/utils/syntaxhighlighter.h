/**
 * @file syntaxhighlighter.h
 * @brief Declaration of the SyntaxHighlighter class.
 * 
 * This file contains the SyntaxHighlighter class which provides syntax highlighting
 * for various programming languages in the editor widget.
 */

#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

/**
 * @brief The SyntaxHighlighter class provides syntax highlighting for source code.
 * 
 * This class implements syntax highlighting for multiple programming languages
 * including HTML, CSS, and JavaScript. It uses a rule-based system to apply
 * different text formats to various syntax elements like keywords, strings, and comments.
 */
class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a SyntaxHighlighter with the given parent document.
     * 
     * @param parent The parent QTextDocument that will be highlighted.
     */
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);
    
    /**
     * @brief Sets the programming language for syntax highlighting.
     * 
     * Configures the highlighter with the appropriate rules for the specified language.
     * 
     * @param language The language to set (e.g., "html", "css", "javascript").
     */
    void setLanguage(const QString &language);
    
protected:
    /**
     * @brief Highlights the given text block according to the current syntax rules.
     * 
     * This method is called by Qt's syntax highlighting system for each block of text
     * that needs to be highlighted.
     * 
     * @param text The text to be highlighted.
     */
    void highlightBlock(const QString &text) override;
    
    /**
     * @brief Structure defining a single highlighting rule.
     * 
     * Combines a regular expression pattern with a text format to be applied
     * when the pattern is matched in the text.
     */
    struct HighlightingRule
    {
        QRegularExpression pattern;  /**< Regular expression to match in the text */
        QTextCharFormat format;      /**< Format to apply to matching text */
    };
    
    /**
     * @brief Adds a new highlighting rule with the given pattern and format.
     * 
     * @param pattern The regular expression pattern to match.
     * @param format The text format to apply to matching text.
     */
    void addRule(const QString &pattern, const QTextCharFormat &format);
    
    /**
     * @brief Adds multiple keywords that share the same text format.
     * 
     * @param keywords List of keywords to highlight.
     * @param format The text format to apply to all keywords.
     */
    void addKeywords(const QStringList &keywords, const QTextCharFormat &format);
    
    /**
     * @brief Sets up highlighting rules for HTML documents.
     */
    void setupHtmlRules();
    
    /**
     * @brief Sets up highlighting rules for CSS stylesheets.
     */
    void setupCssRules();
    
    /**
     * @brief Sets up highlighting rules for JavaScript code.
     */
    void setupJsRules();
    
    // Text formats for different syntax elements
    QTextCharFormat m_keywordFormat;     /**< Format for language keywords */
    QTextCharFormat m_tagFormat;         /**< Format for HTML/XML tags */
    QTextCharFormat m_attributeFormat;   /**< Format for HTML/XML attributes */
    QTextCharFormat m_valueFormat;       /**< Format for attribute values */
    QTextCharFormat m_commentFormat;     /**< Format for comments */
    QTextCharFormat m_stringFormat;      /**< Format for string literals */
    QTextCharFormat m_numberFormat;      /**< Format for numeric literals */
    QTextCharFormat m_functionFormat;    /**< Format for function names */
    
    QVector<HighlightingRule> m_rules;   /**< List of all active highlighting rules */
    
    QRegularExpression m_commentStartExpression;  /**< Pattern for the start of multi-line comments */
    QRegularExpression m_commentEndExpression;    /**< Pattern for the end of multi-line comments */
    
    QString m_language;  /**< Currently selected language for syntax highlighting */
};

#endif // SYNTAXHIGHLIGHTER_H