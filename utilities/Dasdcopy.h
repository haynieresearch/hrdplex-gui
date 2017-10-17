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

#ifndef DASDCOPY_H
#define DASDCOPY_H

#include "GenericUtility.h"

#include <QDialog>
#include "ui_Dasdcopy.h"

class UtilityExecutor;
class Dasdcopy : public GenericUtility
{
    Q_OBJECT

public:
    Dasdcopy(QWidget *parent = 0);
    ~Dasdcopy();

private:
    Ui::DasdcopyClass ui;
    bool mFirstEndReceived;

	virtual void finishedSlot();

private slots:
    void runClicked();
    void exitClicked();
    void browseFileClicked();
    void browseShadowClicked();
    void browseOutClicked();
    void runnerMaximumChanged(int maximum);
    void runnerValueChanged(int value);

};

#endif // DASDCOPY_H
