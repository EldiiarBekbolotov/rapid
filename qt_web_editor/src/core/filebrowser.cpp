/**
 * @file filebrowser.cpp
 * @brief Implementation of the FileBrowser class.
 * 
 * This file contains the implementation of the file system browser widget
 * that allows users to navigate and manipulate files and directories.
 */

#include "filebrowser.h"
#include <QFileSystemModel>
#include <QHeaderView>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

/**
 * @brief Constructs a FileBrowser with the given parent.
 * 
 * Initializes the file system model, sets up the view properties, and
 * connects the necessary signals and slots.
 * 
 * @param parent The parent widget.
 */
FileBrowser::FileBrowser(QWidget *parent)
    : QTreeView(parent)
    , m_model(new QFileSystemModel(this))
    , m_contextMenu(new QMenu(this))
{
    // Set up the model
    m_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files);
    m_model->setRootPath("");
    
    // Set up the view
    setModel(m_model);
    setRootIsDecorated(true);
    setAnimated(false);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    setHeaderHidden(true);
    
    // Hide unnecessary columns
    for (int i = 1; i < m_model->columnCount(); ++i) {
        hideColumn(i);
    }
    
    // Set up context menu
    setupActions();
    setupMenu();
    
    // Connect signals
    connect(this, &FileBrowser::doubleClicked, this, &FileBrowser::onDoubleClicked);
}

/**
 * @brief Destroys the FileBrowser and cleans up resources.
 */
FileBrowser::~FileBrowser()
{
    delete m_contextMenu;
}

/**
 * @brief Sets the root directory to be displayed in the browser.
 * 
 * If the specified path is valid, updates the view to show the contents
 * of that directory and emits the rootPathChanged signal.
 * 
 * @param path The file system path to set as root.
 */
void FileBrowser::setRootPath(const QString &path)
{
    QModelIndex index = m_model->index(path);
    if (index.isValid()) {
        setRootIndex(index);
        emit rootPathChanged(path);
    }
}

/**
 * @brief Gets the current directory path.
 * 
 * @return The current directory path as a string.
 */
QString FileBrowser::currentPath() const
{
    return m_model->filePath(rootIndex());
}

/**
 * @brief Handles context menu events for the file browser.
 * 
 * Shows a context menu with appropriate actions based on the selected item.
 * 
 * @param event The context menu event.
 */
void FileBrowser::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    bool isFile = m_model->fileInfo(index).isFile();
    bool isDir = m_model->fileInfo(index).isDir();
    
    m_renameAction->setEnabled(index.isValid());
    m_deleteAction->setEnabled(index.isValid());
    m_showInExplorerAction->setEnabled(index.isValid());
    
    m_contextMenu->exec(event->globalPos());
}

/**
 * @brief Handles double-click events on items in the browser.
 * 
 * If the double-clicked item is a file, emits the fileDoubleClicked signal
 * with the file's path.
 * 
 * @param index The model index of the clicked item.
 */
void FileBrowser::onDoubleClicked(const QModelIndex &index)
{
    if (m_model->fileInfo(index).isFile()) {
        emit fileDoubleClicked(m_model->filePath(index));
    }
}

/**
 * @brief Creates a new file in the current directory.
 * 
 * Shows an input dialog to get the file name, then creates an empty file
 * with that name in the current directory.
 */
void FileBrowser::createNewFile()
{
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("New File"),
                                           tr("File name:"), QLineEdit::Normal,
                                           "newfile.txt", &ok);
    
    if (ok && !fileName.isEmpty()) {
        QModelIndex current = rootIndex();
        QString dirPath = m_model->filePath(current);
        QString filePath = QDir(dirPath).filePath(fileName);
        
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, tr("Error"), 
                                tr("Could not create file %1").arg(filePath));
            return;
        }
        file.close();
        
        // Select the new file
        QModelIndex index = m_model->index(filePath);
        if (index.isValid()) {
            setCurrentIndex(index);
            emit fileDoubleClicked(filePath);
        }
    }
}

/**
 * @brief Creates a new subdirectory in the current directory.
 * 
 * Shows an input dialog to get the directory name, then creates a new
 * subdirectory with that name in the current directory.
 */
void FileBrowser::createNewFolder()
{
    bool ok;
    QString folderName = QInputDialog::getText(this, tr("New Folder"),
                                             tr("Folder name:"), QLineEdit::Normal,
                                             tr("New Folder"), &ok);
    
    if (ok && !folderName.isEmpty()) {
        QModelIndex current = rootIndex();
        QString dirPath = m_model->filePath(current);
        QString newDirPath = QDir(dirPath).filePath(folderName);
        
        QDir dir;
        if (!dir.mkpath(newDirPath)) {
            QMessageBox::critical(this, tr("Error"),
                                tr("Could not create directory %1").arg(newDirPath));
        }
    }
}

/**
 * @brief Initiates renaming of the currently selected item.
 * 
 * If a single item is selected, puts the item into edit mode to allow
 * the user to rename it.
 */
void FileBrowser::rename()
{
    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return;
    }
    
    QString oldPath = m_model->filePath(index);
    QFileInfo fileInfo(oldPath);
    
    bool ok;
    QString newName = QInputDialog::getText(this, tr("Rename"),
                                          tr("New name:"), QLineEdit::Normal,
                                          fileInfo.fileName(), &ok);
    
    if (ok && !newName.isEmpty() && newName != fileInfo.fileName()) {
        QString newPath = fileInfo.dir().filePath(newName);
        
        if (QFile::rename(oldPath, newPath)) {
            // Update the view
            QModelIndex parent = index.parent();
            m_model->setRootPath("");
            setRootIndex(m_model->index(fileInfo.dir().path()));
            
            // Select the renamed item
            QModelIndex newIndex = m_model->index(newPath);
            if (newIndex.isValid()) {
                setCurrentIndex(newIndex);
            }
        } else {
            QMessageBox::critical(this, tr("Error"),
                                tr("Could not rename %1").arg(fileInfo.fileName()));
        }
    }
}

/**
 * @brief Deletes the currently selected items.
 * 
 * Shows a confirmation dialog, then deletes all selected files and
 * directories recursively if confirmed.
 */
void FileBrowser::remove()
{
    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return;
    }
    
    QString path = m_model->filePath(index);
    QFileInfo fileInfo(path);
    
    QString message = tr("Are you sure you want to delete %1?").arg(fileInfo.fileName());
    if (fileInfo.isDir()) {
        message += tr("\nThis will also delete all files and subdirectories.");
    }
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm Delete"),
                                message,
                                QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        bool success = false;
        
        if (fileInfo.isDir()) {
            QDir dir(path);
            success = dir.removeRecursively();
        } else {
            success = QFile::remove(path);
        }
        
        if (!success) {
            QMessageBox::critical(this, tr("Error"),
                                tr("Could not delete %1").arg(fileInfo.fileName()));
        }
    }
}

/**
 * @brief Opens the system file explorer at the location of the selected item.
 * 
 * Uses the platform-specific method to open the system's file explorer
 * with the selected file or directory highlighted.
 */
void FileBrowser::showInExplorer()
{
    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return;
    }
    
    QString path = m_model->filePath(index);
    QFileInfo fileInfo(path);
    
    // On macOS
    QStringList args;
    args << "-e" << "tell application \"Finder\"";
    args << "-e" << "reveal POSIX file \"" + path + "\"";
    args << "-e" << "activate";
    args << "-e" << "end tell";
    
    QProcess::startDetached("osascript", args);
}

/**
 * @brief Initializes the actions used in the context menu.
 * 
 * Creates and configures all QAction objects with their respective
 * shortcuts, icons, and connections.
 */
void FileBrowser::setupActions()
{
    m_newFileAction = new QAction(tr("New File"), this);
    m_newFileAction->setIcon(QIcon::fromTheme("document-new"));
    connect(m_newFileAction, &QAction::triggered, this, &FileBrowser::createNewFile);
    
    m_newFolderAction = new QAction(tr("New Folder"), this);
    m_newFolderAction->setIcon(QIcon::fromTheme("folder-new"));
    connect(m_newFolderAction, &QAction::triggered, this, &FileBrowser::createNewFolder);
    
    m_renameAction = new QAction(tr("Rename"), this);
    m_renameAction->setIcon(QIcon::fromTheme("edit-rename"));
    connect(m_renameAction, &QAction::triggered, this, &FileBrowser::rename);
    
    m_deleteAction = new QAction(tr("Delete"), this);
    m_deleteAction->setIcon(QIcon::fromTheme("edit-delete"));
    connect(m_deleteAction, &QAction::triggered, this, &FileBrowser::remove);
    
    m_showInExplorerAction = new QAction(tr("Show in Finder"), this);
    m_showInExplorerAction->setIcon(QIcon::fromTheme("document-open"));
    connect(m_showInExplorerAction, &QAction::triggered, this, &FileBrowser::showInExplorer);
}

/**
 * @brief Sets up the context menu with all available actions.
 * 
 * Adds all the actions to the context menu in the appropriate order.
 */
void FileBrowser::setupMenu()
{
    m_contextMenu->addAction(m_newFileAction);
    m_contextMenu->addAction(m_newFolderAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_renameAction);
    m_contextMenu->addAction(m_deleteAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_showInExplorerAction);
}

/**
 * @brief Gets the file system path for a given model index.
 * 
 * @param index The model index to get the path for.
 * @return The corresponding file system path.
 */
QString FileBrowser::filePath(const QModelIndex &index) const
{
    return m_model->filePath(index);
}
