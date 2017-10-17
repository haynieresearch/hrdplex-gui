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

#include "DasdTab.h"
#include "gui.h"


SizeMap DasdTab::ckdTab;
SizeMap DasdTab::fbaTab;

DasdTab DasdTab::mInstance;


DasdTab::DasdTab()
{
    init();

}

DasdTab::~DasdTab()
{

}

DasdTab& DasdTab::getInstance()
{
    return mInstance;
}

int DasdTab::getCkdSize(const std::string& dev)
{
    hOutDebug(3,"DasdTab dev=" << dev << " size=" << ckdTab.size());
    SizeMap::iterator it = ckdTab.find(dev);
    if (it == ckdTab.end())
        return 0;
    hOutDebug(3, it->second << " " << it->first);
    return it->second;
}

int DasdTab::getFbaSize(const std::string& dev)
{
    hOutDebug(3, "DasdTab dev=" << dev << " size=" << fbaTab.size());
    SizeMap::iterator it = fbaTab.find(dev);
    if (it == fbaTab.end())
        return 0;
    hOutDebug(3, it->second << " " << it->first);
    return it->second;
}

void DasdTab::init()
{
    ckdTab.insert(SizePair("2305",48));
    ckdTab.insert(SizePair("2305-1",48));
    ckdTab.insert(SizePair("2305-2",96));
    ckdTab.insert(SizePair("2311",200));
    ckdTab.insert(SizePair("2311-1",200));
    ckdTab.insert(SizePair("2314",200));
    ckdTab.insert(SizePair("2314-1",200));
    ckdTab.insert(SizePair("3330",404));
    ckdTab.insert(SizePair("3330-1",404));
    ckdTab.insert(SizePair("3330-2",808));
    ckdTab.insert(SizePair("3330-11",808));
    ckdTab.insert(SizePair("3340",348));
    ckdTab.insert(SizePair("3340-1",348));
    ckdTab.insert(SizePair("3340-35",348));
    ckdTab.insert(SizePair("3340-2",696));
    ckdTab.insert(SizePair("3340-70",696));
    ckdTab.insert(SizePair("3350",555));
    ckdTab.insert(SizePair("3350-1",555));
    ckdTab.insert(SizePair("3375",959));
    ckdTab.insert(SizePair("3375-1",959));
    ckdTab.insert(SizePair("3380",885));
    ckdTab.insert(SizePair("3380-1",885));
    ckdTab.insert(SizePair("3380-A",885));
    ckdTab.insert(SizePair("3380-B",885));
    ckdTab.insert(SizePair("3380-D",885));
    ckdTab.insert(SizePair("3380-J",885));
    ckdTab.insert(SizePair("3380-2",1770));
    ckdTab.insert(SizePair("3380-E",1770));
    ckdTab.insert(SizePair("3380-3",2655));
    ckdTab.insert(SizePair("3380-K",2655));
    ckdTab.insert(SizePair("EMC3380K+",3339));
    ckdTab.insert(SizePair("EMC3380K++",3993));
    ckdTab.insert(SizePair("3390",1113));
    ckdTab.insert(SizePair("3390-1",1113));
    ckdTab.insert(SizePair("3390-2",2226));
    ckdTab.insert(SizePair("3390-3",3339));
    ckdTab.insert(SizePair("3390-9",10017));
    ckdTab.insert(SizePair("3390-27",32760));
    ckdTab.insert(SizePair("3390-J",32760));
    ckdTab.insert(SizePair("3390-54",65520));
    ckdTab.insert(SizePair("3390-JJ",65520));
    ckdTab.insert(SizePair("9345",1440));
    ckdTab.insert(SizePair("9345-1",1440));
    ckdTab.insert(SizePair("9345-2",2156));

    fbaTab.insert(SizePair("3310",125664));
    fbaTab.insert(SizePair("3310-1",125664));
    fbaTab.insert(SizePair("3310-x",0 ));
    fbaTab.insert(SizePair("3370",558000));
    fbaTab.insert(SizePair("3370-1",558000));
    fbaTab.insert(SizePair("3370-A1",558000));
    fbaTab.insert(SizePair("3370-B1",558000));
    fbaTab.insert(SizePair("3370-2",712752));
    fbaTab.insert(SizePair("3370-A2",712752));
    fbaTab.insert(SizePair("3370-B2",712752));
    fbaTab.insert(SizePair("3370-x",0 ));
    fbaTab.insert(SizePair("9332",360036));
    fbaTab.insert(SizePair("9332-400",360036));
    fbaTab.insert(SizePair("9332-600",554800));
    fbaTab.insert(SizePair("9332-x",0 ));
    fbaTab.insert(SizePair("9335",804714));
    fbaTab.insert(SizePair("9335-x",0 ));
    fbaTab.insert(SizePair("9313",246240));
    fbaTab.insert(SizePair("9313-x",0 ));
    fbaTab.insert(SizePair("9336",920115));
    fbaTab.insert(SizePair("9336-10",920115));
    fbaTab.insert(SizePair("9336-20",1672881));
    fbaTab.insert(SizePair("9336-25",1672881));
    fbaTab.insert(SizePair("9336-x",0));
    fbaTab.insert(SizePair("0671-08",513072));
    fbaTab.insert(SizePair("0671",574560));
    fbaTab.insert(SizePair("0671-04",624456));
    fbaTab.insert(SizePair("0671-x",0 ));

}
