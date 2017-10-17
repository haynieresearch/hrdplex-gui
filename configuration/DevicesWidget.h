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

#ifndef DEVICESDLG_H_
#define DEVICESDLG_H_

#include "ConfigFile.h"
#include "DeviceConfigView.h"
#include "DeviceMenuProcessor.h"

#include <QDialog>
#include <QFrame>
#include <QIcon>

class DevicesWidget : public DeviceMenuProcessor
{
	Q_OBJECT
public:
	DevicesWidget(ConfigFile * configFile, QWidget * parent);
	virtual ~DevicesWidget();
	void update(ConfigFile * configFile);
	bool isUnique(std::string devno);

private:
	ConfigFile * mConfigFile;
	DeviceConfigView * mConfigView;
	QMenu mMenu;
	std::map<int,int> mDialogToFile;
	QIcon *mCardIcon;
	QIcon *mConsoleIcon;
	QIcon *mDasdIcon;
	QIcon *mNetIcon;
	QIcon *mPrinterIcon;
	QIcon *mTapeIcon;
	QIcon *mTerminalIcon;
	QIcon *mUnknownIcon;

	void initialize();
	virtual bool isRealDev(int) { return true;}
	bool canAddSYSG();
	bool isConfig() { return true; };
	bool realDevice();
	DeviceTypes::Type getType(int lineNumber);
	virtual void doLoadTape(QString&) { return; }
	virtual bool hasConfig() { return true; };
	virtual bool traced() { return false; }

private slots:
	void mousePressed(QModelIndex index);
	void doAddDevice(bool keep);
	void updateDevice(bool);

public slots:

	void menuDelete();
	void menuProperties();
};

#endif /* DEVICESDLG_H_ */
