#include "syntaxhighlighter.h"
#include <QRegularExpression>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    setupFormats();
    setupRules();
    
    // Initialize comment expressions
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void SyntaxHighlighter::setupFormats()
{
    // Keyword format
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    // Class format
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);

    // Single-line comment
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    singleLineCommentFormat.setFontItalic(true);

    // Multi-line comment
    multiLineCommentFormat.setForeground(Qt::darkGreen);
    multiLineCommentFormat.setFontItalic(true);

    // Quotation
    quotationFormat.setForeground(Qt::darkGreen);

    // Function/method
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);

    // Numbers
    numberFormat.setForeground(Qt::darkMagenta);

    // HTML/XML tags
    tagFormat.setForeground(Qt::darkBlue);
    tagFormat.setFontWeight(QFont::Bold);
}

void SyntaxHighlighter::setupRules()
{
    // Default implementation - can be overridden by derived classes
    // This is a basic set of rules for C++ like syntax
    
    // Keywords
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b";

    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Class names
    HighlightingRule classRule;
    classRule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    classRule.format = classFormat;
    highlightingRules.append(classRule);

    // Single line comments
    HighlightingRule singleLineCommentRule;
    singleLineCommentRule.pattern = QRegularExpression("//[^\n]*");
    singleLineCommentRule.format = singleLineCommentFormat;
    highlightingRules.append(singleLineCommentRule);

    // Multi-line comments
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");

    // Quotations
    HighlightingRule stringRule;
    stringRule.pattern = QRegularExpression("\"[^\n\"]*\"");
    stringRule.format = quotationFormat;
    highlightingRules.append(stringRule);

    // Numbers
    HighlightingRule numberRule;
    numberRule.pattern = QRegularExpression("\\b[0-9]+\\b");
    numberRule.format = numberFormat;
    highlightingRules.append(numberRule);
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply syntax highlighting to the given text block
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle multi-line comments
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(commentStartExpression);
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

// Minimal implementations for syntax highlighters
HtmlSyntaxHighlighter::HtmlSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent) {}

void HtmlSyntaxHighlighter::setupRules() {}

CssSyntaxHighlighter::CssSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent) {}

void CssSyntaxHighlighter::setupRules() {}

JsSyntaxHighlighter::JsSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent) {}

void JsSyntaxHighlighter::setupRules() {}

JsonSyntaxHighlighter::JsonSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent) {}

void JsonSyntaxHighlighter::setupRules() {}

XmlSyntaxHighlighter::XmlSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent) {}

void XmlSyntaxHighlighter::setupRules() {}

MarkdownSyntaxHighlighter::MarkdownSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent) {}

void MarkdownSyntaxHighlighter::setupRules() {}