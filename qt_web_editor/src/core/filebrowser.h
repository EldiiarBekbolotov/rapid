/**
 * @file filebrowser.h
 * @brief Declaration of the FileBrowser class.
 * 
 * This file contains the FileBrowser class which provides a tree view
 * for browsing and manipulating the file system within the editor.
 */

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QTreeView>
#include <QFileSystemModel>
#include <QContextMenuEvent>
#include <QMenu>

/**
 * @brief The FileBrowser class provides a file system browser widget.
 * 
 * This class extends QTreeView to display a hierarchical view of the file system,
 * allowing users to navigate directories and interact with files. It supports
 * common file operations like creating, renaming, and deleting files and folders.
 */
class FileBrowser : public QTreeView
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a FileBrowser with the given parent.
     * 
     * @param parent The parent widget.
     */
    explicit FileBrowser(QWidget *parent = nullptr);
    
    /**
     * @brief Destroys the FileBrowser.
     */
    ~FileBrowser() override;

    /**
     * @brief Sets the root directory to be displayed in the browser.
     * 
     * @param path The file system path to set as root.
     */
    void setRootPath(const QString &path);
    
    /**
     * @brief Gets the current directory path.
     * 
     * @return The current directory path as a string.
     */
    QString currentPath() const;

signals:
    /**
     * @brief Emitted when a file is double-clicked in the browser.
     * 
     * @param filePath The full path of the file that was double-clicked.
     */
    void fileDoubleClicked(const QString &filePath);
    
    /**
     * @brief Emitted when the root path of the browser changes.
     * 
     * @param path The new root path.
     */
    void rootPathChanged(const QString &path);

protected:
    /**
     * @brief Handles context menu events for the file browser.
     * 
     * @param event The context menu event.
     */
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    /**
     * @brief Handles double-click events on items in the browser.
     * 
     * @param index The model index of the clicked item.
     */
    void onDoubleClicked(const QModelIndex &index);
    
    /**
     * @brief Creates a new file in the current directory.
     */
    void createNewFile();
    
    /**
     * @brief Creates a new subdirectory in the current directory.
     */
    void createNewFolder();
    
    /**
     * @brief Initiates renaming of the currently selected item.
     */
    void rename();
    
    /**
     * @brief Deletes the currently selected items.
     */
    void remove();
    
    /**
     * @brief Opens the system file explorer at the location of the selected item.
     */
    void showInExplorer();

private:
    QFileSystemModel *m_model;          /**< The model representing the file system. */
    QMenu *m_contextMenu;               /**< The context menu for file operations. */
    QAction *m_newFileAction;           /**< Action for creating a new file. */
    QAction *m_newFolderAction;         /**< Action for creating a new folder. */
    QAction *m_renameAction;            /**< Action for renaming an item. */
    QAction *m_deleteAction;            /**< Action for deleting selected items. */
    QAction *m_showInExplorerAction;    /**< Action for showing an item in the system file manager. */

    /**
     * @brief Initializes the actions used in the context menu.
     */
    void setupActions();
    
    /**
     * @brief Sets up the context menu with all available actions.
     */
    void setupMenu();
    
    /**
     * @brief Gets the file system path for a given model index.
     * 
     * @param index The model index to get the path for.
     * @return The corresponding file system path.
     */
    QString filePath(const QModelIndex &index) const;
};

#endif // FILEBROWSER_H
