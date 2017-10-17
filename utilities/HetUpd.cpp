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

#include "HetUpd.h"
#include "gui.h"
#include "UtilityRunner.h"
#include "Preferences.h"

#include <QFileDialog>
#include <QMessageBox>

#include <csignal>


HetUpd::HetUpd(QWidget *parent)
    : GenericUtility("hetupd",parent)
{
    ui.setupUi(this);

    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.selInfile, SIGNAL(clicked()), this, SLOT(browseInFileClicked()));
	connect(ui.selOutdir, SIGNAL(clicked()), this, SLOT(browseOutDirClicked()));
	connect(ui.selOutfile, SIGNAL(clicked()), this, SLOT(browseOutFileClicked()));

    ui.outdir->setText(Preferences::getInstance().logsDir().c_str());
    ui.outdir->setBackgroundRole(QPalette::Window);
    ui.outdir->setToolTip("Use the button on the right to select directory");
}

HetUpd::~HetUpd()
{

}

void HetUpd::runClicked()
{
    if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }

    if (ui.outfile->text().isEmpty())
    {
        QMessageBox::warning(this, "hetupd", "Please specify tape file to create",
                QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    std::vector<std::string> parameters;


    std::string command = "hetupd";

    execute(command, Preferences::getInstance().hercDir(), parameters);
    ui.runButton->setText("Stop");
}


void HetUpd::exitClicked()
{
    deleteLater();
}

void HetUpd::browseInFileClicked()
{
	QString s = QFileDialog::getOpenFileName(this,"Browse for Input File",ui.infile->text());
	if (s.size() > 0)
		ui.infile->setText(s);
}

void HetUpd::browseOutFileClicked()
{
	QString s = QFileDialog::getOpenFileName(this,"Browse for Output File",ui.outfile->text());
	if (s.size() > 0)
	{
		QFileInfo fi(s);
		ui.outdir->setText(fi.absolutePath());
		ui.outfile->setText(fi.fileName());
	}
}

void HetUpd::browseOutDirClicked()
{
    QString s = QFileDialog::getExistingDirectory(this,"Browse For Output Directory",ui.outdir->text());
    if (s.size() > 0) ui.outdir->setText(s);
}

void HetUpd::finishedSlot()
{
    deleteLater();
}
