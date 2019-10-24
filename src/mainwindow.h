#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void init();
    void search();

private:
    void Folder1ButtonClicked();
    void Folder2ButtonClicked();
    void searchButtonClicked();
    void itemDoubleClicked(QListWidgetItem* item);
    void CopyButtonClicked();
    void MoveButtonClicked();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
