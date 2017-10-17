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

#ifndef DASDINIT_H
#define DASDINIT_H

#include "UtilityExecutor.h"
#include "GenericUtility.h"

#include <QDialog>
#include <QDialog>
#include <QThread>
#include <QValidator>
#include "ui_DasdInit.h"

class UtilityExecutor;
class DasdInit : public GenericUtility
{
    Q_OBJECT

public:
    DasdInit(QWidget *parent = 0);
    virtual ~DasdInit();

private:
    Ui::dasdinitClass ui;
    bool mEnded;

    void initialize();
    void setSizeValue();
	virtual void finishedSlot();

    class VolserValidator : public QValidator
    {
    public:
    	VolserValidator(QObject * parent);
    	virtual State validate(QString & input, int & pos) const;
    };

private slots:
    void rawSlot(int);
    void sizeClicked();
    void exitClicked();
    void runClicked();
    void newDevType();
    void browseClicked();
    void filenameFocus();
    void runnerMaximumChanged(int maximum);
    void runnerValueChanged(int value);
};

#endif // DASDINIT_H
