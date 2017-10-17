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

#include "TapeCopy.h"
#include "Preferences.h"
#include "UtilityExecutor.h"
#include "UtilityRunner.h"
#include "StringTokenizer.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTimer>

#include <csignal>


TapeCopy::TapeCopy(QWidget *parent)
    : GenericUtility("tapecopy",parent)
{
    ui.setupUi(this);
    mUpperGeometry = ui.FileBox->geometry();
    mLowerGeometry = ui.ScsiBox->geometry();
    connect(ui.scsiToFileRadio, SIGNAL(clicked()), this, SLOT(scsiToTape()));
    connect(ui.fileToScsiRadio, SIGNAL(clicked()), this, SLOT(tapeToScsi()));
    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(ui.browseFileButton, SIGNAL(clicked()), this, SLOT(browseFileClicked()));
}

TapeCopy::~TapeCopy()
{

}

void TapeCopy::runClicked()
{
    if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }
    if (ui.filename->text().isEmpty())
    {
        QMessageBox::warning(this, "tapecopy", "Please specify tape file to process",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!QFile::exists(ui.filename->text()))
    {
        QMessageBox::warning(this, "tapecopy", "The specified file does not exist",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    std::vector<std::string> parameters;
    parameters.push_back(ui.filename->text().toStdString());
    std::string scsi="/dev/st";
    scsi+=ui.spinBox->text().toStdString();
    parameters.push_back(scsi.c_str());
    std::string command = "tapecopy";

    execute(command, Preferences::getInstance().hercDir(), parameters);

    ui.runButton->setText("Stop");
}

void TapeCopy::browseFileClicked()
{
    QString dir = ui.filename->text();
    if (dir.isEmpty())
        dir = Preferences::getInstance().configDir().c_str();
    QString s = QFileDialog::getOpenFileName(this,"Browse for tape to map",dir);
    ui.filename->setText(s);
}

void TapeCopy::cancelClicked()
{
    deleteLater();
}

void TapeCopy::tapeToScsi()
{
    ui.FileBox->setGeometry(mUpperGeometry);
    ui.ScsiBox->setGeometry(mLowerGeometry);
    ui.FileBox->setTitle("Input Tape");
    ui.ScsiBox->setTitle("Output Tape");
}

void TapeCopy::scsiToTape()
{
    ui.FileBox->setGeometry(mLowerGeometry);
    ui.ScsiBox->setGeometry(mUpperGeometry);
    ui.FileBox->setTitle("Output Tape");
    ui.ScsiBox->setTitle("Input Tape");
}

void TapeCopy::finishedSlot()
{

}
