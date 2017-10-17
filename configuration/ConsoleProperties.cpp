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

#include "ConsoleProperties.h"
#include "gui.h"

#include <sstream>
#include <iostream>

ConsoleProperties::ConsoleProperties(ConfigLine & line, QWidget *parent)
    : GenericDeviceProperties(line, parent), mLine(line)
{
	ui.setupUi(this);
	if (!mLine.isNew())
    {
        hOutDebug(5, "not new" << mLine.getLine());
        ui.deviceNumber->setText(mLine.getToken(0).c_str());

        if (mLine.getToken(1) == "1052")
            ui.deviceCombo->setCurrentIndex(0);
        else if (mLine.getToken(1) == "3215")
            ui.deviceCombo->setCurrentIndex(1);
        else if (mLine.getLowercaseToken(1) == "1052-c")
            ui.deviceCombo->setCurrentIndex(2);
        else ui.deviceCombo->setCurrentIndex(3);

        if (ui.deviceCombo->currentIndex() >= 2)
        // integrated
        {
            setIntegrated(true);

            if (mLine.size() > 2)
            {
                ui.commandPrefix->setText(mLine.getToken(2).c_str());
            }
        }
        else // not integrated
        {
            setIntegrated(false);
            int index=2;
            if (mLine.getToken(index) == "noprompt")
            {
            	ui.noPromptCheckBox->setChecked(true);
            	index++;
            }

            if (mLine.size() > index)
            {
            	if (mLine.getToken(index).length() > 0 && mLine.getToken(index)[0] != '*')
            		ui.terminalGroup->setText(mLine.getToken(index).c_str());
                ui.ipAddress->setText(mLine.getToken(index+1).c_str());
                ui.subnetMask->setText(mLine.getToken(index+2).c_str());
            }
        }
    }
	else
	{
		setIntegrated(false);
	}
    connect(ui.OKButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui.deviceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceTypeChanged(int)) );
}

ConsoleProperties::~ConsoleProperties()
{

}

void ConsoleProperties::ok()
{
    if (!deviceNumberValidator(ui.deviceNumber))
        return;
    if ( (ui.deviceCombo->currentIndex() < 2) && !validateIp(ui.ipAddress,false, false))
    	return;
    if (!validateIp(ui.subnetMask,true, false))
    	return;
    std::stringstream newLineBuff;
    if (ui.deviceCombo->currentIndex() >= 2)
    {
    newLineBuff << ui.deviceNumber->text().toStdString() << " "
            << ui.deviceCombo->currentText().toStdString() << " "
            << ui.commandPrefix->text().toStdString() + "\n";
    }
    else
    {
        newLineBuff << ui.deviceNumber->text().toStdString()
			<< " " << ui.deviceCombo->currentText().toStdString();

        // noprompt
        if (ui.noPromptCheckBox->isChecked())
        	newLineBuff << " noprompt" ;

        // terminal group
        if (ui.terminalGroup->text().length() > 0)
			newLineBuff << " " << ui.terminalGroup->text().toStdString();
        else
        	newLineBuff << " *";

        // ip address
        newLineBuff	<< " " << ui.ipAddress->text().toStdString();

        // subnet mask
        if (ui.subnetMask->text() != "...")
            newLineBuff << " " << ui.subnetMask->text().toStdString();

        newLineBuff << "\n";

    }
    hOutDebug(5, newLineBuff.str());
    mLine.replaceLine(newLineBuff.str());
    emit updateLine(true);
    this->deleteLater();
}

void ConsoleProperties::cancel()
{
    emit updateLine(false);
    deleteLater();
}

void ConsoleProperties::setIntegrated(bool integrated)
{
    ui.commandPrefix->setVisible(integrated);
    ui.noPromptCheckBox->setEnabled(!integrated);
    ui.label_terminal->setText( integrated ? "Command Prefix" : "Terminal Group");
    ui.terminalGroup->setVisible(!integrated);
    ui.ipAddress->setEnabled(!integrated);
    ui.subnetMask->setEnabled(!integrated);

}

void ConsoleProperties::deviceTypeChanged(int index)
{
    if (index >= 2)
        setIntegrated(true);
    else
        setIntegrated(false);
}
