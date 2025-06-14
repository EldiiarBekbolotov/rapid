#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QTabWidget>
#include <QSplitter>
#include <QFont>
#include <QString>
#include <QMenu>
#include <QMenuBar>
#include <QDockWidget>
#include <QToolBar>
#include <QTreeView>
#include <QStandardPaths>

class EditorWidget;
class QToolBar;
class QAction;
class QLabel;
class QWebEngineView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

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
    QList<QAction*> m_recentFileActions;
    
    // State
    QString m_currentFolder;
    bool m_isPreviewVisible = true;
    bool m_isFileBrowserVisible = true;
    bool m_wasMenuBarVisible = true;
    bool m_wasStatusBarVisible = true;
    
    // Theme and font
    QMenu *m_themeMenu = nullptr;
    QMenu *m_fontMenu = nullptr;
    QLabel *m_statusLabel = nullptr;
    
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
