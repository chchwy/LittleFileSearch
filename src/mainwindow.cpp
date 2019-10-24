#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <map>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include <QDirIterator>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QDesktopServices>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    QSettings settings("chchwy", "ImageSearchKing");
    ui->folder1Text->setText(settings.value("Folder1").toString());
    ui->folder2Text->setText(settings.value("Folder2").toString());

    connect(ui->folder1Button, &QPushButton::clicked, this, &MainWindow::Folder1ButtonClicked);
    connect(ui->folder2Button, &QPushButton::clicked, this, &MainWindow::Folder2ButtonClicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::searchButtonClicked);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::itemDoubleClicked);
    connect(ui->copyButton, &QPushButton::clicked, this, &MainWindow::CopyButtonClicked);
    connect(ui->moveButton, &QPushButton::clicked, this, &MainWindow::MoveButtonClicked);

    ui->progressBar->setVisible(false);
}

void MainWindow::search()
{
    ui->listWidget->clear();

    QStringList filesInFolder1;
    QStringList filesInFolder2;

    QDirIterator it(ui->folder1Text->text(), QDir::Files);
    while (it.hasNext())
    {
        QString s1 = it.next();
        filesInFolder1.append(s1);
    }

    //qDebug() << "Dir2 = " << ui->folder2Text->text();
    
    QDirIterator it2(ui->folder2Text->text(), QDir::Files);
    while (it2.hasNext())
    {
        QString s2 = it2.next();
        filesInFolder2.append(s2);
    }

    //qDebug() << "Count2 = " << filesInFolder2.size();

    std::map<QString, QFileInfo> map1;

    for (QString& s : filesInFolder1)
    {
        QFileInfo info(s);

        map1.emplace(info.baseName(), info);
    }

    for (const QString& f2 : filesInFolder2)
    {
        QFileInfo info2(f2);

        auto it = map1.find(info2.baseName());
        if (it != map1.end())
        {
            //qDebug() << "Got u!" << info2.baseName();

            QFileInfo& info1 = it->second;
            ui->listWidget->addItem(info1.absoluteFilePath());
        }
    }
    int totalCount = ui->listWidget->count();
    ui->statusBar->showMessage(QString("%1 files match").arg(totalCount));
}

void MainWindow::Folder1ButtonClicked()
{
    QSettings settings("chchwy", "ImageSearchKing");

    QString initialFolder = settings.value("Folder1").toString();
    QString s = QFileDialog::getExistingDirectory(this, "Folder 1", initialFolder);
    if (!s.isEmpty())
    {
        ui->folder1Text->setText(s);
        settings.setValue("Folder1", s);
    }
}

void MainWindow::Folder2ButtonClicked()
{
    QSettings settings("chchwy", "ImageSearchKing");

    QString initialFolder = settings.value("Folder2").toString();
    QString s = QFileDialog::getExistingDirectory(this, "Folder 2", initialFolder);
    if (!s.isEmpty())
    {
        ui->folder2Text->setText(s);
        settings.setValue("Folder2", s);
    }
}

void MainWindow::searchButtonClicked()
{
    if (ui->folder1Text->text() == ui->folder2Text->text())
    {
        QMessageBox::warning(this, "Warning", "No~~~~~~");
        ui->statusBar->showMessage("2 Folders are the same!!");
        return;
    }
    ui->progressBar->setVisible(false);
    search();
}

void MainWindow::itemDoubleClicked(QListWidgetItem* item)
{
    qDebug() << item->text();
    QDesktopServices::openUrl(QUrl::fromLocalFile(item->text()));
}

void MainWindow::CopyButtonClicked()
{
    QSettings settings("chchwy", "ImageSearchKing");
    QString initialFolder = settings.value("Dest").toString();
    QString dest = QFileDialog::getExistingDirectory(this, "Destination", initialFolder);

    if (dest.isEmpty())
    {
        return;
    }

    settings.setValue("Dest", dest);
    int n = ui->listWidget->count();

    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);

    for (int i = 0; i < n; ++i)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        QString srcFile = item->text();

        QFileInfo info(srcFile);
        QString destFile = QDir(dest).filePath(info.fileName());

        if (QFile::exists(destFile))
        {
            QFile::remove(destFile);
        }
        QFile::copy(srcFile, destFile);

        ui->progressBar->setValue(float(i + 1) / float(n) * 100);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    if (n > 0)
    {
        QString msg = QString("Copy %1 files to %2").arg(n).arg(dest);
        QMessageBox::information(this, "Completed!", msg, QMessageBox::Ok);
        ui->statusBar->showMessage(msg);
    }
}

void MainWindow::MoveButtonClicked()
{
    QSettings settings("chchwy", "ImageSearchKing");
    QString dest = QFileDialog::getExistingDirectory(this, "Destination", settings.value("Dest").toString());

    if (dest.isEmpty())
    {
        return;
    }

    settings.setValue("Dest", dest);
    int n = ui->listWidget->count();

    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);

    for (int i = 0; i < n; ++i)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        QString srcFile = item->text();

        QFileInfo info(srcFile);
        QString destFile = QDir(dest).filePath(info.fileName());
        //qDebug() << srcFile << " "<< destFile;

        if (QFile::exists(destFile))
        {
            QFile::remove(destFile);
        }

        bool bCopyOK = QFile::copy(srcFile, destFile);
        if (bCopyOK)
            QFile::remove(srcFile);

        ui->progressBar->setValue(float(i + 1) / float(n) * 100);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    if (n > 0)
    {
        QString msg = QString("Move %1 files to %2").arg(n).arg(dest);
        QMessageBox::information(this, "Completed!", msg, QMessageBox::Ok);
        ui->statusBar->showMessage(msg);
    }
}
