#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QMessageBox::critical(this, "Read Me!", "Before you do anything, check that your TF2 is using Proton!\n"
                                            "You can check this by going to\n"
                                            "Steam -> Team Fortress 2 -> Properties -> Compatibility\n"
                                            "If it's enabled, be sure to remember the Proton you're using!");
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    // file paths
    QString tfpath = ui->tfPath->text();
    QString lboxpath = ui->loaderPath->text();

    if (ui->tfPath->text().isEmpty() || ui->loaderPath->text().isEmpty()) {
        QMessageBox::critical(this, "Error!", "Your TF2 and/or LMAOBOX path is empty!");
        return;
    }

    if (ui->protonPath->text().trimmed().isEmpty()) {
        QMessageBox::critical(this, "Error!", "Your Proton path is empty!");
        return;
    }

    QString filter = "Bat file (*.bat)";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save .bat file"), QDir::currentPath(), filter);
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // launch options
        QString tfOptions = ui->tfOptions->placeholderText();
        QString loaderOptions = ui->loaderOptions->placeholderText();

        if (!ui->tfOptions->text().isEmpty())
            tfOptions = ui->tfOptions->text();

        if (!ui->loaderOptions->text().isEmpty())
            loaderOptions = ui->loaderOptions->text();

        tfpath.replace("/", "\\");
        lboxpath.replace("/", "\\");

        out << "start \"\" cmd /c \"Z:" + tfpath + "\" " + tfOptions + "\n";
        out << "start \"\" cmd /c \"Z:" + lboxpath + "\" " + loaderOptions;

        file.close();

        QFileInfo info(fileName);
        QString dir = info.absolutePath();

        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText("\"" + ui->protonPath->text() + "/proton\" run \"" + fileName + "\" ; # %command%");

        QFile launchFile(dir + "/launch-options.txt");
        if (launchFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out2(&launchFile);

            out2 << "\"" + ui->protonPath->text() + "/proton\" run \"" + fileName + "\" ; # %command%";

            launchFile.close();

            QMessageBox::information(this, "Success!", "Copied to clipboard the launch options for tf2!\n"
                                                       "Place them in\n"
                                                       "Steam -> Team Fortress 2 -> Properties -> Launch Options\n"
                                                       "The copied launch options were saved in the same folder as the .bat file!");
        }
    }
}


void MainWindow::on_pushButton_clicked()
{
    ui->loaderPath->setText(QFileDialog::getOpenFileName(this, "Select LMAOBOX loader .exe", QDir::currentPath(), "Executable (*.exe)"));
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->tfPath->setText(QFileDialog::getExistingDirectory(this, "Select TF2's root folder (the one that has tf_win64.exe)", QDir::currentPath()));
}


void MainWindow::on_pushButton_4_clicked()
{
    QMessageBox::information(this, "Read Me", "Search for the directory of the Proton your TF2 is using!\n\n"
                                              "For custom Proton installs (like Proton-GE), it probably is in\n"
                                              "~/.steam/steam/compatibilitytools.d/\n\n"
                                              "If you're using normal Valve's Proton (the default), it probably is in\n"
                                              "~/.steam/steam/steamapps/common/\n"
                                              "Example for Proton 9.0:\n~/.steam/steam/steamapps/common/Proton 9.0 (Beta)");
    QString dir = QFileDialog::getExistingDirectory(this, "Select Proton's directory", QDir::currentPath());
    if (!dir.isEmpty()) {
        QFileInfo proton(dir + "/proton");
        if (proton.exists() && proton.isFile() && proton.isExecutable())
            ui->protonPath->setText(dir);
        else QMessageBox::critical(this, "Error!", "Invalid Proton folder, make sure it has a 'proton' file!");
    }
}

