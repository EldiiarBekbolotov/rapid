/**
 * @file application.cpp
 * @brief Implementation of the Application class.
 * 
 * This file contains the implementation of the Application class which serves as the
 * central controller for the application, managing initialization, theming, and global settings.
 */

#include "application.h"
#include "mainwindow.h"
#include "settings.h"
#include <QFile>
#include <QStyle>
#include <QStyleFactory>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>

// Initialize static member
Application *Application::s_instance = nullptr;

/**
 * @brief Constructs the Application instance.
 * 
 * Initializes the application with default settings and sets up the application metadata.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 */
Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_settings(new Settings(this))
    , m_applicationName("Rapid")
    , m_organizationName("Rapid")
    , m_organizationDomain("rapideditor.dev")
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

/**
 * @brief Handles theme changes by loading the appropriate stylesheet.
 * 
 * This slot is called whenever the application's theme is changed. It loads
 * the corresponding stylesheet and applies it to the application.
 * 
 * @param theme The new theme to apply.
 */
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

/**
 * @brief Destroys the Application instance.
 * 
 * Cleans up resources and resets the singleton instance pointer.
 */
Application::~Application()
{
    s_instance = nullptr;
}

/**
 * @brief Initializes the application components.
 * 
 * This method completes the initialization of the application by:
 * 1. Loading application settings
 * 2. Applying the saved theme and font
 * 3. Creating and showing the main window
 */
void Application::initialize()
{
    // Initialize settings from persistent storage
    m_settings->load();
    
    // Apply the saved theme
    onThemeChanged(m_settings->theme());
    
    // Apply the saved font if one exists
    QFont savedFont = m_settings->font();
    if (savedFont != QFont()) {
        setApplicationFont(savedFont);
    }
    
    // Create and show the main application window
    m_mainWindow.reset(new MainWindow);
    m_mainWindow->show();
}

/**
 * @brief Sets the application's visual style.
 * 
 * Applies the specified style to the application if it's available.
 * Logs a warning if the requested style is not found.
 * 
 * @param style Name of the style to apply (e.g., "Fusion", "Windows").
 */
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

/**
 * @brief Returns the singleton instance of the Application.
 * 
 * @return Pointer to the Application instance.
 * @warning This will return nullptr if called before the Application is constructed.
 */
Application *Application::instance()
{
    return s_instance;
}

/**
 * @brief Provides access to the application settings.
 * 
 * @return Pointer to the Settings instance.
 */
Settings *Application::settings() const
{
    return m_settings.data();
}
