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

#include "DasdIsup.h"
#include "UtilityExecutor.h"
#include "UtilityRunner.h"
#include "StringTokenizer.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QMessageBox>

#include <csignal>

DasdIsup::DasdIsup(QWidget *parent)
    : GenericUtility("dasdisup",parent)
{
    ui.setupUi(this);
    ui.progressBar->setVisible(false);

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.browseFileButton, SIGNAL(clicked()), this, SLOT(browseFileClicked()));
    connect(ui.browseSfButton, SIGNAL(clicked()), this, SLOT(browseSfClicked()));
}

DasdIsup::~DasdIsup()
{

}

void DasdIsup::exitClicked()
{
    deleteLater();
}

void DasdIsup::runClicked()
{
	if ((mExecutor != NULL) && (mExecutor->getQProcess()->state() == QProcess::Running))
	{
		mExecutor->terminate();
		QMessageBox::warning(this, "dasdisup",
			"dasdisup operation was aborted at user's request",
			QMessageBox::Ok,
			QMessageBox::NoButton);
		mExecutor = NULL;
		ui.runButton->setText("Run");
		return;
	}
    if (ui.filename->text().isEmpty())
    {
        QMessageBox::warning(this, "dasdisup", "Please specify ckd file to process",
                QMessageBox::Ok, QMessageBox::NoButton);
    }
    std::vector<std::string> parameters;

    parameters.push_back(ui.filename->text().toStdString());

    if (!ui.shadowFile->text().isEmpty())
    {
        parameters.push_back(ui.shadowFile->text().toStdString());
    }
    parameters.push_back("EXTERNALGUI");
    std::string command = "dasdisup";

    ui.progressBar->setVisible(true);
	execute(command, Preferences::getInstance().hercDir(), parameters);

    connect(mRunner, SIGNAL(valueChanged(int)), this, SLOT(runnerValueChanged(int)));
    connect(mRunner, SIGNAL(maximumChanged(int)), this, SLOT(runnerMaximumChanged(int)));
    connect(mRunner, SIGNAL(error(QByteArray)), this, SLOT(runnerError(QByteArray)));
    ui.runButton->setText("Stop");
}

void DasdIsup::browseFileClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for disk to update",ui.filename->text());
    ui.filename->setText(s);
}

void DasdIsup::browseSfClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for shadow file",ui.shadowFile->text());
    ui.shadowFile->setText(s);
}

void DasdIsup::runnerMaximumChanged(int maximum)
{
    ui.progressBar->setMaximum(maximum);
    hOutDebug(2,"dasdisup maximum is " << maximum);
}

void DasdIsup::runnerValueChanged(int value)
{
    if (value > 0)
        ui.progressBar->setValue(value);
    else if (value < 0)
    {
        ui.progressBar->setValue(ui.progressBar->maximum());
    }
}

void DasdIsup::finishedSlot()
{
	QMessageBox::information(this, "dasdisup", "Disk update successfully completed!",
		QMessageBox::Ok,
		QMessageBox::NoButton);
	deleteLater();
}
