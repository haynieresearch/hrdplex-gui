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

#include "Dasdcat.h"
#include "gui.h"
#include "UtilityRunner.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QMessageBox>

#include <csignal>


Dasdcat::Dasdcat(QWidget *parent)
    : GenericUtility("dasdcat",parent)
{
    ui.setupUi(this);
    ui.listRadioButton->setChecked(true);
    ui.memberName->setDisabled(true);

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.browseFileButton, SIGNAL(clicked()), this, SLOT(browseFileClicked()));
    connect(ui.browseSfButton, SIGNAL(clicked()), this, SLOT(browseSfClicked()));
    connect(ui.specificRadioButton, SIGNAL(toggled(bool)), this, SLOT(specificToggeled(bool)));
}

Dasdcat::~Dasdcat()
{

}

void Dasdcat::runClicked()
{
    if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }

    if (ui.filename->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdcat", "Please specify ckd file to process",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (ui.dsname->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdcat", "Please specify PDS name",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    std::vector<std::string> parameters;

    parameters.push_back("-i");
    parameters.push_back(ui.filename->text().toStdString());

    if (!ui.shadowFile->text().isEmpty())
    {
        std::string sf = "sf=";
        sf += ui.shadowFile->text().toStdString();
        parameters.push_back(sf);
    }
    std::string dsnameSpec = ui.dsname->text().toStdString();
    dsnameSpec += "/";
    if (ui.listRadioButton->isChecked())
    {
        dsnameSpec+= "?";
    }
    else if (ui.specificRadioButton->isChecked())
    {
        if (ui.memberName->text().isEmpty())
        {
            QMessageBox::warning(this, "dasdcat", "Member name must be provided when selecting\n'show specific member content'",
                            QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }
        dsnameSpec += ui.memberName->text().toStdString();
        dsnameSpec += ":c";  // make it readable on log
    }
    else if (ui.allRadioButton)
    {
        dsnameSpec += "*:c";
    }
    parameters.push_back(dsnameSpec);
    std::string command = "dasdcat";

    execute(command, Preferences::getInstance().hercDir(), parameters);
    ui.runButton->setText("Stop");
}


void Dasdcat::exitClicked()
{
    deleteLater();
}

void Dasdcat::browseFileClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for disk to update",ui.filename->text());
    ui.filename->setText(s);
}

void Dasdcat::browseSfClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for shadow file",ui.shadowFile->text());
    ui.shadowFile->setText(s);
}

void Dasdcat::specificToggeled(bool checked)
{
    ui.memberName->setEnabled(checked);
}

void Dasdcat::finishedSlot()
{
}
