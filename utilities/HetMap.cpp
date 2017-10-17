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

#include "HetMap.h"
#include "gui.h"
#include "UtilityRunner.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QMessageBox>

#include <csignal>

HetMap::HetMap(QWidget *parent)
    : GenericUtility("hetmap",parent)
{
    ui.setupUi(this);
	mInternalChange = false;

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
	connect(ui.allCheckBox, SIGNAL(stateChanged(int)), this, SLOT(allClicked()));
	connect(ui.fileInfoCheckBox, SIGNAL(stateChanged(int)), this, SLOT(fileInfoClicked()));
	connect(ui.labelInfoCheckBox, SIGNAL(stateChanged(int)), this, SLOT(labelInfoClicked()));
	connect(ui.dataInfoCheckBox, SIGNAL(stateChanged(int)), this, SLOT(dataInfoClicked()));
	connect(ui.tapemapCheckBox, SIGNAL(stateChanged(int)), this, SLOT(tapemapClicked()));
	connect(ui.selFile, SIGNAL(clicked()), this, SLOT(browseFileClicked()));
}

HetMap::~HetMap()
{

}

void HetMap::runClicked()
{
    if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }

    if (ui.filename->text().isEmpty())
    {
        QMessageBox::warning(this, "hetmap", "Please specify tape file to map",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    std::vector<std::string> parameters;

    if (ui.dataInfoCheckBox->isChecked())
    	parameters.push_back("-d");
    if (ui.allCheckBox->isChecked())
    	parameters.push_back("-a");
    if (ui.fileInfoCheckBox->isChecked())
    	parameters.push_back("-f");
    if (ui.labelInfoCheckBox->isChecked())
    	parameters.push_back("-l");
    if (ui.tapemapCheckBox->isChecked())
    	parameters.push_back("-t");
    parameters.push_back(ui.filename->text().toStdString());

    std::string command = "hetmap";

    execute(command, Preferences::getInstance().hercDir(), parameters);
    ui.runButton->setText("Stop");
}


void HetMap::exitClicked()
{
    deleteLater();
}

void HetMap::browseFileClicked()
{
	QString s = QFileDialog::getOpenFileName(this,"Browse for Input File",ui.filename->text());
	if (s.size() > 0)
		ui.filename->setText(s);
}

void HetMap::allClicked()
{
	if (mInternalChange) return;
	mInternalChange = true;  // do not trigger signal when changing the bellow checkboxes

	ui.fileInfoCheckBox->setChecked(false);
	ui.labelInfoCheckBox->setChecked(false);
	ui.allCheckBox->setChecked(true);

	mInternalChange = false;  // resume propagating signals
}

void HetMap::fileInfoClicked()
{
	if (mInternalChange) return;
	mInternalChange = true;  // do not trigger signal when changing the bellow checkboxes

	ui.allCheckBox->setChecked(false);
	ui.fileInfoCheckBox->setChecked(true);

	mInternalChange = false;  // resume propagating signals
}

void HetMap::labelInfoClicked()
{
	if (mInternalChange) return;
	mInternalChange = true;  // do not trigger signal when changing the bellow checkboxes

	ui.allCheckBox->setChecked(false);
	ui.labelInfoCheckBox->setChecked(true);

	mInternalChange = false;  // resume propagating signals
}

void HetMap::dataInfoClicked()
{
	if (mInternalChange) return;
	mInternalChange = true;  // do not trigger signal when changing the bellow checkboxes

	if (ui.dataInfoCheckBox->isChecked())
	{
		ui.allCheckBox->setChecked(false);
		ui.fileInfoCheckBox->setChecked(false);
		ui.labelInfoCheckBox->setChecked(false);
		ui.tapemapCheckBox->setChecked(false);
		ui.allCheckBox->setEnabled(false);
		ui.fileInfoCheckBox->setEnabled(false);
		ui.labelInfoCheckBox->setEnabled(false);
		ui.tapemapCheckBox->setEnabled(false);
	}
	else
	{
		ui.allCheckBox->setEnabled(true);
		ui.fileInfoCheckBox->setEnabled(true);
		ui.labelInfoCheckBox->setEnabled(true);
		ui.tapemapCheckBox->setEnabled(true);
		ui.allCheckBox->setChecked(true);
	}
	mInternalChange = false;  // resume propagating signals
}

void HetMap::tapemapClicked()
{
	if (mInternalChange) return;
	mInternalChange = true;  // do not trigger signal when changing the bellow checkboxes

	if (ui.tapemapCheckBox->isChecked())
	{
		ui.allCheckBox->setChecked(false);
		ui.fileInfoCheckBox->setChecked(false);
		ui.labelInfoCheckBox->setChecked(false);
		ui.dataInfoCheckBox->setChecked(false);
		ui.allCheckBox->setEnabled(false);
		ui.fileInfoCheckBox->setEnabled(false);
		ui.labelInfoCheckBox->setEnabled(false);
		ui.dataInfoCheckBox->setEnabled(false);
	}
	else
	{
		ui.allCheckBox->setEnabled(true);
		ui.fileInfoCheckBox->setEnabled(true);
		ui.labelInfoCheckBox->setEnabled(true);
		ui.dataInfoCheckBox->setEnabled(true);
		ui.allCheckBox->setChecked(true);
	}

	mInternalChange = false;  // resume propagating signals
}

void HetMap::runnerError(const QByteArray &line)
{
	QByteArray lineCopy = line;
	if (lineCopy.endsWith("\n"))
		lineCopy.chop(1);
	emit output(lineCopy);
}

void HetMap::finishedSlot()
{
   deleteLater();
}
