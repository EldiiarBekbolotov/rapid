#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QTreeView>
#include <QFileSystemModel>
#include <QContextMenuEvent>
#include <QMenu>

class FileBrowser : public QTreeView
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget *parent = nullptr);
    ~FileBrowser() override;

    void setRootPath(const QString &path);
    QString currentPath() const;

signals:
    void fileDoubleClicked(const QString &filePath);
    void rootPathChanged(const QString &path);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void createNewFile();
    void createNewFolder();
    void rename();
    void remove();
    void showInExplorer();

private:
    QFileSystemModel *m_model;
    QMenu *m_contextMenu;
    QAction *m_newFileAction;
    QAction *m_newFolderAction;
    QAction *m_renameAction;
    QAction *m_deleteAction;
    QAction *m_showInExplorerAction;

    void setupActions();
    void setupMenu();
    QString filePath(const QModelIndex &index) const;
};

#endif // FILEBROWSER_H
