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

/* Fuck you Qt */
auto expandHome = [](QString path) -> QString {
    if (path.startsWith("~")) {
        path.replace(0, 1, QDir::homePath());
    }
    return path;
};

void MainWindow::on_pushButton_3_clicked()
{
    // file paths from UI
    QString tfpath = ui->tfPath->text().trimmed();
    QString lboxpath = ui->loaderPath->text().trimmed();
    QString protonPath = ui->protonPath->text().trimmed();

    // basic validation
    if (tfpath.isEmpty() || lboxpath.isEmpty()) {
        QMessageBox::critical(this, "Error!", "Your TF2 and/or LMAOBOX path is empty!");
        return;
    }

    if (protonPath.isEmpty()) {
        QMessageBox::critical(this, "Error!", "Your Proton path is empty!");
        return;
    }

    // convert to absolute paths
    tfpath = expandHome(tfpath);
    lboxpath = expandHome(lboxpath);
    protonPath = expandHome(protonPath);

    // select .bat file save location
    QString filter = "Bat file (*.bat)";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save .bat file"), QDir::currentPath(), filter);
    if (fileName.isEmpty())
        return;

    // normalize slashes and remove trailing backslashes
    tfpath.replace("/", "\\");
    lboxpath.replace("/", "\\");
    if (tfpath.endsWith("\\")) tfpath.chop(1);
    if (lboxpath.endsWith("\\")) lboxpath.chop(1);
    if (protonPath.endsWith("/")) protonPath.chop(1);

    // get launch options (use user input if available, otherwise placeholder)
    QString tfOptions = ui->tfOptions->text().isEmpty() ? ui->tfOptions->placeholderText() : ui->tfOptions->text();
    QString loaderOptions = ui->loaderOptions->text().isEmpty() ? ui->loaderOptions->placeholderText() : ui->loaderOptions->text();

    // write .bat file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error!", "Failed to create .bat file!");
        return;
    }

    QTextStream out(&file);
    out << "start \"\" cmd /c \"Z:" + tfpath + "\\tf_win64.exe\" " + tfOptions + "\n";
    out << "start \"\" cmd /c \"Z:" + lboxpath + "\" " + loaderOptions + "\n";
    file.close();

    // prepare Proton launch command
    QString protonCmd = "\"" + protonPath + "/proton\" run \"" + fileName + "\" ; # %command%";
    qDebug() << protonCmd;

    // copy launch command to clipboard
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(protonCmd);

    // also save launch command to a text file in the same directory
    QFileInfo info(fileName);
    QFile launchFile(info.absolutePath() + "/launch-options.txt");
    if (launchFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out2(&launchFile);
        out2 << protonCmd;
        launchFile.close();

        QMessageBox::information(this, "Success!",
                                 "Copied to clipboard the launch options for TF2!\n"
                                 "Place them in Steam -> Team Fortress 2 -> Properties -> Launch Options.\n"
                                 "The copied launch options were saved in the same folder as the .bat file!");
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
        if (dir.endsWith("/"))
            dir.chop(1);

        QFileInfo proton(dir + "/proton");
        qDebug() << dir + "/proton";
        if (proton.exists() && proton.isFile() && proton.isExecutable())
            ui->protonPath->setText(dir);
        else QMessageBox::critical(this, "Error!", "Invalid Proton folder, make sure it has a 'proton' file!");
    }
}

