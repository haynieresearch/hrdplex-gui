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

#include "HetGet.h"
#include "gui.h"
#include "UtilityRunner.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QMessageBox>

#include <csignal>


HetGet::HetGet(QWidget *parent)
    : GenericUtility("hetget",parent)
{
    ui.setupUi(this);

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.browseInFile, SIGNAL(clicked()), this, SLOT(browseInFileClicked()));
    connect(ui.browseOutDir, SIGNAL(clicked()), this, SLOT(browseOutDirClicked()));
    connect(ui.browseOutFile, SIGNAL(clicked()), this, SLOT(browseOutFileClicked()));
	connect(ui.asciiCheckBox, SIGNAL(stateChanged(int)), this, SLOT(asciiChanged()));
	connect(ui.stripCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stripChanged()));
    connect(ui.unblockCheckBox, SIGNAL(stateChanged(int)), this, SLOT(unblockChanged()));

    ui.outdir->setText(Preferences::getInstance().logsDir().c_str());
    ui.outdir->setBackgroundRole(QPalette::Window);
    ui.outdir->setToolTip("Use the button on the right to select directory");
}

HetGet::~HetGet()
{

}

void HetGet::runClicked()
{
    if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }

	if (ui.infile->text().isEmpty())
	{
		QMessageBox::warning(this, "hetget", "Please specify input tape file to process",
			QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	else
	{
		if (!QFile::exists(ui.infile->text()))
		{
			QMessageBox::warning(this, "hetget", "Input tape file does not exist",
				QMessageBox::Ok, QMessageBox::NoButton);
			ui.infile->setFocus(Qt::OtherFocusReason);
			return;

		}
	}
	if (ui.outfile->text().isEmpty())
	{
		QMessageBox::warning(this, "hetget", "Please specify output tape file",
			QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
    std::vector<std::string> parameters;
	if (ui.nlCheckBox->isChecked()) parameters.push_back("-n");
	if (ui.asciiCheckBox->isChecked()) parameters.push_back("-a");
	if (ui.unblockCheckBox->isChecked()) parameters.push_back("-u");
	if (ui.stripCheckBox->isChecked()) parameters.push_back("-s");
	parameters.push_back(ui.infile->text().toStdString());
	QString outfile = ui.outdir->text();
	if (outfile.size() > 0)
		outfile += "/";
	outfile += ui.outfile->text();
	parameters.push_back(outfile.toStdString());
	parameters.push_back(QString::number(ui.inputBlocksSpinBox->value()).toStdString());

    std::string command = "hetget";

    execute(command, Preferences::getInstance().hercDir(), parameters);
    ui.runButton->setText("Stop");
}


void HetGet::exitClicked()
{
    deleteLater();
}

void HetGet::browseInFileClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for tape to process",ui.infile->text());
    if (s.size() > 0)
    {
        ui.infile->setText(s);
    }
}

void HetGet::browseOutDirClicked()
{
    QString s = QFileDialog::getExistingDirectory(this,"Browse for target output directory",ui.outdir->text());
    if (s.size() > 0) ui.outdir->setText(s);
}

void HetGet::browseOutFileClicked()
{
    QString s = QFileDialog::getOpenFileName(this,"Browse for target output file",ui.outdir->text());
    if (s.size() > 0)
    {
        QFileInfo fi(s);
        ui.outdir->setText(fi.absolutePath());
        ui.outfile->setText(fi.fileName());
    }
}

void HetGet::asciiChanged()
{
	if (ui.asciiCheckBox->isChecked())
		ui.unblockCheckBox->setChecked(true);
}

void HetGet::stripChanged()
{
	if (ui.stripCheckBox->isChecked())
		ui.asciiCheckBox->setChecked(true);
}

void HetGet::unblockChanged()
{
    if (!ui.unblockCheckBox->isChecked())
        ui.asciiCheckBox->setChecked(false);
}

void HetGet::finishedSlot()
{
    deleteLater();
}
