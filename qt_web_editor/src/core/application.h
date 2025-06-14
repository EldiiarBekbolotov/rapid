#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QScopedPointer>
#include <QIcon>
#include <QString>
#include <QFont>
#include <QStyleFactory>
#include "settings.h"

class MainWindow;

class Application : public QApplication {
    Q_OBJECT
    
public:
    explicit Application(int &argc, char **argv);
    ~Application() override;
    
    void initialize();
    
    static Application *instance();
    Settings *settings() const;
    
    void setApplicationStyle(const QString &style);
    void setApplicationFont(const QFont &font);
    void loadStyleSheet(const QString &path);
    
    QString applicationName() const { return m_applicationName; }
    QString organizationName() const { return m_organizationName; }
    QString organizationDomain() const { return m_organizationDomain; }
    
private slots:
    void onThemeChanged(Settings::Theme theme);

private:
    QScopedPointer<MainWindow> m_mainWindow;
    QScopedPointer<Settings> m_settings;
    
    // Application properties
    QString m_applicationName;
    QString m_organizationName;
    QString m_organizationDomain;
    
    static Application *s_instance;
};

#endif // APPLICATION_H
