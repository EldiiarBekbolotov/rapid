#include "syntaxhighlighter.h"
#include <QDebug>
#include <QRegularExpression>

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

void SyntaxHighlighter::addRule(const QString &pattern, const QTextCharFormat &format)
{
    HighlightingRule rule;
    rule.pattern = QRegularExpression(pattern);
    rule.format = format;
    m_rules.append(rule);
}

void SyntaxHighlighter::addKeywords(const QStringList &keywords, const QTextCharFormat &format)
{
    for (const QString &keyword : keywords) {
        addRule(QStringLiteral("\\b%1\\b").arg(keyword), format);
    }
}

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
