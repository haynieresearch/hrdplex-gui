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

#ifndef DEVICECONFIGLINE_H_
#define DEVICECONFIGLINE_H_

#include "ConfigLine.h"
#include "DeviceTypes.h"

#include <map>

class DeviceConfigLine: public ConfigLine
{
public:
    typedef std::map<std::string, DeviceTypes::Type> DeviceTypesMap;
    DeviceConfigLine(const char *line);
    virtual ~DeviceConfigLine();
	static void clear();

    int getDeviceNumber() const;
    DeviceTypes::Type getDeviceType() const;
    std::string toString() const;
    int getMultiCount();

private:
    static std::map<std::string,DeviceTypes::Type> * mDeviceTypes;
    static std::map<std::string,DeviceTypes::BaseType> * mDeviceBaseTypes;
    int mMultiCount;

    void initilize();
    void handleMultiLine();
};

#endif /* DEVICECONFIGLINE_H_ */
