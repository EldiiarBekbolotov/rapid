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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // Disable copy constructor and assignment operator
    MainWindow(const MainWindow&) = delete;
    MainWindow& operator=(const MainWindow&) = delete;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newFile();
    void openFile();
    void openFolder();
    void saveFile();
    void saveFileAs();
    void closeTab(int index);
    void currentTabChanged(int index);
    void fileDoubleClicked(const QModelIndex &index);
    void updateWindowTitle();
    void showAbout();
    void updatePreview();
    void updateUiForTheme();
    void updateUiForFont(const QFont &font);
    void showPreferences();
    void toggleFullScreen();
    void openFileInEditor(const QString &filePath);
    void updateRecentFilesMenu(const QStringList &recentFiles);
    void runInBrowser();
    void runInDefaultBrowser();
    void runInSpecificBrowser(const QString &browserPath);
    void showBrowserSelectionDialog();
    void toggleMenuBar();
    void toggleToolBar();

private:
    void setupActions();
    void setupToolBar();
    void setupDockWidgets();
    void setupStatusBar();
    void applyThemeAndFontSettings();
    
    // UI Components
    QMenu *m_fileMenu = nullptr;
    QToolBar *m_toolBar = nullptr;
    QDockWidget *m_fileBrowserDock = nullptr;
    QTabWidget *m_tabWidget = nullptr;
    QSplitter *m_mainSplitter = nullptr;
    QFileSystemModel *m_fileSystemModel = nullptr;
    QTreeView *m_fileBrowser = nullptr;
    QWebEngineView *m_webView = nullptr;
    QWebChannel m_webChannel;
    
    // Actions
    QAction *m_newFileAction = nullptr;
    QAction *m_openFileAction = nullptr;
    QAction *m_openFolderAction = nullptr;
    QAction *m_saveAction = nullptr;
    QAction *m_saveAsAction = nullptr;
    QAction *m_closeTabAction = nullptr;
    QAction *m_exitAction = nullptr;
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
