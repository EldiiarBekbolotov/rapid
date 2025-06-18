/**
 * @file mainwindow.h
 * @brief Declaration of the MainWindow class.
 * 
 * This file contains the MainWindow class which serves as the main application
 * window for the Qt Web Editor, providing the user interface and coordinating
 * between different components like the editor, file browser, and preview.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QWebEngineView>
#include <QWebChannel>
#include <QWebEngineSettings>
#include <QDockWidget>
#include <QSplitter>
#include <QTabWidget>
#include <QToolBar>
#include <QMenu>
#include <QAction>
#include <QTreeView>
#include <QStatusBar>
#include <QLabel>
#include <QSettings>
#include <QCloseEvent>
#include <QTemporaryFile>
#include <QStringList>
#include <QStandardPaths>
#include <QKeySequence>
#include <QDebug>

// Forward declarations
class EditorWidget;
class QLabel;
class QWebEngineView;

// Forward declarations
class EditorWidget;
class QLabel;
class QWebEngineView;

/**
 * @brief The MainWindow class represents the main application window.
 * 
 * This class manages the main user interface, including the menu bar, toolbars,
 * editor tabs, file browser, and web preview. It coordinates between different
 * components and handles user interactions.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a MainWindow with the given parent.
     * 
     * @param parent The parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Destroys the MainWindow.
     */
    ~MainWindow();
    
    // Disable copy constructor and assignment operator
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

protected:
    /**
     * @brief Handles the window close event.
     * 
     * Prompts the user to save any unsaved changes before closing.
     * 
     * @param event The close event.
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    /** @name File Operations
     *  Methods handling file operations.
     */
    ///@{
    void newFile();
    void openFile();
    void openFolder();
    void saveFile();
    void saveFileAs();
    void closeTab(int index);
    void openFileInEditor(const QString &filePath);
    void updateRecentFilesMenu(const QStringList &recentFiles);
    ///@}
    
    /** @name Tab Management
     *  Methods for managing editor tabs.
     */
    ///@{
    void currentTabChanged(int index);
    void fileDoubleClicked(const QModelIndex &index);
    ///@}
    
    /** @name UI Updates
     *  Methods for updating the user interface.
     */
    ///@{
    void updateWindowTitle();
    void updatePreview();
    void updateUiForTheme();
    void updateUiForFont(const QFont &font);
    ///@}
    
    /** @name Application Actions
     *  Methods for application-level actions.
     */
    ///@{
    void showAbout();
    void showPreferences();
    void toggleFullScreen();
    void toggleMenuBar();
    void toggleToolBar();
    ///@}
    
    /** @name Browser Preview
     *  Methods for managing browser preview functionality.
     */
    ///@{
    void runInBrowser();
    void runInDefaultBrowser();
    void runInSpecificBrowser(const QString &browserPath);
    void showBrowserSelectionDialog();
    ///@}

private:
    /** @name Initialization Methods
     *  Methods for setting up the main window components.
     */
    ///@{
    void setupActions();
    void setupToolBar();
    void setupDockWidgets();
    void setupStatusBar();
    void applyThemeAndFontSettings();
    ///@}
    
    // UI Components
    QMenu *m_fileMenu = nullptr;              /**< The File menu in the menu bar. */
    QToolBar *m_toolBar = nullptr;            /**< The main toolbar. */
    QDockWidget *m_fileBrowserDock = nullptr; /**< Dock widget for the file browser. */
    QTabWidget *m_tabWidget = nullptr;        /**< Widget for managing editor tabs. */
    QSplitter *m_mainSplitter = nullptr;      /**< Main splitter for resizable panels. */
    
    // File System
    QFileSystemModel *m_fileSystemModel = nullptr;  /**< Model for the file system. */
    QTreeView *m_fileBrowser = nullptr;             /**< Tree view for file system navigation. */
    
    // Web Preview
    QWebEngineView *m_webView = nullptr;      /**< Web view for previewing content. */
    QWebChannel m_webChannel;                 /**< Channel for communication with web content. */
    
    // File Actions
    QAction *m_newFileAction = nullptr;       /**< Action for creating a new file. */
    QAction *m_openFileAction = nullptr;      /**< Action for opening a file. */
    QAction *m_openFolderAction = nullptr;    /**< Action for opening a folder. */
    QAction *m_saveAction = nullptr;          /**< Action for saving the current file. */
    QAction *m_saveAsAction = nullptr;        /**< Action for saving the current file with a new name. */
    QAction *m_closeTabAction = nullptr;      /**< Action for closing the current tab. */
    QAction *m_exitAction = nullptr;          /**< Action for exiting the application. */
    QAction *m_togglePreviewAction = nullptr;
    QAction *m_toggleFileBrowserAction = nullptr;
    QAction *m_fullScreenAction = nullptr;
    QAction *m_preferencesAction = nullptr;
    QAction *m_lightThemeAction = nullptr;
    QAction *m_darkThemeAction = nullptr;
    QAction *m_systemThemeAction = nullptr;
    QAction *m_increaseFontSizeAction = nullptr;
    QAction *m_decreaseFontSizeAction = nullptr;
    QAction *m_resetFontSizeAction = nullptr;
    QAction *m_aboutAction = nullptr;
    QAction *m_runAction = nullptr;
    QList<QAction*> m_recentFileActions;
    
    // State
    QString m_currentFolder;
    bool m_isPreviewVisible = true;
    bool m_isFileBrowserVisible = true;
    QLabel *m_statusLabel = nullptr;
    bool m_isFullScreen = false;
    bool m_wasMenuBarVisible = true;
    QStringList m_tempFiles;
    bool m_wasStatusBarVisible = true;
    bool m_wasToolBarVisible = true;
    
    // Theme and font
    QMenu *m_themeMenu = nullptr;
    QMenu *m_fontMenu = nullptr;
    
    // Private methods
    void setupConnections();
    void loadSettings();
    void saveSettings();
    EditorWidget *currentEditor() const;
    EditorWidget *editorForPath(const QString &filePath) const;
    bool maybeSave(EditorWidget *editor);
    void createNewEditorTab(const QString &filePath = QString());
};

#endif // MAINWINDOW_H
