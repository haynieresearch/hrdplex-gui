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

#include "Recovery.h"
#include "NamedPipe.h"

#include <sstream>

Recovery::Recovery(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	mHerculesConf = readHerculesConf();

	ui.configurationFilename->setText(mHerculesConf.c_str());
	std::stringstream ss;
	ss << NamedPipe::getInstance().getHerculesPid();
	ui.processNumber->setText(ss.str().c_str());

	connect(ui.exitButton, SIGNAL(clicked()), this , SLOT(exit()));
	connect(ui.attachButton, SIGNAL(clicked()), this , SLOT(recover()));
	connect(ui.skipButton, SIGNAL(clicked()), this , SLOT(skip()));
}

Recovery::~Recovery()
{

}

void Recovery::exit()
{
    mHerculesConf = "!";
    this->close();
}

void Recovery::skip()
{
    mHerculesConf = "";
    this->close();
}


void Recovery::recover()
{
    this->close();
}

std::string Recovery::getHerculesConf()
{
    return mHerculesConf;
}

std::string Recovery::readHerculesConf()
{
    std::string cmdLine = NamedPipe::getInstance().getHerculesCommandline();
    int ind = cmdLine.find("-d -f");
    std::string past_f = cmdLine.substr(ind+6);
    ind = past_f.find(" ");
    return past_f.substr(0,ind);
}
