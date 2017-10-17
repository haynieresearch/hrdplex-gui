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

#include "TapeMap.h"

#include "gui.h"
#include "SystemUtils.h"
#include "Preferences.h"
#include "UtilityExecutor.h"
#include "UtilityRunner.h"
#include "StringTokenizer.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include <csignal>

TapeMap::TapeMap(QWidget *parent)
    : GenericUtility("tapemap", parent)
{
    ui.setupUi(this);

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.browseFileButton, SIGNAL(clicked()), this, SLOT(browseFileClicked()));
}

TapeMap::~TapeMap()
{

}

void TapeMap::runClicked()
{
    if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }
    if (ui.filename->text().isEmpty())
    {
        QMessageBox::warning(this, "tapemap", "Please specify tape file to process",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!QFile::exists(ui.filename->text()))
    {
        QMessageBox::warning(this, "tapemap", "The specified file does not exist",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    std::vector<std::string> parameters;

    parameters.push_back(ui.filename->text().toStdString());

    std::string command = "tapemap";

    execute(command, Preferences::getInstance().hercDir(), parameters);
    ui.runButton->setText("Stop");
}

void TapeMap::browseFileClicked()
{
    QString dir = ui.filename->text();
    if (dir.isEmpty())
        dir = Preferences::getInstance().configDir().c_str();
    QString s = QFileDialog::getOpenFileName(this,"Browse for tape to map",dir);
    ui.filename->setText(s);
}

void TapeMap::exitClicked()
{
    deleteLater();
}

void TapeMap::finishedSlot()
{
 	if (mStarted && mFinished && mFinishedOK)
	{
        QMessageBox::information(this, "tapemap", "Tapemap operation successfully completed!",
                QMessageBox::Ok,
                QMessageBox::NoButton);
        deleteLater();
	}
	else emit error();
}
