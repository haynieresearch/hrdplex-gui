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

#include "Dasdconv.h"
#include "gui.h"
#include "UtilityExecutor.h"
#include "UtilityRunner.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QMessageBox>

#include <csignal>


Dasdconv::Dasdconv(QWidget *parent)
    : GenericUtility("dasdconv",parent)
{
	ui.setupUi(this);

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.browseFileButton, SIGNAL(clicked()), this, SLOT(browseFileClicked()));
    connect(ui.browseOutButton, SIGNAL(clicked()), this, SLOT(browseOutClicked()));

}

Dasdconv::~Dasdconv()
{

}

void Dasdconv::runClicked()
{
	if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }
    if (ui.infile->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdconv", "Please specify input file to convert",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!QFile::exists(ui.infile->text()))
    {
        QMessageBox::warning(this, "dasdconv", "Input file does not exist",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;

    }
    if (ui.filename->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdconv", "Please specify ckd file to create",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (ui.path->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdconv", "Please specify output directory",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!QFile::exists(ui.path->text()))
    {
        QMessageBox::warning(this, "dasdconv", "Output directory does not exist",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QFileInfo dirInfo(ui.path->text());
    if (!dirInfo.isDir())
    {
        QMessageBox::warning(this, "dasdconv", "Output path is not a directory",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    std::vector<std::string> parameters;

    if (ui.replaceCheckbox->isChecked())
        parameters.push_back("-r");
    if (ui.lfsCheckbox->isChecked())
        parameters.push_back("-lfs");

    parameters.push_back(ui.infile->text().toStdString());

    std::string fullPath;
    if (ui.path->text().length() > 0)
    {
        fullPath = ui.path->text().toStdString();
        fullPath += "/";
    }
    fullPath += ui.filename->text().toStdString();

    parameters.push_back(fullPath);
    std::string command = "dasdconv";

	execute(command, Preferences::getInstance().hercDir(), parameters);

    connect(mRunner, SIGNAL(valueChanged(int)), this, SLOT(runnerValueChanged(int)));
    connect(mRunner, SIGNAL(maximumChanged(int)), this, SLOT(runnerMaximumChanged(int)));
    connect(mRunner, SIGNAL(error(QByteArray)), this, SLOT(runnerError(QByteArray)));
    connect(mErrorRunner, SIGNAL(valueChanged(int)), this, SLOT(runnerValueChanged(int)));
    connect(mErrorRunner, SIGNAL(maximumChanged(int)), this, SLOT(runnerMaximumChanged(int)));
    ui.runButton->setText("Stop");
}


void Dasdconv::exitClicked()
{
    deleteLater();
}

void Dasdconv::browseFileClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for input disk",ui.filename->text());
    ui.infile->setText(s);
    if (ui.path->text().isEmpty())
    {
        QDir d("/");
        std::string path = d.absoluteFilePath(s).toStdString();
        int lastSlash = s.toStdString().find_last_of('/');
        ui.path->setText(path.substr(0,lastSlash).c_str());
    }

}
void Dasdconv::browseOutClicked()
{
    QString s = QFileDialog::getExistingDirectory(this,"Browse for output file path",ui.path->text());
    ui.path->setText(s);
}


void Dasdconv::runnerMaximumChanged(int maximum)
{
    ui.progressBar->setMaximum(maximum);
}

void Dasdconv::runnerValueChanged(int value)
{
    hOutDebug(5, "dasdconv got value changed " << value);
    if (value > 0)
        ui.progressBar->setValue(value);
    else
    {
        ui.progressBar->setValue(ui.progressBar->maximum());
    }
}

void Dasdconv::finishedSlot()
{
	deleteLater();
}
