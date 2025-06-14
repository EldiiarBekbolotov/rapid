#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QFont>
#include <QString>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT

public:
    enum class Theme {
        Light,
        Dark,
        System
    };

    explicit Settings(QObject *parent = nullptr);
    ~Settings() override;

    void load();
    void save();

    // Getters
    Theme theme() const { return m_theme; }
    QString themeName() const { return m_theme == Theme::Dark ? "dark" : "light"; }
    QFont font() const { return m_editorFont; }
    QFont editorFont() const { return m_editorFont; }
    QString lastOpenedPath() const { return m_lastOpenedPath; }
    bool wordWrap() const { return m_wordWrap; }
    bool lineNumbers() const { return m_lineNumbers; }
    int tabSize() const { return m_tabSize; }
    bool useSpacesForTabs() const { return m_useSpacesForTabs; }

    // Setters
    void setTheme(Theme theme);
    void setTheme(const QString &themeName) {
        if (themeName.compare("dark", Qt::CaseInsensitive) == 0) {
            setTheme(Theme::Dark);
        } else {
            setTheme(Theme::Light);
        }
    }
    void setEditorFont(const QFont &font);
    void setLastOpenedPath(const QString &path);
    void setWordWrap(bool enabled);
    void setLineNumbers(bool enabled);
    void setTabSize(int size);
    void setUseSpacesForTabs(bool useSpaces);
    
    // Convenience methods
    QString settingsPath() const { return m_settings->fileName(); }
    
    // Recent files
    QStringList recentFiles() const { return m_recentFiles; }
    void setRecentFiles(const QStringList &files) {
        if (m_recentFiles != files) {
            m_recentFiles = files.mid(0, 10); // Keep only the 10 most recent files
            m_settings->setValue("recentFiles", m_recentFiles);
            m_settings->sync();
        }
    }

signals:
    void themeChanged(Theme theme);
    void editorFontChanged(const QFont &font);
    void wordWrapChanged(bool enabled);
    void lineNumbersChanged(bool enabled);
    void tabSizeChanged(int size);
    void useSpacesForTabsChanged(bool useSpaces);

private:
    QSettings *m_settings;
    
    Theme m_theme{Theme::System};
    QFont m_editorFont{"Monospace", 10};
    QString m_lastOpenedPath;
    bool m_wordWrap{false};
    bool m_lineNumbers{true};
    int m_tabSize{4};
    bool m_useSpacesForTabs{true};
    QStringList m_recentFiles;
};

#endif // SETTINGS_H
