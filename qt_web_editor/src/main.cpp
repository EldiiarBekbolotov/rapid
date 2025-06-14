#include "core/application.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QFontDatabase>

int main(int argc, char *argv[]) {
    try {
        // Initialize the application
        Application app(argc, argv);
        
        // Set application metadata
        app.setApplicationName("Qt Web Editor");
        app.setApplicationVersion("1.0.0");
        app.setOrganizationName("QtWebEditor");
        
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
