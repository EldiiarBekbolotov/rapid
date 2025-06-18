/**
 * @file application.h
 * @brief Defines the Application class which serves as the main application controller.
 * 
 * This class manages the application lifecycle, settings, and main window. It implements
 * the singleton pattern to provide global access to application-wide functionality.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QScopedPointer>
#include <QIcon>
#include <QString>
#include <QFont>
#include <QStyleFactory>
#include "settings.h"

// Forward declaration to reduce header dependencies
class MainWindow;

/**
 * @class Application
 * @brief Main application class that manages the application lifecycle and global settings.
 * 
 * The Application class is responsible for initializing the application, managing themes
 * and styles, and providing global access to application resources. It follows the
 * singleton pattern to ensure only one instance exists.
 */
class Application : public QApplication {
    Q_OBJECT
    
public:
    /**
     * @brief Constructs an Application instance.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line arguments.
     */
    explicit Application(int &argc, char **argv);
    
    /**
     * @brief Destroys the Application instance and cleans up resources.
     */
    ~Application() override;
    
    /**
     * @brief Initializes the application components.
     * 
     * This method should be called after construction to complete the initialization
     * of the application, including loading settings and showing the main window.
     */
    void initialize();
    
    /**
     * @brief Returns the singleton instance of the Application.
     * @return Pointer to the Application instance.
     */
    static Application *instance();
    
    /**
     * @brief Provides access to the application settings.
     * @return Pointer to the Settings instance.
     */
    Settings *settings() const;
    
    /**
     * @brief Sets the application's visual style.
     * @param style Name of the style to apply (e.g., "Fusion", "Windows").
     */
    void setApplicationStyle(const QString &style);
    
    /**
     * @brief Sets the application's default font.
     * @param font The QFont to use as the application's default font.
     */
    void setApplicationFont(const QFont &font);
    
    /**
     * @brief Loads and applies a stylesheet from the given path.
     * @param path Path to the stylesheet file.
     */
    void loadStyleSheet(const QString &path);
    
    // Getters for application properties
    QString applicationName() const { return m_applicationName; }    ///< Returns the application name
    QString organizationName() const { return m_organizationName; }  ///< Returns the organization name
    QString organizationDomain() const { return m_organizationDomain; } ///< Returns the organization domain
    
private slots:
    /**
     * @brief Handles theme change events.
     * @param theme The new theme to apply.
     */
    void onThemeChanged(Settings::Theme theme);

private:
    QScopedPointer<MainWindow> m_mainWindow;  ///< Main application window
    QScopedPointer<Settings> m_settings;      ///< Application settings
    
    // Application metadata
    QString m_applicationName;     ///< Name of the application
    QString m_organizationName;    ///< Name of the organization
    QString m_organizationDomain;  ///< Domain of the organization
    
    static Application *s_instance;  ///< Singleton instance pointer
};

#endif // APPLICATION_H
