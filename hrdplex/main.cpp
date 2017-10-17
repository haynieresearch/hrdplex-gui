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

#include "MainWindow.h"
#include "MainPanel.h"
#include "Arguments.h"


#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
	Arguments::getInstance().parse(argc, argv);
	if (Arguments::getInstance().helpRequested())
	{
		Arguments::getInstance().printUsage();
		return 1;
	}

    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_DontShowIconsInMenus, false);

    MainWindow w;

    w.show();
    return a.exec();
}
