#include "mainwindow.h"
#include "editorwidget.h"
#include "settings.h"
#include "application.h"

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QTabWidget>
#include <QSplitter>
#include <QFileSystemModel>
#include <QTreeView>
#include <QWebEngineView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>

#include <QTabBar>
#include <QSplitter>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QWebEngineView>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QMenuBar>
#include <QMenu>
#include <QFontDialog>
#include <QStyleFactory>
#include <QShortcut>
#include <QActionGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_toolBar(nullptr)
    , m_fileBrowserDock(nullptr)
    , m_tabWidget(new QTabWidget(this))
    , m_mainSplitter(new QSplitter(Qt::Horizontal, this))
    , m_fileSystemModel(new QFileSystemModel(this))
    , m_fileBrowser(new QTreeView(this))
    , m_webView(new QWebEngineView(this))
    , m_newFileAction(nullptr)
    , m_openFileAction(nullptr)
    , m_openFolderAction(nullptr)
    , m_saveAction(nullptr)
    , m_saveAsAction(nullptr)
    , m_closeTabAction(nullptr)
    , m_exitAction(nullptr)
    , m_togglePreviewAction(nullptr)
    , m_toggleFileBrowserAction(nullptr)
    , m_fullScreenAction(nullptr)
    , m_preferencesAction(nullptr)
    , m_lightThemeAction(nullptr)
    , m_darkThemeAction(nullptr)
    , m_systemThemeAction(nullptr)
    , m_increaseFontSizeAction(nullptr)
    , m_decreaseFontSizeAction(nullptr)
    , m_resetFontSizeAction(nullptr)
    , m_aboutAction(nullptr)
    , m_currentFolder()
    , m_isPreviewVisible(true)
    , m_isFileBrowserVisible(true)
    , m_wasMenuBarVisible(true)
    , m_wasStatusBarVisible(true)
    , m_statusLabel(new QLabel(this))
{
    setWindowTitle(tr("Qt Web Editor"));
    setMinimumSize(800, 600);
    
    // Setup UI
    setupActions();
    setupToolBar();
    setupDockWidgets();
    setupStatusBar();
    setupConnections();
    
    // Load settings
    loadSettings();
    
    // Show welcome tab
    createNewEditorTab();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Check for unsaved changes
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        auto editor = qobject_cast<EditorWidget*>(m_tabWidget->widget(i));
        if (editor && !maybeSave(editor)) {
            event->ignore();
            return;
        }
    }
    
    event->accept();
}

void MainWindow::setupActions()
{
    // File actions
    m_newFileAction = new QAction(QIcon(":/icons/document-new.svg"), tr("&New"), this);
    m_newFileAction->setShortcut(QKeySequence::New);
    
    m_openFileAction = new QAction(QIcon(":/icons/document-open.svg"), tr("&Open File..."), this);
    m_openFileAction->setShortcut(QKeySequence::Open);
    
    m_openFolderAction = new QAction(QIcon(":/icons/folder-open.svg"), tr("Open &Folder..."), this);
    m_saveAction = new QAction(QIcon(":/icons/document-save.svg"), tr("&Save"), this);
    m_saveAction->setShortcut(QKeySequence::Save);
    
    m_saveAsAction = new QAction(tr("Save &As..."), this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    
    m_closeTabAction = new QAction(tr("&Close Tab"), this);
    m_closeTabAction->setShortcut(QKeySequence::Close);
    
    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    
    // View actions
    m_togglePreviewAction = new QAction(tr("&Preview"), this);
    m_togglePreviewAction->setCheckable(true);
    m_togglePreviewAction->setChecked(m_isPreviewVisible);
    m_togglePreviewAction->setShortcut(Qt::CTRL | Qt::Key_P);
    
    m_toggleFileBrowserAction = new QAction(tr("&File Browser"), this);
    m_toggleFileBrowserAction->setCheckable(true);
    m_toggleFileBrowserAction->setChecked(m_isFileBrowserVisible);
    m_toggleFileBrowserAction->setShortcut(Qt::CTRL | Qt::Key_B);
    
    m_fullScreenAction = new QAction(tr("Full Screen"), this);
    m_fullScreenAction->setCheckable(true);
    m_fullScreenAction->setShortcut(Qt::Key_F11);
    
    // Preferences
    m_preferencesAction = new QAction(tr("Preferences..."), this);
    m_preferencesAction->setMenuRole(QAction::PreferencesRole);
    
    // Theme actions
    m_themeMenu = new QMenu(tr("Theme"), this);
    
    QActionGroup *themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);
    
    m_lightThemeAction = new QAction(tr("Light"), this);
    m_lightThemeAction->setCheckable(true);
    m_lightThemeAction->setActionGroup(themeGroup);
    m_lightThemeAction->setData("light");
    
    m_darkThemeAction = new QAction(tr("Dark"), this);
    m_darkThemeAction->setCheckable(true);
    m_darkThemeAction->setActionGroup(themeGroup);
    m_darkThemeAction->setData("dark");
    
    m_systemThemeAction = new QAction(tr("System"), this);
    m_systemThemeAction->setCheckable(true);
    m_systemThemeAction->setActionGroup(themeGroup);
    m_systemThemeAction->setData("system");
    m_systemThemeAction->setChecked(true);
    
    m_themeMenu->addAction(m_lightThemeAction);
    m_themeMenu->addAction(m_darkThemeAction);
    m_themeMenu->addSeparator();
    m_themeMenu->addAction(m_systemThemeAction);
    
    // Font actions
    m_fontMenu = new QMenu(tr("Font"), this);
    
    m_increaseFontSizeAction = new QAction(tr("Increase Font Size"), this);
    m_increaseFontSizeAction->setShortcut(QKeySequence::ZoomIn);
    
    m_decreaseFontSizeAction = new QAction(tr("Decrease Font Size"), this);
    m_decreaseFontSizeAction->setShortcut(QKeySequence::ZoomOut);
    
    m_resetFontSizeAction = new QAction(tr("Reset Font Size"), this);
    m_resetFontSizeAction->setShortcut(QKeySequence("Ctrl+0"));
    
    m_fontMenu->addAction(m_increaseFontSizeAction);
    m_fontMenu->addAction(m_decreaseFontSizeAction);
    m_fontMenu->addSeparator();
    m_fontMenu->addAction(m_resetFontSizeAction);
    
    // Help actions
    m_aboutAction = new QAction(tr("&About"), this);
    
    // Set up menu bar
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newFileAction);
    m_fileMenu->addAction(m_openFileAction);
    m_fileMenu->addAction(m_openFolderAction);
    m_fileMenu->addSeparator();
    
    // Add recent files menu items (will be populated later)
    updateRecentFilesMenu(Application::instance()->settings()->recentFiles());
    
    // Add the rest of the file menu items
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_closeTabAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_preferencesAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);
    
    // Edit menu
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    
    QAction *undoAction = editMenu->addAction(QIcon(":/icons/edit-undo.svg"), tr("&Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    
    QAction *redoAction = editMenu->addAction(QIcon(":/icons/edit-redo.svg"), tr("&Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    
    editMenu->addSeparator();
    
    QAction *cutAction = editMenu->addAction(QIcon(":/icons/edit-cut.svg"), tr("Cu&t"));
    cutAction->setShortcut(QKeySequence::Cut);
    
    QAction *copyAction = editMenu->addAction(QIcon(":/icons/edit-copy.svg"), tr("&Copy"));
    copyAction->setShortcut(QKeySequence::Copy);
    
    QAction *pasteAction = editMenu->addAction(QIcon(":/icons/edit-paste.svg"), tr("&Paste"));
    pasteAction->setShortcut(QKeySequence::Paste);
    
    // View menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(m_toggleFileBrowserAction);
    viewMenu->addAction(m_togglePreviewAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_fullScreenAction);
    viewMenu->addSeparator();
    
    // Add theme submenu
    viewMenu->addMenu(m_themeMenu);
    
    // Add font submenu
    viewMenu->addMenu(m_fontMenu);
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(m_aboutAction);
    
    // Connect actions
    connect(m_newFileAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(m_openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_openFolderAction, &QAction::triggered, this, &MainWindow::openFolder);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(m_closeTabAction, &QAction::triggered, [this]() { closeTab(m_tabWidget->currentIndex()); });
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // Edit actions
    connect(undoAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->undo();
    });
    connect(redoAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->redo();
    });
    connect(cutAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->cut();
    });
    connect(copyAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->copy();
    });
    connect(pasteAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->paste();
    });
    
    // View actions
    connect(m_togglePreviewAction, &QAction::toggled, [this](bool visible) {
        m_isPreviewVisible = visible;
        m_webView->setVisible(visible);
    });
    connect(m_toggleFileBrowserAction, &QAction::toggled, [this](bool visible) {
        m_isFileBrowserVisible = visible;
        m_fileBrowser->setVisible(visible);
    });
    connect(m_fullScreenAction, &QAction::toggled, this, &MainWindow::toggleFullScreen);
    
    // Theme actions
    connect(m_lightThemeAction, &QAction::triggered, this, [this]() {
        Application::instance()->settings()->setTheme("light");
        updateUiForTheme();
    });
    connect(m_darkThemeAction, &QAction::triggered, this, [this]() {
        Application::instance()->settings()->setTheme("dark");
        updateUiForTheme();
    });
    connect(m_systemThemeAction, &QAction::triggered, this, [this]() {
        Application::instance()->settings()->setTheme("system");
        updateUiForTheme();
    });
    
    // Font actions
    connect(m_increaseFontSizeAction, &QAction::triggered, this, [this]() {
        QFont font = Application::instance()->settings()->font();
        font.setPointSize(font.pointSize() + 1);
        Application::instance()->settings()->setEditorFont(font);
        updateUiForFont(font);
    });
    connect(m_decreaseFontSizeAction, &QAction::triggered, this, [this]() {
        QFont font = Application::instance()->settings()->font();
        if (font.pointSize() > 6) {
            font.setPointSize(font.pointSize() - 1);
            Application::instance()->settings()->setEditorFont(font);
            updateUiForFont(font);
        }
    });
    connect(m_resetFontSizeAction, &QAction::triggered, this, [this]() {
        QFont font = Application::instance()->settings()->font();
        font.setPointSize(10);
        Application::instance()->settings()->setEditorFont(font);
        updateUiForFont(font);
    });
    
    // Help action
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::setupToolBar()
{
    QToolBar *toolBar = addToolBar(tr("Main Toolbar"));
    toolBar->setMovable(false);
    toolBar->setIconSize(QSize(16, 16));
    
    // File actions
    toolBar->addAction(m_newFileAction);
    toolBar->addAction(m_openFileAction);
    toolBar->addAction(m_saveAction);
    
    // Separator
    toolBar->addSeparator();
    
    // Edit actions
    QAction *undoAction = toolBar->addAction(QIcon(":/icons/edit-undo.svg"), tr("Undo"));
    undoAction->setShortcut(QKeySequence::Undo);
    
    QAction *redoAction = toolBar->addAction(QIcon(":/icons/edit-redo.svg"), tr("Redo"));
    redoAction->setShortcut(QKeySequence::Redo);
    
    toolBar->addSeparator();
    
    QAction *cutAction = toolBar->addAction(QIcon(":/icons/edit-cut.svg"), tr("Cut"));
    cutAction->setShortcut(QKeySequence::Cut);
    
    QAction *copyAction = toolBar->addAction(QIcon(":/icons/edit-copy.svg"), tr("Copy"));
    copyAction->setShortcut(QKeySequence::Copy);
    
    QAction *pasteAction = toolBar->addAction(QIcon(":/icons/edit-paste.svg"), tr("Paste"));
    pasteAction->setShortcut(QKeySequence::Paste);
    
    // Separator
    toolBar->addSeparator();
    
    // View actions
    toolBar->addAction(m_toggleFileBrowserAction);
    toolBar->addAction(m_togglePreviewAction);
    toolBar->addAction(m_fullScreenAction);
    
    // Connect edit actions
    connect(undoAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->undo();
    });
    
    connect(redoAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->redo();
    });
    
    connect(cutAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->cut();
    });
    
    connect(copyAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->copy();
    });
    
    connect(pasteAction, &QAction::triggered, this, [this]() {
        if (auto editor = currentEditor()) editor->paste();
    });
}

void MainWindow::setupDockWidgets()
{
    // Set up file browser
    m_fileSystemModel->setRootPath("");
    m_fileSystemModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files);
    
    QStringList filters = {"*.html", "*.htm", "*.css", "*.js", "*.json", "*.md", "*.txt"};
    m_fileSystemModel->setNameFilters(filters);
    m_fileSystemModel->setNameFilterDisables(false);
    
    m_fileBrowser->setModel(m_fileSystemModel);
    m_fileBrowser->setRootIndex(m_fileSystemModel->index(""));
    m_fileBrowser->setAnimated(false);
    m_fileBrowser->setIndentation(20);
    m_fileBrowser->setSortingEnabled(true);
    m_fileBrowser->sortByColumn(0, Qt::AscendingOrder);
    m_fileBrowser->setHeaderHidden(true);
    
    // Set up main splitter
    m_mainSplitter->addWidget(m_fileBrowser);
    m_mainSplitter->addWidget(m_tabWidget);
    m_mainSplitter->addWidget(m_webView);
    
    // Set stretch factors
    m_mainSplitter->setStretchFactor(0, 1);
    m_mainSplitter->setStretchFactor(1, 3);
    m_mainSplitter->setStretchFactor(2, 2);
    
    // Set initial sizes
    QList<int> sizes = {200, 400, 200};
    m_mainSplitter->setSizes(sizes);
    
    setCentralWidget(m_mainSplitter);
    
    // Set tab widget properties
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setDocumentMode(true);
    m_tabWidget->setMovable(true);
}

void MainWindow::setupStatusBar()
{
    statusBar()->addPermanentWidget(m_statusLabel);
    m_statusLabel->setText(tr("Ready"));
}

void MainWindow::setupConnections()
{
    // File actions
    connect(m_newFileAction, &QAction::triggered, this, &MainWindow::newFile);
    connect(m_openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_openFolderAction, &QAction::triggered, this, &MainWindow::openFolder);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(m_closeTabAction, &QAction::triggered, this, [this]() {
        closeTab(m_tabWidget->currentIndex());
    });
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // View actions
    connect(m_togglePreviewAction, &QAction::toggled, this, [this](bool visible) {
        m_isPreviewVisible = visible;
        m_webView->setVisible(visible);
    });
    
    connect(m_toggleFileBrowserAction, &QAction::toggled, this, [this](bool visible) {
        m_isFileBrowserVisible = visible;
        m_fileBrowser->setVisible(visible);
    });
    
    connect(m_fullScreenAction, &QAction::toggled, this, [this](bool fullScreen) {
        if (fullScreen) {
            showFullScreen();
        } else {
            showNormal();
        }
    });
    
    // Theme actions
    connect(m_lightThemeAction, &QAction::triggered, this, [this]() {
        Application::instance()->settings()->setTheme("light");
    });
    
    connect(m_darkThemeAction, &QAction::triggered, this, [this]() {
        Application::instance()->settings()->setTheme("dark");
    });
    
    connect(m_systemThemeAction, &QAction::triggered, this, [this]() {
        Application::instance()->settings()->setTheme("system");
    });
    
    // Font actions
    connect(m_increaseFontSizeAction, &QAction::triggered, this, [this]() {
        QFont font = Application::instance()->settings()->font();
        font.setPointSize(font.pointSize() + 1);
        Application::instance()->settings()->setEditorFont(font);
        updateUiForFont(font);
    });
    
    connect(m_decreaseFontSizeAction, &QAction::triggered, this, [this]() {
        QFont font = Application::instance()->settings()->font();
        if (font.pointSize() > 6) {
            font.setPointSize(font.pointSize() - 1);
            Application::instance()->settings()->setEditorFont(font);
            updateUiForFont(font);
        }
    });
    
    connect(m_resetFontSizeAction, &QAction::triggered, this, [this]() {
        QFont font = Application::instance()->settings()->font();
        font.setPointSize(10);
        Application::instance()->settings()->setEditorFont(font);
        updateUiForFont(font);
    });
    
    // Preferences
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);
    
    // Tab widget
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::currentTabChanged);
    
    // File browser
    connect(m_fileBrowser, &QTreeView::doubleClicked, this, &MainWindow::fileDoubleClicked);
    
    // Connect to application settings changes
    Settings *settings = Application::instance()->settings();
    connect(settings, &Settings::themeChanged, this, &MainWindow::updateUiForTheme);
    connect(settings, &Settings::editorFontChanged, this, &MainWindow::updateUiForFont);
}

void MainWindow::loadSettings()
{
    Settings *appSettings = Application::instance()->settings();
    
    // Load window geometry and state
    QSettings settings("QtWebEditor", "QtWebEditor");
    settings.beginGroup("MainWindow");
    
    // Restore window geometry and state
    if (settings.contains("geometry") && settings.contains("windowState")) {
        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("windowState").toByteArray());
    } else {
        // Default size and position
        resize(1024, 768);
        QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
        move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);
    }
    
    // Restore window state (maximized, fullscreen, etc.)
    if (settings.contains("isMaximized") && settings.value("isMaximized").toBool()) {
        showMaximized();
    } else if (settings.contains("isFullScreen") && settings.value("isFullScreen").toBool()) {
        showFullScreen();
    }
    
    // Restore splitter state
    if (settings.contains("splitterState")) {
        m_mainSplitter->restoreState(settings.value("splitterState").toByteArray());
    } else {
        // Default splitter sizes
        QList<int> sizes;
        sizes << 250 << width() - 250; // 250px for file browser, rest for editor
        m_mainSplitter->setSizes(sizes);
    }
    
    // Load view states with sensible defaults
    m_isPreviewVisible = settings.value("previewVisible", true).toBool();
    m_isFileBrowserVisible = settings.value("fileBrowserVisible", true).toBool();
    
    // Apply view states
    m_togglePreviewAction->setChecked(m_isPreviewVisible);
    m_webView->setVisible(m_isPreviewVisible);
    
    m_toggleFileBrowserAction->setChecked(m_isFileBrowserVisible);
    m_fileBrowser->setVisible(m_isFileBrowserVisible);
    
    // Load last opened folder
    m_currentFolder = settings.value("lastFolder", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();
    
    // Load recent files
    QStringList recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFilesMenu(recentFiles);
    
    // Apply theme and font settings
    applyThemeAndFontSettings();
    
    settings.endGroup();
}

void MainWindow::applyThemeAndFontSettings()
{
    Settings *appSettings = Application::instance()->settings();
    
    // Load and apply theme
    QString theme = appSettings->themeName();
    
    // Update theme actions
    m_lightThemeAction->setChecked(theme == "light");
    m_darkThemeAction->setChecked(theme == "dark");
    m_systemThemeAction->setChecked(theme == "system");
    
    // Load and apply font
    QFont font = appSettings->font();
    
    // Update UI for current theme and font
    updateUiForTheme();
    updateUiForFont(font);
    
    // Update the application font
    qApp->setFont(font);
}

void MainWindow::saveSettings()
{
    QSettings settings("QtWebEditor", "QtWebEditor");
    
    settings.beginGroup("MainWindow");
    
    // Save window geometry and state
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    
    // Save window state (maximized, fullscreen, etc.)
    settings.setValue("isMaximized", isMaximized());
    settings.setValue("isFullScreen", isFullScreen());
    
    // Save splitter state
    settings.setValue("splitterState", m_mainSplitter->saveState());
    
    // Save view states
    settings.setValue("previewVisible", m_isPreviewVisible);
    settings.setValue("fileBrowserVisible", m_isFileBrowserVisible);
    
    // Save last opened folder
    settings.setValue("lastFolder", m_currentFolder);
    
    // Save recent files
    QStringList recentFiles;
    for (const QAction *action : m_recentFileActions) {
        QString file = action->data().toString();
        if (!file.isEmpty() && QFile::exists(file)) {
            recentFiles.append(file);
        }
    }
    settings.setValue("recentFiles", recentFiles);
    
    settings.endGroup();
    
    // Save application settings (theme, font, etc.)
    Application::instance()->settings()->save();
}

void MainWindow::newFile()
{
    createNewEditorTab();
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, 
        tr("Open File"), 
        m_currentFolder,
        tr("Web Files (*.html *.htm *.css *.js *.json);;All Files (*)"));
    
    if (!filePath.isEmpty()) {
        // Update the current folder to the parent directory of the selected file
        QFileInfo fileInfo(filePath);
        m_currentFolder = fileInfo.absolutePath();
        
        // Open the file in the editor
        openFileInEditor(filePath);
    }
}

void MainWindow::openFolder()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, 
        tr("Open Folder"), 
        m_currentFolder);
    
    if (!folderPath.isEmpty()) {
        m_currentFolder = folderPath;
        m_fileBrowser->setRootIndex(m_fileSystemModel->index(folderPath));
        Application::instance()->settings()->setLastOpenedPath(folderPath);
    }
}

void MainWindow::saveFile()
{
    auto editor = currentEditor();
    if (!editor) {
        return;
    }
    
    if (editor->filePath().isEmpty()) {
        saveFileAs();
        return;
    } else {
        if (editor->save()) {
            // Add to recent files
            QString filePath = editor->filePath();
            QStringList recentFiles = Application::instance()->settings()->recentFiles();
            recentFiles.removeAll(filePath);
            recentFiles.prepend(filePath);
            while (recentFiles.size() > 10) {
                recentFiles.removeLast();
            }
            Application::instance()->settings()->setRecentFiles(recentFiles);
            
            // Update the recent files menu
            updateRecentFilesMenu(recentFiles);
            
            statusBar()->showMessage(tr("File saved successfully"), 3000);
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to save file."));
        }
    }
}

void MainWindow::saveFileAs()
{
    auto editor = currentEditor();
    if (!editor) {
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(this, 
        tr("Save File"), 
        m_currentFolder,
        tr("Web Files (*.html *.htm *.css *.js *.json);;All Files (*)"));
    
    if (!filePath.isEmpty()) {
        // Update the current folder to the parent directory of the saved file
        QFileInfo fileInfo(filePath);
        m_currentFolder = fileInfo.absolutePath();
        
        if (editor->saveAs(filePath)) {
            // Add to recent files
            QStringList recentFiles = Application::instance()->settings()->recentFiles();
            recentFiles.removeAll(filePath);
            recentFiles.prepend(filePath);
            while (recentFiles.size() > 10) {
                recentFiles.removeLast();
            }
            Application::instance()->settings()->setRecentFiles(recentFiles);
            
            // Update the recent files menu
            updateRecentFilesMenu(recentFiles);
            
            updateWindowTitle();
            statusBar()->showMessage(tr("File saved successfully"), 3000);
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to save file."));
        }
    }
}

void MainWindow::updateUiForFont(const QFont &font)
{
    // Apply font to all editors
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        if (auto editor = qobject_cast<EditorWidget*>(m_tabWidget->widget(i))) {
            editor->setFont(font);
        }
    }
    
    // Update font in file browser
    QFont fileBrowserFont = font;
    fileBrowserFont.setPointSize(font.pointSize() - 1);
    m_fileBrowser->setFont(fileBrowserFont);
    
    // Update status bar font
    statusBar()->setFont(font);
}

void MainWindow::showPreferences()
{
    // Create a simple preferences dialog
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Preferences"));
    dialog.setMinimumWidth(500);
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    // Theme selection
    QGroupBox *themeGroup = new QGroupBox(tr("Theme"), &dialog);
    QVBoxLayout *themeLayout = new QVBoxLayout(themeGroup);
    
    QButtonGroup *themeButtonGroup = new QButtonGroup(themeGroup);
    QRadioButton *lightThemeBtn = new QRadioButton(tr("Light"), themeGroup);
    QRadioButton *darkThemeBtn = new QRadioButton(tr("Dark"), themeGroup);
    QRadioButton *systemThemeBtn = new QRadioButton(tr("System"), themeGroup);
    
    themeButtonGroup->addButton(lightThemeBtn, 0);
    themeButtonGroup->addButton(darkThemeBtn, 1);
    themeButtonGroup->addButton(systemThemeBtn, 2);
    
    // Set the current theme radio button
    Settings::Theme currentTheme = Application::instance()->settings()->theme();
    switch (currentTheme) {
        case Settings::Theme::Light:
            lightThemeBtn->setChecked(true);
            break;
        case Settings::Theme::Dark:
            darkThemeBtn->setChecked(true);
            break;
        case Settings::Theme::System:
        default:
            systemThemeBtn->setChecked(true);
            break;
    }
    
    themeLayout->addWidget(lightThemeBtn);
    themeLayout->addWidget(darkThemeBtn);
    themeLayout->addWidget(systemThemeBtn);
    
    // Font selection
    QGroupBox *fontGroup = new QGroupBox(tr("Editor Font"), &dialog);
    QHBoxLayout *fontLayout = new QHBoxLayout(fontGroup);
    
    QFont currentFont = Application::instance()->settings()->font();
    QLabel *fontLabel = new QLabel(currentFont.family() + " " + QString::number(currentFont.pointSize()) + "pt", fontGroup);
    QPushButton *fontButton = new QPushButton(tr("Change..."), fontGroup);
    
    fontLayout->addWidget(fontLabel, 1);
    fontLayout->addWidget(fontButton);
    
    // Dialog buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    
    layout->addWidget(themeGroup);
    layout->addWidget(fontGroup);
    layout->addStretch();
    layout->addWidget(buttonBox);
    
    // Connect signals
    connect(fontButton, &QPushButton::clicked, [&]() {
        bool ok;
        QFont newFont = QFontDialog::getFont(&ok, currentFont, this, tr("Select Editor Font"));
        if (ok) {
            currentFont = newFont;
            fontLabel->setText(currentFont.family() + " " + QString::number(currentFont.pointSize()) + "pt");
        }
    });
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    // Show the dialog
    if (dialog.exec() == QDialog::Accepted) {
        // Apply theme
        if (lightThemeBtn->isChecked()) {
            Application::instance()->settings()->setTheme(Settings::Theme::Light);
        } else if (darkThemeBtn->isChecked()) {
            Application::instance()->settings()->setTheme(Settings::Theme::Dark);
        } else {
            Application::instance()->settings()->setTheme(Settings::Theme::System);
        }
        
        // Apply font if changed
        if (currentFont != Application::instance()->settings()->font()) {
            Application::instance()->settings()->setEditorFont(currentFont);
        }
        
        // Apply changes
        updateUiForTheme();
        updateUiForFont(Application::instance()->settings()->font());
    }
}

void MainWindow::toggleFullScreen()
{
    static bool wasMaximized = false;
    static QRect normalGeometry;
    
    if (isFullScreen()) {
        // Exiting fullscreen
        if (wasMaximized) {
            showMaximized();
        } else {
            showNormal();
            if (normalGeometry.isValid()) {
                setGeometry(normalGeometry);
            }
        }
        menuBar()->show();
        statusBar()->show();
        m_fullScreenAction->setChecked(false);
        
        // Restore toolbars and dock widgets visibility
        if (m_toolBar) m_toolBar->show();
        if (m_fileBrowserDock && m_isFileBrowserVisible) m_fileBrowserDock->show();
        
    } else {
        // Entering fullscreen
        wasMaximized = isMaximized();
        normalGeometry = geometry();
        
        // Save current window state
        m_wasMenuBarVisible = menuBar()->isVisible();
        m_wasStatusBarVisible = statusBar()->isVisible();
        
        // Hide UI elements
        menuBar()->hide();
        statusBar()->hide();
        if (m_toolBar) m_toolBar->hide();
        if (m_fileBrowserDock) m_fileBrowserDock->hide();
        
        // Enter fullscreen
        showFullScreen();
        m_fullScreenAction->setChecked(true);
    }
    
    // Update the UI
    update();
}

// This method is a duplicate and has been removed. Using the implementation at the end of the file.

void MainWindow::updateRecentFilesMenu(const QStringList &recentFiles)
{
    if (!m_fileMenu) {
        return; // Menu not initialized yet
    }
    
    // Clear existing recent file actions
    for (QAction *action : m_recentFileActions) {
        m_fileMenu->removeAction(action);
        delete action;
    }
    m_recentFileActions.clear();
    
    // Add recent files to the menu
    for (const QString &file : recentFiles) {
        if (QFile::exists(file)) {
            QFileInfo fileInfo(file);
            QAction *action = new QAction(fileInfo.fileName(), this);
            action->setData(file);
            action->setToolTip(file); // Show full path in tooltip
            connect(action, &QAction::triggered, this, [this, file]() {
                openFileInEditor(file);
            });
            m_recentFileActions.append(action);
        }
    }
    
    // Add recent files to the file menu if there are any
    if (!m_recentFileActions.isEmpty()) {
        // Add a separator before the recent files if there isn't one already
        QList<QAction*> actions = m_fileMenu->actions();
        if (!actions.isEmpty() && !actions.last()->isSeparator()) {
            m_fileMenu->addSeparator();
        }
        
        // Add the recent files
        m_fileMenu->addActions(m_recentFileActions);
    }
}

void MainWindow::updateUiForTheme()
{
    // Apply current theme to UI elements
    Settings::Theme theme = Application::instance()->settings()->theme();
    
    // Update theme menu check state
    m_lightThemeAction->setChecked(theme == Settings::Theme::Light);
    m_darkThemeAction->setChecked(theme == Settings::Theme::Dark);
    m_systemThemeAction->setChecked(theme == Settings::Theme::System);
    
    // Apply theme-specific styles
    QString styleSheet;
    if (theme == Settings::Theme::Dark) {
        // Dark theme styles
        styleSheet = "";
    } else {
        // Light theme styles
        styleSheet = "";
    }
    
    // Apply styles to the main window and its children
    setStyleSheet(styleSheet);
    
    // Update UI components that need special theming
    update();
}

void MainWindow::updateWindowTitle()
{
    QString title = "Qt Web Editor";
    EditorWidget *editor = currentEditor();
    
    if (editor && !editor->filePath().isEmpty()) {
        QFileInfo fileInfo(editor->filePath());
        title = fileInfo.fileName() + " - " + title;
        
        if (editor->document()->isModified()) {
            title.prepend("*");
        }
    }
    
    setWindowTitle(title);
}

void MainWindow::currentTabChanged(int index)
{
    Q_UNUSED(index);
    updateWindowTitle();
    updatePreview();
}

void MainWindow::fileDoubleClicked(const QModelIndex &index)
{
    if (m_fileSystemModel->isDir(index)) {
        return; // Don't open directories here
    }
    
    QString filePath = m_fileSystemModel->filePath(index);
    openFileInEditor(filePath);
}

void MainWindow::closeTab(int index)
{
    if (index < 0 || index >= m_tabWidget->count()) {
        return;
    }
    
    EditorWidget *editor = qobject_cast<EditorWidget*>(m_tabWidget->widget(index));
    if (!editor) {
        return;
    }
    
    if (maybeSave(editor)) {
        m_tabWidget->removeTab(index);
        editor->deleteLater();
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, tr("About Qt Web Editor"),
        tr("<h2>Qt Web Editor</h2>"
           "<p>Version 1.0.0</p>"
           "<p>A simple web development editor built with Qt</p>"
           "<p>© 2023 Your Company</p>"));
}

void MainWindow::updatePreview()
{
    auto editor = currentEditor();
    if (!editor || !m_webView) {
        return;
    }
    
    QString filePath = editor->filePath();
    if (filePath.endsWith(".html", Qt::CaseInsensitive) || 
        filePath.endsWith(".htm", Qt::CaseInsensitive)) {
        
        // For HTML files, load the file directly
        m_webView->setUrl(QUrl::fromLocalFile(filePath));
    } else if (filePath.endsWith(".css", Qt::CaseInsensitive)) {
        // For CSS files, create a simple HTML wrapper
        QString html = QString(
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"UTF-8\">\n"
            "<title>%1</title>\n"
            "<style>%2</style>\n"
            "</head>\n"
            "<body>\n"
            "<h1>%1</h1>\n"
            "<p>This is a preview of your CSS file.</p>\n"
            "</body>\n"
            "</html>"
        ).arg(QFileInfo(filePath).fileName(), editor->toPlainText());
        
        m_webView->setHtml(html, QUrl::fromLocalFile(filePath));
    } else if (filePath.endsWith(".js", Qt::CaseInsensitive)) {
        // For JS files, create a simple HTML wrapper
        QString html = QString(
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"UTF-8\">\n"
            "<title>%1</title>\n"
            "<script>\n"
            "// JavaScript preview\n"
            "document.addEventListener('DOMContentLoaded', function() {\n"
            "    var output = document.getElementById('output');\n"
            "    try {\n"
            "        %2\n"
            "        output.innerHTML = 'JavaScript executed successfully.';\n"
            "    } catch (e) {\n"
            "        output.innerHTML = 'Error: ' + e.message;\n"
            "    }\n"
            "});\n"
            "</script>\n"
            "</head>\n"
            "<body>\n"
            "<h1>%1</h1>\n"
            "<div id=\"output\">Running JavaScript preview...</div>\n"
            "</body>\n"
            "</html>"
        ).arg(QFileInfo(filePath).fileName(), editor->toPlainText());
        
        m_webView->setHtml(html, QUrl::fromLocalFile(filePath));
    } else {
        // For other file types, just show the text content
        m_webView->setHtml(QString(
            "<html><body><pre>%1</pre></body></html>"
        ).arg(editor->toPlainText().toHtmlEscaped()));
    }
}

EditorWidget *MainWindow::currentEditor() const
{
    if (!m_tabWidget) {
        return nullptr;
    }
    return qobject_cast<EditorWidget*>(m_tabWidget->currentWidget());
}

EditorWidget *MainWindow::editorForPath(const QString &filePath) const
{
    if (!m_tabWidget) {
        return nullptr;
    }
    
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        if (auto editor = qobject_cast<EditorWidget*>(m_tabWidget->widget(i))) {
            if (editor->filePath() == filePath) {
                return editor;
            }
        }
    }
    return nullptr;
}

void MainWindow::openFileInEditor(const QString &filePath)
{
    if (filePath.isEmpty()) {
        return;
    }
    
    // Check if file is already open
    auto existingEditor = editorForPath(filePath);
    if (existingEditor) {
        m_tabWidget->setCurrentWidget(existingEditor);
        return;
    }
    
    // Create new editor tab
    EditorWidget *editor = nullptr;
    createNewEditorTab(filePath);
    
    // Get the newly created editor
    editor = qobject_cast<EditorWidget*>(m_tabWidget->currentWidget());
    if (!editor) {
        return;
    }
    
    // Load file content
    if (!editor->load(filePath)) {
        QMessageBox::critical(this, tr("Error"), 
            tr("Could not open file: %1").arg(filePath));
        closeTab(m_tabWidget->indexOf(editor));
        return;
    }
    
    // Update current folder
    QFileInfo fileInfo(filePath);
    m_currentFolder = fileInfo.absolutePath();
    Application::instance()->settings()->setLastOpenedPath(m_currentFolder);
    
    // Update file browser
    if (m_fileBrowser) {
        m_fileBrowser->setRootIndex(m_fileSystemModel->index(m_currentFolder));
    }
    
    // Update window title
    updateWindowTitle();
    
    // Update preview
    updatePreview();
    
    // Add to recent files
    QStringList recentFiles = Application::instance()->settings()->recentFiles();
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    while (recentFiles.size() > 10) {
        recentFiles.removeLast();
    }
    Application::instance()->settings()->setRecentFiles(recentFiles);
    updateRecentFilesMenu(recentFiles);
}

bool MainWindow::maybeSave(EditorWidget *editor)
{
    if (!editor || !editor->isModified()) {
        return true;
    }
    
    const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Document Modified"),
        tr("The document has been modified.\nDo you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    
    if (ret == QMessageBox::Save) {
        if (editor->filePath().isEmpty()) {
            saveFileAs();
            return true;
        } else {
            return editor->save();
        }
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    
    return true;
}

void MainWindow::createNewEditorTab(const QString &filePath)
{
    auto editor = new EditorWidget(this);
    
    // Connect editor signals
    connect(editor, &EditorWidget::modificationChanged, [this, editor](bool changed) {
        int index = m_tabWidget->indexOf(editor);
        if (index >= 0) {
            QString tabText = m_tabWidget->tabText(index);
            if (tabText.startsWith('*') != changed) {
                m_tabWidget->setTabText(index, changed ? "*" + tabText : tabText.mid(1));
            }
            
            if (editor == currentEditor()) {
                if (m_saveAction) {
                    m_saveAction->setEnabled(changed);
                }
                updateWindowTitle();
            }
        }
    });
    
    connect(editor, &EditorWidget::textChanged, this, &MainWindow::updatePreview);
    
    // Add tab
    QString tabText = filePath.isEmpty() ? "Untitled" : QFileInfo(filePath).fileName();
    int index = m_tabWidget->addTab(editor, tabText);
    m_tabWidget->setCurrentIndex(index);
    
    // Set file path if provided
    if (!filePath.isEmpty()) {
        editor->setFilePath(filePath);
    }
}
