/**
 * @file settings.h
 * @brief Declaration of the Settings class.
 * 
 * This file contains the Settings class which manages application settings
 * including theme, font, editor preferences, and recent files.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QFont>
#include <QString>
#include <QSettings>

/**
 * @brief The Settings class manages application settings and preferences.
 * 
 * This class handles loading, saving, and notifying about changes to
 * application settings such as theme, font, and editor preferences.
 */
class Settings : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief The Theme enum represents the available UI themes.
     */
    enum class Theme {
        Light,  /**< Light color theme. */
        Dark,   /**< Dark color theme. */
        System  /**< Use system theme if available, otherwise fallback to Light. */
    };

    /**
     * @brief Constructs a Settings object with the given parent.
     * 
     * @param parent The parent QObject.
     */
    explicit Settings(QObject *parent = nullptr);
    
    /**
     * @brief Destroys the Settings object.
     */
    ~Settings() override;

    /**
     * @brief Loads settings from persistent storage.
     */
    void load();
    
    /**
     * @brief Saves current settings to persistent storage.
     */
    void save();

    // Getters
    /** @return The current theme setting. */
    Theme theme() const { return m_theme; }
    
    /** @return The current theme name as a string ("light" or "dark"). */
    QString themeName() const { return m_theme == Theme::Dark ? "dark" : "light"; }
    
    /** @return The current editor font. */
    QFont font() const { return m_editorFont; }
    
    /** @return The current editor font (alias for font()). */
    QFont editorFont() const { return m_editorFont; }
    
    /** @return The last opened directory path. */
    QString lastOpenedPath() const { return m_lastOpenedPath; }
    
    /** @return True if word wrap is enabled, false otherwise. */
    bool wordWrap() const { return m_wordWrap; }
    
    /** @return True if line numbers are shown, false otherwise. */
    bool lineNumbers() const { return m_lineNumbers; }
    
    /** @return The number of spaces per tab. */
    int tabSize() const { return m_tabSize; }
    
    /** @return True if spaces should be used instead of tab characters. */
    bool useSpacesForTabs() const { return m_useSpacesForTabs; }

    /**
     * @brief Sets the application theme.
     * 
     * @param theme The theme to set.
     */
    void setTheme(Theme theme);
    
    /**
     * @brief Sets the application theme by name.
     * 
     * @param themeName The name of the theme ("light" or "dark").
     */
    void setTheme(const QString &themeName) {
        if (themeName.compare("dark", Qt::CaseInsensitive) == 0) {
            setTheme(Theme::Dark);
        } else {
            setTheme(Theme::Light);
        }
    }
    
    /**
     * @brief Sets the editor font.
     * 
     * @param font The font to use in the editor.
     */
    void setEditorFont(const QFont &font);
    
    /**
     * @brief Sets the last opened directory path.
     * 
     * @param path The path to remember as the last opened directory.
     */
    void setLastOpenedPath(const QString &path);
    
    /**
     * @brief Enables or disables word wrap in the editor.
     * 
     * @param enabled Whether word wrap should be enabled.
     */
    void setWordWrap(bool enabled);
    
    /**
     * @brief Shows or hides line numbers in the editor.
     * 
     * @param enabled Whether to show line numbers.
     */
    void setLineNumbers(bool enabled);
    
    /**
     * @brief Sets the number of spaces per tab.
     * 
     * @param size The number of spaces for each tab character.
     */
    void setTabSize(int size);
    
    /**
     * @brief Sets whether to use spaces instead of tab characters.
     * 
     * @param useSpaces If true, spaces will be inserted when pressing Tab.
     */
    void setUseSpacesForTabs(bool useSpaces);
    
    /**
     * @brief Gets the path to the settings file.
     * 
     * @return The full path to the settings file.
     */
    QString settingsPath() const { return m_settings->fileName(); }
    
    /**
     * @brief Gets the list of recently opened files.
     * 
     * @return A list of file paths.
     */
    QStringList recentFiles() const { return m_recentFiles; }
    
    /**
     * @brief Sets the list of recently opened files.
     * 
     * Only the 10 most recent files are kept.
     * 
     * @param files The list of recent file paths.
     */
    void setRecentFiles(const QStringList &files) {
        if (m_recentFiles != files) {
            m_recentFiles = files.mid(0, 10); // Keep only the 10 most recent files
            m_settings->setValue("recentFiles", m_recentFiles);
            m_settings->sync();
        }
    }

signals:
    /**
     * @brief Emitted when the application theme changes.
     * 
     * @param theme The new theme.
     */
    void themeChanged(Theme theme);
    
    /**
     * @brief Emitted when the editor font changes.
     * 
     * @param font The new font.
     */
    void editorFontChanged(const QFont &font);
    
    /**
     * @brief Emitted when word wrap setting changes.
     * 
     * @param enabled The new word wrap state.
     */
    void wordWrapChanged(bool enabled);
    
    /**
     * @brief Emitted when line numbers visibility changes.
     * 
     * @param enabled The new line numbers state.
     */
    void lineNumbersChanged(bool enabled);
    
    /**
     * @brief Emitted when tab size changes.
     * 
     * @param size The new tab size in spaces.
     */
    void tabSizeChanged(int size);
    
    /**
     * @brief Emitted when the tab/space preference changes.
     * 
     * @param useSpaces True if spaces should be used instead of tabs.
     */
    void useSpacesForTabsChanged(bool useSpaces);

private:
    QSettings *m_settings;                /**< The QSettings instance for persistent storage. */
    
    // Application settings
    Theme m_theme{Theme::System};         /**< The current application theme. */
    QFont m_editorFont{"Monospace", 10}; /**< The font used in the editor. */
    QString m_lastOpenedPath;            /**< The last directory path that was opened. */
    bool m_wordWrap{false};              /**< Whether word wrap is enabled. */
    bool m_lineNumbers{true};            /**< Whether line numbers are shown. */
    int m_tabSize{4};                    /**< Number of spaces per tab. */
    bool m_useSpacesForTabs{true};       /**< Whether to use spaces instead of tabs. */
    QStringList m_recentFiles;           /**< List of recently opened files. */
};

#endif // SETTINGS_H
