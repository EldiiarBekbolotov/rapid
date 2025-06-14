#include "settings.h"
#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings("QtWebEditor", "QtWebEditor", this))
{
}

Settings::~Settings()
{
    save();
}

void Settings::load()
{
    m_settings->beginGroup("Application");
    
    // Load theme
    QString themeName = m_settings->value("theme", "light").toString();
    setTheme(themeName);
    
    // Load font settings
    if (m_settings->contains("font")) {
        m_editorFont.fromString(m_settings->value("font").toString());
    } else {
        // Default font settings
        m_editorFont = QFont("Monospace", 10);
        m_editorFont.setStyleHint(QFont::TypeWriter);
    }
    
    m_settings->endGroup();
    
    m_settings->beginGroup("Editor");
    
    // Load editor settings
    m_wordWrap = m_settings->value("wordWrap", false).toBool();
    m_lineNumbers = m_settings->value("lineNumbers", true).toBool();
    m_tabSize = m_settings->value("tabSize", 4).toInt();
    m_useSpacesForTabs = m_settings->value("useSpacesForTabs", true).toBool();
    
    // Load window state
    m_lastOpenedPath = m_settings->value("lastOpenedPath", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();
    
    // Load recent files
    m_recentFiles = m_settings->value("recentFiles").toStringList();
    
    m_settings->endGroup();
    
    qDebug() << "Loaded settings:";
    qDebug() << "  Theme:" << themeName;
    qDebug() << "  Font:" << m_editorFont.toString();
    qDebug() << "  Word wrap:" << m_wordWrap;
    qDebug() << "  Line numbers:" << m_lineNumbers;
    qDebug() << "  Tab size:" << m_tabSize;
    qDebug() << "  Use spaces for tabs:" << m_useSpacesForTabs;
    qDebug() << "  Last opened path:" << m_lastOpenedPath;
}

void Settings::save()
{
    qDebug() << "Saving settings...";
    
    m_settings->beginGroup("Application");
    m_settings->setValue("theme", themeName());
    m_settings->setValue("font", m_editorFont.toString());
    m_settings->endGroup();
    
    m_settings->beginGroup("Editor");
    m_settings->setValue("wordWrap", m_wordWrap);
    m_settings->setValue("lineNumbers", m_lineNumbers);
    m_settings->setValue("tabSize", m_tabSize);
    m_settings->setValue("useSpacesForTabs", m_useSpacesForTabs);
    m_settings->setValue("lastOpenedPath", m_lastOpenedPath);
    m_settings->endGroup();
    
    // Force sync to write to disk immediately
    m_settings->sync();
    if (m_settings->status() != QSettings::NoError) {
        qWarning() << "Failed to save settings:" << m_settings->status();
    } else {
        qDebug() << "Settings saved successfully";
    }
}

void Settings::setTheme(Theme theme)
{
    if (m_theme != theme) {
        m_theme = theme;
        emit themeChanged(theme);
    }
}

void Settings::setEditorFont(const QFont &font)
{
    if (m_editorFont != font) {
        m_editorFont = font;
        emit editorFontChanged(font);
    }
}

void Settings::setLastOpenedPath(const QString &path)
{
    if (m_lastOpenedPath != path) {
        m_lastOpenedPath = path;
    }
}

void Settings::setWordWrap(bool enabled)
{
    if (m_wordWrap != enabled) {
        m_wordWrap = enabled;
        emit wordWrapChanged(enabled);
    }
}

void Settings::setLineNumbers(bool enabled)
{
    if (m_lineNumbers != enabled) {
        m_lineNumbers = enabled;
        emit lineNumbersChanged(enabled);
    }
}

void Settings::setTabSize(int size)
{
    if (m_tabSize != size && size > 0) {
        m_tabSize = size;
        emit tabSizeChanged(size);
    }
}

void Settings::setUseSpacesForTabs(bool useSpaces)
{
    if (m_useSpacesForTabs != useSpaces) {
        m_useSpacesForTabs = useSpaces;
        emit useSpacesForTabsChanged(useSpaces);
    }
}

