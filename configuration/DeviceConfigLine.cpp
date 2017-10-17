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

#include "gui.h"
#include "DeviceConfigLine.h"
#include "ConfigurationEditor.h"

#include <cstdlib>
#include <sstream>


DeviceConfigLine::DeviceTypesMap *DeviceConfigLine::mDeviceTypes = NULL;
std::map<std::string,DeviceTypes::BaseType> *DeviceConfigLine::mDeviceBaseTypes = NULL;

DeviceConfigLine::DeviceConfigLine(const char *line):
    ConfigLine(line), mMultiCount(0)
{
    initilize();
    handleMultiLine();
}

DeviceConfigLine::~DeviceConfigLine()
{
    hOutDebug(5,"~DeviceConfigLine");
}

void DeviceConfigLine::clear()
{
	if (mDeviceTypes != NULL) mDeviceTypes->clear();
	if (mDeviceBaseTypes != NULL) mDeviceBaseTypes->clear();
}

int DeviceConfigLine::getDeviceNumber() const
{
    return std::strtol(getToken(0).c_str(),NULL, 16);
}

DeviceTypes::Type DeviceConfigLine::getDeviceType() const
{
    QString token = getToken(1).c_str();
    DeviceTypesMap::iterator it = (*mDeviceTypes).find(token.toUpper().toStdString());
    if (it == (*mDeviceTypes).end())
        return (*mDeviceTypes)["0000"];
    hOutDebug(5,"getDeviceType " << token.toStdString());
    return it->second;
}

void DeviceConfigLine::handleMultiLine()
{
    std::string devno = getToken(0);
    int first = -1, last = -1;
    size_t dash = devno.find_first_of("-");
    if (dash != std::string::npos)
    {
        first = ConfigurationEditor::parseNum(devno.substr(0,dash), 16);
        last = ConfigurationEditor::parseNum(devno.substr(dash+1), 16);
    }
    size_t  dot = devno.find_first_of(".");
    if (dot != std::string::npos)
    {
        first = ConfigurationEditor::parseNum(devno.substr(0,dot), 16);
        last = first+ConfigurationEditor::parseNum(devno.substr(dot+1), 10)-1;
    }
    size_t  comma = devno.find_first_of(",");
    if (comma != std::string::npos)
    {
        first = ConfigurationEditor::parseNum(devno.substr(0,comma), 16);
        last = ConfigurationEditor::parseNum(devno.substr(comma+1), 16);
    }

    if (first > -1)
    {
        mMultiCount = last-first+1;
        hOutDebug(5, devno << " multiCount:" << mMultiCount);
        std::stringstream line;
        line << std::hex << first ;
        line << " " + getMultiToken(1,0) + "\n";
        replaceLine(line.str());
    }
    else
    {
        first = ConfigurationEditor::parseNum(devno, 16);
    }
}

int DeviceConfigLine::getMultiCount()
{
    hOutDebug(5, "get multiCount:" << mMultiCount);
    return mMultiCount;
}

void DeviceConfigLine::initilize()
{
    if (mDeviceTypes != NULL)
        return;

    mDeviceTypes = new std::map<std::string,DeviceTypes::Type>;
    mDeviceBaseTypes = new std::map<std::string,DeviceTypes::BaseType>;

    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("0000",DeviceTypes::Other));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3270",DeviceTypes::Terminal));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3287",DeviceTypes::Terminal));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("SYSG",DeviceTypes::Sysg));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("1052",DeviceTypes::Console));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3215",DeviceTypes::Console));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("1052-C",DeviceTypes::Console));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3215-C",DeviceTypes::Console));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("1442",DeviceTypes::CardReader));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("2501",DeviceTypes::CardReader));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3505",DeviceTypes::CardReader));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3525",DeviceTypes::CardPunch));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("1403",DeviceTypes::Printer));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3211",DeviceTypes::Printer));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3410",DeviceTypes::Tape));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3420",DeviceTypes::Tape));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3422",DeviceTypes::Tape));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3430",DeviceTypes::Tape));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3480",DeviceTypes::Tape));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3490",DeviceTypes::Tape));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3590",DeviceTypes::Tape));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("9347",DeviceTypes::Tape));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("8809",DeviceTypes::Tape));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3088",DeviceTypes::CTC));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("CTCI",DeviceTypes::CTC));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("LCS", DeviceTypes::CTC));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("QETH", DeviceTypes::CTC));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("CTCT", DeviceTypes::CTC));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("2305",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("2311",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("2314",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3330",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3340",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3350",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3375",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3380",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3390",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("9345",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3310",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("3370",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("9332",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("9335",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("9336",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("0671",DeviceTypes::DASD));
    mDeviceTypes->insert(std::pair<std::string,DeviceTypes::Type>("2703",DeviceTypes::Comm));

    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("3088",DeviceTypes::CTCT));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("CTCI",DeviceTypes::CTCI));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("LCS", DeviceTypes::LCS));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("QETH", DeviceTypes::QETH));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("2305",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("2311",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("2314",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("3330",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("3340",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("3350",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("3375",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("3380",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("3390",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("9345",DeviceTypes::CKD));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("3310",DeviceTypes::FBA));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("3370",DeviceTypes::FBA));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("9332",DeviceTypes::FBA));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("9335",DeviceTypes::FBA));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("9336",DeviceTypes::FBA));
    mDeviceBaseTypes->insert(std::pair<std::string,DeviceTypes::BaseType>("9336",DeviceTypes::FBA));

}

std::string DeviceConfigLine::toString() const
{
    int i=mLine.length()-3;
    while (i>=0)
    {
        if ( (mLine[i] != ' ') && (mLine[i] != '\t') && (mLine[i] != '\n') && (mLine[i] != '\r') )
            break;
        --i;
    }
    return mLine.substr(0,i);
}
