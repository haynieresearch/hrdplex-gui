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

#ifndef GENERICDEVICEPROPERTIES_H_
#define GENERICDEVICEPROPERTIES_H_

#include "DeviceConfigLine.h"
#include "DeviceTypes.h"
#include "IpValidator.h"

#include <QLineEdit>
#include <QDialog>

class GenericDeviceProperties : public IpValidator
{
	Q_OBJECT
public:
	GenericDeviceProperties(ConfigLine& line, QWidget *parent);
	virtual ~GenericDeviceProperties();

	static GenericDeviceProperties * classFactory(DeviceConfigLine& line, QWidget *parent, DeviceTypes::Type type=DeviceTypes::Other);

protected:
	ConfigLine & mLine;
	QWidget * mParent;

	bool  deviceNumberValidator(QLineEdit * deviceLine);
	int   mOriginalDeviceNumber;

private slots:
	virtual void ok()=0;
	virtual void cancel()=0;

signals:
	void updateLine(bool keep);

};

#endif /* GENERICDEVICEPROPERTIES_H_ */
