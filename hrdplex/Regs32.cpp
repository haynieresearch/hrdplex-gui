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

#include "Regs32.h"
#include "Preferences.h"
#include "MainWindow.h"

#include <iostream>

std::string Regs32::gCommand = "]GREGS=";
std::string Regs32::cCommand = "]CREGS=";
std::string Regs32::fCommand = "]FREGS=";
std::string Regs32::aCommand = "]AREGS=";

Regs32::Regs32(QWidget * parent, std::string command) :
    QWidget(parent),
    mParent(parent),
	mLine1(this),
	mLine2(this),
	mLine3(this),
	mLine4(this)
{
    //QFont font("monospace", 9);
	setFont();

    mCommand = command;
}

Regs32::~Regs32()
{
}

void Regs32::setActive(bool active)
{
    mLine1.setVisible(active);
    mLine2.setVisible(active);
    mLine3.setVisible(active);
    mLine4.setVisible(active);
    std::string command = mCommand + (active ? '1' : '0');
    MainWindow * mainWindow = dynamic_cast<MainWindow *>(mParent);
    mainWindow->issueCommand(command);
}

void Regs32::setFont()
{
	Preferences& pref = Preferences::getInstance();
	int fontSize = pref.fontSize(Preferences::RegsFontObject);
	QFont font(pref.fontName(Preferences::RegsFontObject).c_str(),
			fontSize,
			(pref.fontIsBold(Preferences::RegsFontObject) ? QFont::Bold : QFont::Normal),
			pref.fontIsItalic(Preferences::RegsFontObject));

    font.setStyleHint(QFont::Courier);
    mLine1.setFont(font);
    mLine2.setFont(font);
    mLine3.setFont(font);
    mLine4.setFont(font);
    mLine1.setGeometry(2,fontSize+2,fontSize*35,fontSize+2);
    mLine2.setGeometry(2,2*(fontSize+2),fontSize*35,fontSize+2);
    mLine3.setGeometry(2,3*(fontSize+2),fontSize*35,fontSize+2);
    mLine4.setGeometry(2,4*(fontSize+2),fontSize*35,fontSize+2);

}

bool Regs32::notify(const QByteArray& statusLine)
{
	if (statusLine.length() < 2)
	{
		setFont();
		return false;
	}
	if (statusLine[2] == '0')
		Regs32::mLine1.setText(statusLine);
	else if (statusLine[2] == '4')
		Regs32::mLine2.setText(statusLine);
	else if (statusLine[2] == '8')
		Regs32::mLine3.setText(statusLine);
	else if (statusLine[2] == 'C')
		Regs32::mLine4.setText(statusLine);
	else return false;

	return true;
}
