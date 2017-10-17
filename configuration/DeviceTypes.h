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

#ifndef DEVICETYPES_H_
#define DEVICETYPES_H_

#include <QIcon>

class DeviceTypes
{
public:
	static DeviceTypes& instance();
    virtual ~DeviceTypes();
    enum Type { Other = 0,
            Terminal, Sysg, Console, Printer, CardReader, CardPunch,
            LinePrinter, Tape, CTC, DASD , Comm };
    enum BaseType { CTCT=100 , CTCI, LCS, QETH, CKD, FBA};
	QIcon & getIcon(DeviceTypes::Type type) ;
	const char * getName(Type type);
private:
	DeviceTypes();
	static DeviceTypes mInstance;
    const static char *sDeviceNames[];
    const static char *sDeviceIconNames[];
	static std::vector<QIcon> sDeviceIcons;
};

#endif /* DEVICETYPES_H_ */
