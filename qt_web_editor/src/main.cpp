/**
 * @file main.cpp
 * @brief Main entry point for the Rapid Qt Web Editor application.
 * 
 * This file contains the main() function which serves as the entry point
 * for the application. It initializes the application, sets up error handling,
 * and starts the event loop.
 */

#include "core/application.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QFontDatabase>

/**
 * @brief Main entry point of the application.
 * 
 * Initializes the Qt application, sets up the main window, and starts the
 * event loop. Also handles any uncaught exceptions and displays error messages.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Application exit code (0 for success, non-zero for errors).
 */
int main(int argc, char *argv[]) {
    try {
        // Initialize the application
        Application app(argc, argv);
        
        // Set application metadata
        app.setApplicationName("Rapid");
        app.setApplicationVersion("1.0.0");
        app.setOrganizationName("Rapid");
        app.setWindowIcon(QIcon(":/icons/app-icon.svg"));
        
        // Log available fonts for debugging
        qDebug() << "Available font families:" << QFontDatabase::families();
        
        // Load and apply saved settings
        app.initialize();
        
        // Show application window
        qDebug() << "Starting application...";
        return app.exec();
        
    } catch (const std::exception &e) {
        qCritical() << "Unhandled exception:" << e.what();
        QMessageBox::critical(nullptr, "Fatal Error", 
                            QString("An unexpected error occurred: %1").arg(e.what()));
        return 1;
    } catch (...) {
        qCritical() << "Unknown exception occurred";
        QMessageBox::critical(nullptr, "Fatal Error", "An unknown error occurred");
        return 1;
    }
}
