#include "application.h"
#include "mainwindow.h"
#include "settings.h"
#include <QFile>
#include <QStyle>
#include <QStyleFactory>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>

Application *Application::s_instance = nullptr;

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_settings(new Settings(this))
    , m_applicationName("Rapid")
    , m_organizationName("Rapid")
    , m_organizationDomain("eldiiarbekbolotov.github.io/rapid")
{
    s_instance = this;
    
    // Set application metadata
    setApplicationName(m_applicationName);
    setOrganizationName(m_organizationName);
    setOrganizationDomain(m_organizationDomain);
    setApplicationVersion("1.0.0");
    
    // Set window icon if it exists
    if (QFile::exists(":/icons/app-icon.svg")) {
        setWindowIcon(QIcon(":/icons/app-icon.svg"));
    }
    
    // Set default application style
    setApplicationStyle("Fusion");
    
    // Set default font
    QFont defaultFont("Monospace");
    defaultFont.setStyleHint(QFont::TypeWriter);
    defaultFont.setPointSize(10);
    setApplicationFont(defaultFont);
    
    // Connect theme changes
    connect(m_settings.data(), &Settings::themeChanged, this, &Application::onThemeChanged);
}

void Application::onThemeChanged(Settings::Theme theme)
{
    QString themePath;
    switch (theme) {
        case Settings::Theme::Dark:
            themePath = ":/themes/dark.qss";
            break;
        case Settings::Theme::Light:
        case Settings::Theme::System: // Default to light theme for system
        default:
            themePath = ":/themes/light.qss";
            break;
    }
    
    loadStyleSheet(themePath);
}

Application::~Application()
{
    s_instance = nullptr;
}

void Application::initialize()
{
    // Initialize settings
    m_settings->load();
    
    // Apply saved theme
    onThemeChanged(m_settings->theme());
    
    // Apply saved font
    QFont savedFont = m_settings->font();
    if (savedFont != QFont()) {
        setApplicationFont(savedFont);
    }
    
    // Create and show main window
    m_mainWindow.reset(new MainWindow);
    m_mainWindow->show();
}

void Application::setApplicationStyle(const QString &style)
{
    if (QStyleFactory::keys().contains(style, Qt::CaseInsensitive)) {
        setStyle(QStyleFactory::create(style));
    } else {
        qWarning() << "Style" << style << "not available. Available styles:" << QStyleFactory::keys();
    }
}

void Application::setApplicationFont(const QFont &font)
{
    setFont(font);
    
    // Update font for all widgets
    foreach (QWidget *widget, allWidgets()) {
        widget->setFont(font);
    }
}

void Application::loadStyleSheet(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open stylesheet file:" << path;
        return;
    }
    
    QString styleSheet = QLatin1String(file.readAll());
    setStyleSheet(styleSheet);
    file.close();
}

Application *Application::instance()
{
    return s_instance;
}

Settings *Application::settings() const
{
    return m_settings.data();
}
