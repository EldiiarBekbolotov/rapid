#include "settings.h"
#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
/**
 * @file settings.cpp
 * @brief Implementation of the Settings class.
 * 
 * This file contains the implementation of the Settings class which manages
 * application settings including theme, font, editor preferences, and recent files.
 */

#include <QDebug>
#include <QFontDatabase>
#include <QStandardPaths>

/**
 * @brief Constructs a Settings object with the given parent.
 * 
 * Initializes the settings with default values and loads any saved settings.
 * 
 * @param parent The parent QObject.
 */
Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings("Rapid", "QtWebEditor", this))
{
}

/**
 * @brief Destroys the Settings object.
 * 
 * Ensures all settings are saved before destruction.
 */
Settings::~Settings()
{
    save();
}

/**
 * @brief Loads settings from persistent storage.
 * 
 * Reads all settings from the system's settings storage and updates
 * the corresponding member variables. Emits appropriate signals if
 * the loaded values differ from the current values.
 */
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

/**
 * @brief Saves current settings to persistent storage.
 * 
 * Writes all current setting values to the system's settings storage.
 */
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

/**
 * @brief Sets the application theme.
 * 
 * Updates the theme setting and emits themeChanged() if the theme has changed.
 * 
 * @param theme The theme to set.
 */
void Settings::setTheme(Theme theme)
{
    if (m_theme != theme) {
        m_theme = theme;
        emit themeChanged(theme);
    }
}

/**
 * @brief Sets the editor font.
 * 
 * Updates the editor font setting and emits editorFontChanged() if the font has changed.
 * 
 * @param font The font to use in the editor.
 */
void Settings::setEditorFont(const QFont &font)
{
    if (m_editorFont != font) {
        m_editorFont = font;
        emit editorFontChanged(font);
    }
}

/**
 * @brief Sets the last opened directory path.
 * 
 * Updates the last opened path setting.
 * 
 * @param path The path to remember as the last opened directory.
 */
void Settings::setLastOpenedPath(const QString &path)
{
    if (m_lastOpenedPath != path) {
        m_lastOpenedPath = path;
    }
}

/**
 * @brief Enables or disables word wrap in the editor.
 * 
 * Updates the word wrap setting and emits wordWrapChanged() if the setting has changed.
 * 
 * @param enabled Whether word wrap should be enabled.
 */
void Settings::setWordWrap(bool enabled)
{
    if (m_wordWrap != enabled) {
        m_wordWrap = enabled;
        emit wordWrapChanged(enabled);
    }
}

/**
 * @brief Shows or hides line numbers in the editor.
 * 
 * Updates the line numbers setting and emits lineNumbersChanged() if the setting has changed.
 * 
 * @param enabled Whether to show line numbers.
 */
void Settings::setLineNumbers(bool enabled)
{
    if (m_lineNumbers != enabled) {
        m_lineNumbers = enabled;
        emit lineNumbersChanged(enabled);
    }
}

/**
 * @brief Sets the number of spaces per tab.
 * 
 * Updates the tab size setting and emits tabSizeChanged() if the size has changed.
 * 
 * @param size The number of spaces for each tab character.
 */
void Settings::setTabSize(int size)
{
    if (m_tabSize != size && size > 0) {
        m_tabSize = size;
        emit tabSizeChanged(size);
    }
}

/**
 * @brief Sets whether to use spaces instead of tab characters.
 * 
 * Updates the tab/space preference and emits useSpacesForTabsChanged() if the setting has changed.
 * 
 * @param useSpaces If true, spaces will be inserted when pressing Tab.
 */
void Settings::setUseSpacesForTabs(bool useSpaces)
{
    if (m_useSpacesForTabs != useSpaces) {
        m_useSpacesForTabs = useSpaces;
        emit useSpacesForTabsChanged(useSpaces);
    }
}
