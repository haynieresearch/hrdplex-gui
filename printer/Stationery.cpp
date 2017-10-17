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

#include "Stationery.h"
#include "gui.h"

static const char *paperSizeNames[] =
{
        "Letter",
        "Legal",
        "Ledger/Tabloid",
        "Letter FanFold",
        "Legal FanFold",
        "Std FanFold",
        "A0",
        "A1",
        "A2",
        "A3",
        "A4",
        "A5",
        "Custom"
};

const char *Stationery::mBarsColorNames[] =
{
    "Green",
    "Gray",
    "Light Blue",
    "None",
    "Custom"
};


const int barsColors[][3] =
{
    {0xdd, 0xff, 0xdd}, // Green
    {0xd2, 0xd2, 0xd2}, // Gray
    {0xe0, 0xf0, 0xf0}, // Light Blue
    {0xff, 0xff, 0xff}  // None
};

Stationery::Stationery(const QString& name, const QString& paperSize, const QString& size1, const QString& size2,
                       int topMargin, bool millimeters, bool portrait, const QString& barsColor,
                       const QString& barsRGB, bool Decorated, bool tractorFeedHoles) :
    mName(name),
    mPaperSize(paperSize),
    mSize1(size1),
    mSize2(size2),
    mTopMargin(topMargin),
    mMillimeters(millimeters),
    mPortrait(portrait),
    mBarsColor(barsColor),
    mBarsRGB(barsRGB),
    mDecorated(Decorated),
    mTractorFeedHoles(tractorFeedHoles)
{
}

unsigned int Stationery::paperSizes()
{
    return sizeof(paperSizeNames)/sizeof(char *);
}

const char *Stationery::paperSize(unsigned int i)
{
    return paperSizeNames[i];
}


int Stationery::getBarsColorIndex()
{
    for (int i=0; i<4; i++)
    {
        if (mBarsColor.compare(mBarsColorNames[i]) == 0)
            return i;
    }
    return -1;
}

int * Stationery::getBarsColor(int *fill)
{
    if (mBarsColor.compare("Custom") == 0)
    {
        QString hex = QString("0x") + mBarsRGB.right(6);
        int color = hex.toInt(0,16);
        fill[0] = color >> 16;
        fill[1] = (color >> 8) & 0xff;
        fill[2] = color & 0xff;
        return fill;
    }
    int index = getBarsColorIndex();
    memcpy(fill, barsColors[index], 3*sizeof(int));
    return fill;
}
