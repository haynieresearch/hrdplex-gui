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

#include "DasdLoad.h"
#include "gui.h"
#include "UtilityRunner.h"
#include "Preferences.h"
#include "StringTokenizer.h"


#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

#include <csignal>

DasdLoad::DasdLoad(QWidget *parent)
    : GenericUtility("dasdload", parent)
{
	ui.setupUi(this);

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.browseCtlButton, SIGNAL(clicked()), this, SLOT(browseCtlClicked()));
    connect(ui.browseOutButton, SIGNAL(clicked()), this, SLOT(browseOutClicked()));
}

DasdLoad::~DasdLoad()
{

}

void DasdLoad::exitClicked()
{
    deleteLater();
}

void DasdLoad::runClicked()
{
    if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }

    std::vector<std::string> parameters;

    if (ui.filename->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdload", "Please specify dasd file to create",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (ui.path->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdload", "Please specify path for dasd file to create",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    switch (ui.compressionComboBox->currentIndex())
    {
        case 1:
            parameters.push_back("-z");
            break;
        case 2:
            parameters.push_back("-bz2");
            break;
        case 3:
            parameters.push_back("-0");
            break;
        default:
            break;
    }
    if (ui.lfsCheckBox->isChecked())
        parameters.push_back("-lfs");
    if (ui.altCheckBox->isChecked())
        parameters.push_back("-a");
    parameters.push_back(ui.control->text().toStdString());
    std::string fullPath;
    if (ui.path->text().length() > 0)
    {
        fullPath = ui.path->text().toStdString();
        fullPath += "/";
    }
    fullPath += ui.filename->text().toStdString();
    parameters.push_back(fullPath);
    std::string command = "dasdload";
    std::string path = "";
    QFileInfo fi(ui.control->text());
    hOutDebug(2, "dir: " << fi.absolutePath().toStdString());

    execute(command, Preferences::getInstance().hercDir(), parameters, fi.absolutePath());

    ui.progressBar->setVisible(true);
    connect(mErrorRunner, SIGNAL(valueChanged(int)), this, SLOT(runnerValueChanged(int)));
    connect(mErrorRunner, SIGNAL(maximumChanged(int)), this, SLOT(runnerMaximumChanged(int)));
    ui.runButton->setText("Stop");
}

void DasdLoad::browseCtlClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for input file path",ui.path->text());
    ui.control->setText(s);
}

void DasdLoad::browseOutClicked()
{
    QString s = QFileDialog::getExistingDirectory(this,"Browse for output file path",ui.path->text());
    ui.path->setText(s);
}

void DasdLoad::runnerMaximumChanged(int maximum)
{
    ui.progressBar->setMaximum(maximum);
}

void DasdLoad::runnerValueChanged(int value)
{
    if (value > 0)
        ui.progressBar->setValue(value);
    else
        ui.progressBar->setValue(ui.progressBar->maximum());
}

void DasdLoad::finishedSlot()
{
    if (mStarted && mFinished && mFinishedOK)
    {
        QMessageBox::information(this, "dasdload", "Dasdload operation successfully completed!",
                QMessageBox::Ok,
                QMessageBox::NoButton);
        deleteLater();
    }
    else emit error();
}
