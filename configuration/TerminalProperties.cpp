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

#include "TerminalProperties.h"
#include "ConfigLine.h"

#include <sstream>

TerminalProperties::TerminalProperties(ConfigLine & line, QWidget *parent)
    : GenericDeviceProperties(line, parent)
{
    ui.setupUi(this);

    if (!line.isNew())
    {
      ui.deviceNumber->setText(mLine.getToken(0).c_str());
      ui.terminalCombo->setCurrentIndex( (mLine.getToken(1) == "3287") ? 1 : 0);
      if (mLine.size() > 2)
      {
    	  int index = 2;
    	  if (mLine.getToken(index).compare("SYSGCONS") == 0)
    		  index++;
    	  if (mLine.getToken(index)[0] == '*')
    		  index++;
    	  else
    		  ui.groupName->setText(mLine.getToken(index++).c_str());
          if (mLine.size() >= index)
              ui.IPAddress->setText(mLine.getToken(index).c_str());
      }
    }

    connect(ui.OKButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
}

TerminalProperties::~TerminalProperties()
{

}

void TerminalProperties::ok()
{
    if (!deviceNumberValidator(ui.deviceNumber))
        return;
    std::stringstream newLineBuff;
    std::string ip;
    if (ui.IPAddress->text().toStdString() == "...")  // no address
        ip = "";
    else
    {
        if (!validateIp(ui.IPAddress,false, false))
    	    	return;
        ip = ui.IPAddress->text().toStdString();
    }

    std::string sysgText;
    if (isSysg()) sysgText = "SYSGCONS ";

    newLineBuff << ui.deviceNumber->text().toStdString()
		<< " "  << ui.terminalCombo->currentText().toStdString()
		<< " " << sysgText;
    if (ui.groupName->text().length() == 0)
    {
    	newLineBuff << " *";
    }
    else
    {
    	newLineBuff << " "  << ui.groupName->text().toStdString();
    }
    newLineBuff << " " << ip + "\n";
    hOutDebug(3,"terminal: new line:" << newLineBuff.str()
            << " " << &mLine);
    mLine.replaceLine(newLineBuff.str());
    emit updateLine(true);
    this->deleteLater();
}

bool TerminalProperties::isSysg()
{
	return false;
}

void TerminalProperties::cancel()
{
    emit updateLine(false);
    this->deleteLater();
}
