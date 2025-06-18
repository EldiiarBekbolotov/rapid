/**
 * @file syntaxhighlighter.cpp
 * @brief Implementation of the SyntaxHighlighter class.
 * 
 * This file contains the implementation of syntax highlighting functionality
 * for various programming languages in the editor.
 */

#include "syntaxhighlighter.h"
#include <QDebug>
#include <QRegularExpression>

/**
 * @brief Constructs a SyntaxHighlighter with the given parent document.
 * 
 * Initializes text formats for different syntax elements with default styles.
 * 
 * @param parent The parent QTextDocument that will be highlighted.
 */
SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // Setup text formats
    m_keywordFormat.setForeground(Qt::darkBlue);
    m_keywordFormat.setFontWeight(QFont::Bold);

    m_tagFormat.setForeground(Qt::darkBlue);
    m_tagFormat.setFontWeight(QFont::Bold);

    m_attributeFormat.setForeground(Qt::darkRed);
    m_valueFormat.setForeground(Qt::darkGreen);
    
    m_commentFormat.setForeground(Qt::darkGreen);
    m_commentFormat.setFontItalic(true);
    
    m_stringFormat.setForeground(Qt::darkGreen);
    m_numberFormat.setForeground(Qt::darkMagenta);
    m_functionFormat.setForeground(Qt::blue);
    m_functionFormat.setFontItalic(true);
}

/**
 * @brief Sets the programming language for syntax highlighting.
 * 
 * Configures the highlighter with the appropriate rules for the specified language
 * and triggers rehighlighting of the document.
 * 
 * @param language The language to set (e.g., "html", "css", "javascript").
 */
void SyntaxHighlighter::setLanguage(const QString &language)
{
    m_language = language.toLower();
    m_rules.clear();
    
    if (m_language == "html") {
        setupHtmlRules();
    } else if (m_language == "css") {
        setupCssRules();
    } else if (m_language == "javascript" || m_language == "js") {
        setupJsRules();
    }
    
    rehighlight();
}

/**
 * @brief Highlights the given text block according to the current syntax rules.
 * 
 * This method is called by Qt's syntax highlighting system for each block of text
 * that needs to be highlighted. It applies all registered highlighting rules
 * and handles multi-line comments.
 * 
 * @param text The text block to be highlighted.
 */
void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply syntax highlighting rules
    for (const HighlightingRule &rule : qAsConst(m_rules)) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    
    // Handle multi-line comments
    setCurrentBlockState(0);
    
    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(m_commentStartExpression);
    }
    
    while (startIndex >= 0) {
        QRegularExpressionMatch match = m_commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength;
        
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        
        setFormat(startIndex, commentLength, m_commentFormat);
        startIndex = text.indexOf(m_commentStartExpression, startIndex + commentLength);
    }
}

/**
 * @brief Adds a new highlighting rule with the given pattern and format.
 * 
 * Creates a new highlighting rule that will be applied to all text matching
 * the specified regular expression pattern.
 * 
 * @param pattern The regular expression pattern to match.
 * @param format The text format to apply to matching text.
 */
void SyntaxHighlighter::addRule(const QString &pattern, const QTextCharFormat &format)
{
    HighlightingRule rule;
    rule.pattern = QRegularExpression(pattern);
    rule.format = format;
    m_rules.append(rule);
}

/**
 * @brief Adds multiple keywords that share the same text format.
 * 
 * Creates highlighting rules for each keyword in the list, ensuring they are
 * matched as whole words only.
 * 
 * @param keywords List of keywords to highlight.
 * @param format The text format to apply to all keywords.
 */
void SyntaxHighlighter::addKeywords(const QStringList &keywords, const QTextCharFormat &format)
{
    for (const QString &keyword : keywords) {
        addRule(QStringLiteral("\\b%1\\b").arg(keyword), format);
    }
}

/**
 * @brief Sets up highlighting rules for HTML documents.
 * 
 * Configures syntax highlighting rules specific to HTML, including:
 * - Tags
 * - Attributes
 * - Attribute values
 * - Comments
 * - DOCTYPE declarations
 */
void SyntaxHighlighter::setupHtmlRules()
{
    // HTML tags
    addRule(QStringLiteral("<\\/?[a-zA-Z0-9_:-]+"), m_tagFormat);
    
    // HTML attributes
    addRule(QStringLiteral("\\b[a-zA-Z0-9_:-]+(?=\\s*=\")"), m_attributeFormat);
    addRule(QStringLiteral("\\b[a-zA-Z0-9_:-]+(?=\\s*=')"), m_attributeFormat);
    addRule(QStringLiteral("\\b[a-zA-Z0-9_:-]+(?=\\s*=[^'\"])"), m_attributeFormat);
    
    // Attribute values
    addRule(QStringLiteral("\"[^\"]*\""), m_valueFormat);
    addRule(QStringLiteral("'[^']*'"), m_valueFormat);
    
    // Comments
    m_commentStartExpression = QRegularExpression("<!--");
    m_commentEndExpression = QRegularExpression("-->");
    
    // Special characters
    addRule("&[a-zA-Z0-9]+;", m_valueFormat);
    
    // DOCTYPE
    addRule("<!DOCTYPE[^>]*>", m_keywordFormat);
}

/**
 * @brief Sets up highlighting rules for CSS stylesheets.
 * 
 * Configures syntax highlighting rules specific to CSS, including:
 * - Properties
 * - Values
 * - Selectors
 * - At-rules
 * - Comments
 */
void SyntaxHighlighter::setupCssRules()
{
    // CSS properties
    addRule(QStringLiteral("\\b[a-zA-Z-]+\\s*:"), m_attributeFormat);
    
    // CSS selectors
    addRule(QStringLiteral("\\b([a-zA-Z0-9_][a-zA-Z0-9_>-]*)\\s*{"), m_tagFormat);
    
    // CSS values
    addRule(QStringLiteral("#[0-9a-fA-F]+"), m_valueFormat); // Hex colors
    addRule(QStringLiteral("\\b[0-9]+(\\.[0-9]+)?(px|em|%|in|cm|mm|pt|pc|ex|ch|rem|vh|vw|vmin|vmax)?\\b"), m_numberFormat);
    addRule(QStringLiteral("\\b(url|rgb|rgba|hsl|hsla|linear-gradient|radial-gradient)\\s*\\("), m_functionFormat);
    
    // CSS at-rules
    addRule(QStringLiteral("@\\w+"), m_keywordFormat);
    
    // CSS pseudo-classes and pseudo-elements
    addRule(QStringLiteral("(:|::)\\w[\\w-]*(?=\\s*[^{]*(?:{|$))"), m_functionFormat);
    
    // Comments
    m_commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    m_commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
    
    // Strings
    addRule(QStringLiteral("\"[^\"]*\""), m_stringFormat);
    addRule(QStringLiteral("'[^']*'"), m_stringFormat);
}

/**
 * @brief Sets up highlighting rules for JavaScript code.
 * 
 * Configures syntax highlighting rules specific to JavaScript, including:
 * - Keywords (if, else, function, etc.)
 * - Built-in objects and functions
 * - Numbers
 * - Strings
 * - Comments (both single and multi-line)
 */
void SyntaxHighlighter::setupJsRules()
{
    // JavaScript keywords
    QStringList keywords = {
        "break", "case", "catch", "class", "const", "continue",
        "debugger", "default", "delete", "do", "else", "export",
        "extends", "finally", "for", "function", "if", "import",
        "in", "instanceof", "new", "return", "super", "switch",
        "this", "throw", "try", "typeof", "var", "void",
        "while", "with", "yield"
    };
    addKeywords(keywords, m_keywordFormat);
    
    // JavaScript literals
    QStringList literals = {"true", "false", "null", "undefined", "NaN", "Infinity"};
    addKeywords(literals, m_valueFormat);
    
    // Functions
    addRule(QStringLiteral("\\b[a-zA-Z_][a-zA-Z0-9_]*\\s*\\("), m_functionFormat);
    
    // Numbers
    addRule(QStringLiteral("\\b[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?\\b"), m_numberFormat);
    
    // Strings
    addRule(QStringLiteral("\"[^\"]*\""), m_stringFormat);
    addRule(QStringLiteral("'[^']*'"), m_stringFormat);
    addRule(QStringLiteral("`[^`]*`"), m_stringFormat);
    
    // Comments
    addRule(QStringLiteral("//.*"), m_commentFormat);
    m_commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    m_commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}
