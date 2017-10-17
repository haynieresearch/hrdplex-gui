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

#ifndef DEVICEMENUPROCESSOR_H_
#define DEVICEMENUPROCESSOR_H_

#include "DeviceConfigLine.h"
#include "DeviceTypes.h"

#include <QDialog>
#include <QMenu>

class DeviceMenuProcessor : public QWidget
{
	Q_OBJECT
public:
	DeviceMenuProcessor(QWidget *parent = 0);
	virtual ~DeviceMenuProcessor();

public slots:
    void devicesClick(QMouseEvent * event);
	void menuAddTerminal();
    void menuAddConsole();
    void menuAddPrinter();
    void menuAddCardReader();
    void menuAddCardPunch();
    void menuAddTape();
    void menuAddCTC();
    void menuAddDasd();
    void menuAddSYSG();
    void menuLoad();
	void menuUnload();
    void newTapeFileOK(QString& tapeFile);

protected:
    virtual bool isRealDev(int lineNumber) = 0;
    virtual bool canAddSYSG() = 0;
    virtual bool isConfig() = 0;
    virtual bool realDevice() = 0;
    virtual DeviceTypes::Type getType(int lineNumber) = 0;
    virtual void doLoadTape(QString& tapeFileName) = 0;
    virtual bool hasConfig() = 0;
    virtual bool traced() = 0;

    int mClickRow;
    int mLastClick;
    QMenu mMenu;
    DeviceConfigLine * mCandidateLine;

private:
    void menuAddItem(int);
};

#endif /* DEVICEMENUPROCESSOR_H_ */
