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

#ifndef DEVICEENTRY_H_
#define DEVICEENTRY_H_

#include "DeviceTypes.h"

#include <QPixmap>
#include <QIcon>
#include <QStandardItem>
#include <string>

class VisualizedDeviceEntry
{
public:
    VisualizedDeviceEntry(int number, DeviceTypes::Type type, const std::string& definition);
	VisualizedDeviceEntry();
    virtual ~VisualizedDeviceEntry();

    inline int getDeviceNumber(){ return mNumber; }
    inline int getLineNumber(){ return mLine; }
    inline DeviceTypes::Type getType() { return mType; }
    inline QPixmap * getPixmap() { return mPixmap; }
    inline QIcon * getIcon() { return mIcon; }
    inline QStandardItem * getItem() { return mItem; }
    inline std::string getDefinition() { return mDefinition; }
    inline bool traced() { return mTraced; }
    inline void setLineNumber(int lineNumber) { mLine = lineNumber; }
    inline void setPixmap(QPixmap * w) { mPixmap=w; }
    inline void setIcon(QIcon * w) { mIcon=w; }
    inline void setItem(QStandardItem * w) { mItem=w; }
    inline void setTraced(bool traced) { mTraced = traced; }

private:
    int mNumber;
    int mLine;
    DeviceTypes::Type mType;
    std::string mDefinition;
    QPixmap * mPixmap;
    QIcon * mIcon;
    QStandardItem * mItem;
    bool mTraced;
};
#endif /* DEVICEENTRY_H_ */
