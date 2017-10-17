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

#include "Regs64.h"
#include "Preferences.h"

std::string Regs64::gCommand = "]GREGS64=";
std::string Regs64::cCommand = "]CREGS64=";
std::string Regs64::fCommand = "]FREGS64=";

Regs64::Regs64(QWidget * parent, std::string command) :
	Regs32(parent, command),
	mLine5(this),
	mLine6(this),
	mLine7(this),
	mLine8(this)
{
	setFont();
}

Regs64::~Regs64()
{
}

void Regs64::setActive(bool active)
{
	Regs32::setActive(active);
    mLine5.setVisible(active);
    mLine6.setVisible(active);
    mLine7.setVisible(active);
    mLine8.setVisible(active);
    std::string command = mCommand + (active ? '1' : '0');
}

void Regs64::setFont()
{
	Regs32::setFont();
	Preferences& pref = Preferences::getInstance();
	int fontSize = pref.fontSize(Preferences::RegsFontObject);
	QFont font(pref.fontName(Preferences::RegsFontObject).c_str(),
			fontSize,
			(pref.fontIsBold(Preferences::RegsFontObject) ? QFont::Bold : QFont::Normal),
			pref.fontIsItalic(Preferences::RegsFontObject));

    font.setStyleHint(QFont::Courier);
    mLine5.setFont(font);
    mLine6.setFont(font);
    mLine7.setFont(font);
    mLine8.setFont(font);
    mLine5.setGeometry(fontSize*36,fontSize+2,fontSize*35,fontSize+2);
    mLine6.setGeometry(fontSize*36,2*(fontSize+2),fontSize*35,fontSize+2);
    mLine7.setGeometry(fontSize*36,3*(fontSize+2),fontSize*35,fontSize+2);
    mLine8.setGeometry(fontSize*36,4*(fontSize+2),fontSize*35,fontSize+2);
}
bool Regs64::notify(const QByteArray& statusLine)
{
	if (statusLine.length() < 2)
	{
		setFont();
		return false;
	}
	if (statusLine[5] == '0')
		Regs32::mLine1.setText(statusLine);
	else if (statusLine[5] == '2')
		Regs32::mLine2.setText(statusLine);
	else if (statusLine[5] == '4')
		Regs32::mLine3.setText(statusLine);
	else if (statusLine[5] == '6')
		Regs32::mLine4.setText(statusLine);
	else if (statusLine[5] == '8')
		Regs64::mLine5.setText(statusLine);
	else if (statusLine[5] == 'A')
		Regs64::mLine6.setText(statusLine);
	else if (statusLine[5] == 'C')
		Regs64::mLine7.setText(statusLine);
	else if (statusLine[5] == 'E')
		Regs64::mLine8.setText(statusLine);
	else return false;

	return true;
}
