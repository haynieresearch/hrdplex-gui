/*
#**********************************************************
#* CATEGORY    APPLICATIONS
#* GROUP       HRDPLEX-GUI
#* AUTHOR      LANCE HAYNIE <LHAYNIE@HAYNIEMAIL.COM>
#* DATE        2017-10-17
#**********************************************************
#* MODIFICATIONS
#* 2017-10-17 - LHAYNIE - INITIAL VERSION
#**********************************************************

#HRDPLEX-GUI - A GUI Front End to the Hercules Mainframe Emulator
#Copyright (C) 2017  Haynie Research & Development

#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License along
#with this program; if not, write to the Free Software Foundation, Inc.,
#51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "Dasdcopy.h"
#include "gui.h"
#include "UtilityExecutor.h"
#include "UtilityRunner.h"
#include "Preferences.h"


#include <QFileDialog>
#include <QMessageBox>

#include <csignal>

Dasdcopy::Dasdcopy(QWidget *parent)
    : GenericUtility("dasdcopy",parent)
{
    ui.setupUi(this);

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.browseFileButton, SIGNAL(clicked()), this, SLOT(browseFileClicked()));
    connect(ui.browseSfButton, SIGNAL(clicked()), this, SLOT(browseShadowClicked()));
    connect(ui.browseOutButton, SIGNAL(clicked()), this, SLOT(browseOutClicked()));
}

Dasdcopy::~Dasdcopy()
{

}

void Dasdcopy::runClicked()
{
	if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }
    if (ui.infile->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdcopy", "Please specify input file to convert",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!QFile::exists(ui.infile->text()))
    {
        QMessageBox::warning(this, "dasdcopy", "Input file does not exist",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;

    }
    if (ui.filename->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdcopy", "Please specify ckd file to create",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (ui.path->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdcopy", "Please specify output directory",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!QFile::exists(ui.path->text()))
    {
        QMessageBox::warning(this, "dasdcopy", "Output directory does not exist",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QFileInfo dirInfo(ui.path->text());
    if (!dirInfo.isDir())
    {
        QMessageBox::warning(this, "dasdcopy", "Output path is not a directory",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    std::vector<std::string> parameters;

    if (ui.replaceCheckbox->isChecked())
        parameters.push_back("-r");
    if (ui.lfsCheckbox->isChecked())
        parameters.push_back("-lfs");
    switch (ui.compressionComboBox->currentIndex())
    {
        case 1:
            parameters.push_back("-z");
            break;
        case 2:
            parameters.push_back("-bz2");
            break;
        default:
            break;
    }


    parameters.push_back(ui.infile->text().toStdString());

    std::string fullPath;
    if (ui.path->text().length() > 0)
    {
        fullPath = ui.path->text().toStdString();
        fullPath += "/";
    }
    fullPath += ui.filename->text().toStdString();

    parameters.push_back(fullPath);
    std::string command = "dasdcopy";

    std::string path = Preferences::getInstance().hercDir();
    execute(command, path, parameters);
    mFirstEndReceived = false;

    connect(mRunner, SIGNAL(valueChanged(int)), this, SLOT(runnerValueChanged(int)));
    connect(mRunner, SIGNAL(maximumChanged(int)), this, SLOT(runnerMaximumChanged(int)));
    connect(mErrorRunner, SIGNAL(valueChanged(int)), this, SLOT(runnerValueChanged(int)));
    connect(mErrorRunner, SIGNAL(maximumChanged(int)), this, SLOT(runnerMaximumChanged(int)));
    ui.runButton->setText("Stop");
}

void Dasdcopy::exitClicked()
{
    deleteLater();
}

void Dasdcopy::browseFileClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for input disk",ui.infile->text());
    ui.infile->setText(s);
    if (ui.path->text().isEmpty())
    {
        QDir d("/");
        std::string path = d.absoluteFilePath(s).toStdString();
        int lastSlash = s.toStdString().find_last_of('/');
        ui.path->setText(path.substr(0,lastSlash).c_str());
    }

}

void Dasdcopy::browseShadowClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for input disk",ui.shadowFile->text());
    ui.shadowFile->setText(s);
}

void Dasdcopy::browseOutClicked()
{
    QString s = QFileDialog::getExistingDirectory(this,"Browse for output file path",ui.path->text());
    ui.path->setText(s);
}

void Dasdcopy::runnerMaximumChanged(int maximum)
{
    ui.progressBar->setMaximum(maximum);
}

void Dasdcopy::runnerValueChanged(int value)
{
    hOutDebug(3, "dasdcopy got value changed " << value);
	if (value >= 0)
        ui.progressBar->setValue(value);
    else
    {
        if (!mFirstEndReceived)
        {
            mFirstEndReceived = true;
        }
        else if (mExecutor != NULL)
        {
            mExecutor = NULL;
            QMessageBox::warning(this, "dasdcopy", "Disk copy failed",
                    QMessageBox::Ok,
                    QMessageBox::NoButton);
        }
    }
}

void Dasdcopy::finishedSlot()
{
    if (mStarted && mFinished && mFinishedOK)
    {
        ui.progressBar->setValue(ui.progressBar->maximum());
        QMessageBox::information(this, "dasdcopy", "Disk was successfully copied",
                QMessageBox::Ok,
                QMessageBox::NoButton);
    }
	deleteLater();
}
