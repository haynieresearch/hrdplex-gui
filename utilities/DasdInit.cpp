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

#include "DasdInit.h"
#include "UtilityExecutor.h"
#include "UtilityRunner.h"
#include "DasdTab.h"
#include "LineEditWithFocus.h"
#include "StringTokenizer.h"
#include "Preferences.h"
#include "ConfigurationEditor.h"
#include "SystemUtils.h"

#include <QMessageBox>
#include <QProcess>
#include <QFileDialog>

#include <string>
#include <sstream>
#include <cerrno>
#include <sys/types.h>
#include <csignal>

DasdInit::DasdInit(QWidget *parent)
    : GenericUtility("dasdinit", parent)
{
    ui.setupUi(this);
    ui.volser->setValidator(new VolserValidator(this));
    QLineEdit *old = ui.filename;
    LineEditWithFocus * myLineEdit = new LineEditWithFocus(ui.groupBox);
    ui.filename = myLineEdit;
    ui.filename->setObjectName(old->objectName());
    ui.filename->setGeometry(old->geometry());
    delete old;

    ui.path->setText(Preferences::getInstance().configDir().c_str());

    ui.progressBar->setVisible(false);
    ui.sizeSpinBox->setEnabled(false);
    ui.linuxCheckBox->setEnabled(false);
    setSizeValue();

    connect(ui.rawCheckBox, SIGNAL(stateChanged(int)), this, SLOT(rawSlot(int)));
    connect(ui.runButton, SIGNAL(clicked()), this, SLOT(runClicked()));
    connect(ui.exitButton, SIGNAL(clicked()), this, SLOT(exitClicked()));
    connect(ui.sizeCheckBox, SIGNAL(clicked()), this, SLOT(sizeClicked()));
    connect(ui.deviceTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(newDevType()));
    connect(ui.browseButton, SIGNAL(clicked()), this, SLOT(browseClicked()));
    connect(myLineEdit, SIGNAL(FocusGained()), this, SLOT(filenameFocus()));
}

DasdInit::~DasdInit()
{

}

void DasdInit::rawSlot(int)
{
    if (ui.rawCheckBox->isChecked())
    {
        ui.volser->setEnabled(false);
        ui.volser->clear();
    }
    else
    {
        ui.volser->setEnabled(!ui.rawCheckBox->isChecked());
    }

}

void DasdInit::sizeClicked()
{
    if (ui.sizeCheckBox->isChecked())
    {
        ui.sizeSpinBox->setEnabled(true);
        setSizeValue();
    }
    else
    {
        ui.sizeSpinBox->setEnabled(false);
        setSizeValue();
    }
}

void DasdInit::setSizeValue()
{
   int size = DasdTab::getInstance().getCkdSize(ui.deviceTypeComboBox->currentText().toStdString());
   if (size == 0)
   {
       size = DasdTab::getInstance().getFbaSize(ui.deviceTypeComboBox->currentText().toStdString());
       ui.altCheckBox->setEnabled(false);
       ui.altCheckBox->setChecked(false);
   }
   else
   {
       if (ui.sizeCheckBox->isChecked())
       {
           ui.altCheckBox->setEnabled(false);
           ui.altCheckBox->setChecked(false);
       }
       else
       {
           ui.altCheckBox->setEnabled(true);
       }

   }
   ui.sizeSpinBox->setValue( size );
   ui.progressBar->setMaximum(size);
}

void DasdInit::filenameFocus()
{
    hOutDebug(2,"dasdinit focus");
    if (ui.filename->text().length() == 0)
    {
        QString s = "dasd.";
        s += ui.deviceTypeComboBox->currentText();
        if (DasdTab::getInstance().getCkdSize(ui.deviceTypeComboBox->currentText().toStdString()) > 0)
            s += ".ckd";
        else
            s += ".fba";
        ui.filename->setText(s);
    }
}

void DasdInit::exitClicked()
{
    deleteLater();
}

void DasdInit::runClicked()
{
    if (!runOrStopClicked())
    {
        ui.runButton->setText("Run");
        return;
    }

    if (ui.filename->text().isEmpty() && !ui.rawCheckBox->isChecked())
    {
        QMessageBox::warning(this, "dasdinit",
                "missing output file name",
                QMessageBox::Ok,
                QMessageBox::NoButton);
        return;
    }

    if (ui.volser->text().isEmpty() && !ui.rawCheckBox->isChecked())
    {
        QMessageBox::warning(this, "dasdinit",
                "missing volume serial number",
                QMessageBox::Ok,
                QMessageBox::NoButton);
        return;
    }

    mEnded = false;
    std::vector<std::string> parameters;

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
    if (ui.rawCheckBox->isChecked())
        parameters.push_back("-r");
    if (ui.linuxCheckBox->isChecked())
        parameters.push_back("-linux");
    std::string fullPath;
    if (ui.path->text().length() > 0)
    {
        fullPath = ui.path->text().toStdString();
        fullPath += "/";
    }

    fullPath += ui.filename->text().toStdString();
    parameters.push_back(fullPath);
    parameters.push_back(ui.deviceTypeComboBox->currentText().toStdString());
    if (!ui.rawCheckBox->isChecked())
        parameters.push_back(ui.volser->text().toStdString());
    if (ui.sizeCheckBox->isChecked())
    	parameters.push_back(ui.sizeSpinBox->text().toStdString());
    std::string command = "dasdinit";
    std::string path = Preferences::getInstance().hercDir();

    ui.progressBar->setVisible(true);
    execute(command, path, parameters);
    connect(mRunner, SIGNAL(valueChanged(int)), this, SLOT(runnerValueChanged(int)));
    connect(mRunner, SIGNAL(maximumChanged(int)), this, SLOT(runnerMaximumChanged(int)));
    connect(mErrorRunner, SIGNAL(valueChanged(int)), this, SLOT(runnerValueChanged(int)));
    connect(mErrorRunner, SIGNAL(maximumChanged(int)), this, SLOT(runnerMaximumChanged(int)));
    ui.runButton->setText("Stop");
}

void DasdInit::newDevType()
{
    if (ui.deviceTypeComboBox->currentText().toStdString().compare(0,4,"3390") == 0)
        ui.linuxCheckBox->setEnabled(true);
    else
    {
        ui.linuxCheckBox->setEnabled(false);
        ui.linuxCheckBox->setChecked(false);
    }
    setSizeValue();
}

void DasdInit::browseClicked()
{
    QString s = QFileDialog::getExistingDirectory(this,"Browse for dasd path",ui.path->text());
    ui.path->setText(s);
}

void DasdInit::runnerMaximumChanged(int maximum)
{
    ui.progressBar->setMaximum(maximum);
}

void DasdInit::runnerValueChanged(int value)
{
    if (value > 0)
        ui.progressBar->setValue(value);
    else
        ui.progressBar->setValue(ui.progressBar->maximum());
}

void DasdInit::initialize()
{

}

DasdInit::VolserValidator::VolserValidator(QObject * parent )
: QValidator(parent)
{

}

QValidator::State DasdInit::VolserValidator::validate(QString & input, int & ) const
{
	if (input.length() > 6)
		return Invalid;
	input = input.toUpper();
	return Acceptable;
}

void DasdInit::finishedSlot()
{
	if (mExecutor == NULL)
	{
		ui.progressBar->setValue(ui.progressBar->maximum());
		if (mRc == 0)
			QMessageBox::information(this, "dasdinit", "Disk creation successfully completed!",
				QMessageBox::Ok,
				QMessageBox::NoButton);
	}
	deleteLater();
}
