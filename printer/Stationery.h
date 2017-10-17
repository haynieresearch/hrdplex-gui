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

#ifndef STATIONERY_H
#define STATIONERY_H

#include "gui.h"
#include <QString>

mk_shared_ptr(Stationery)

class Stationery
{
public:
    Stationery(const QString& name, const QString& paperSize, const QString& size1, const QString& size2,
               int topMargin, bool millimters, bool portrait, const QString& barsColor,
               const QString& barsRGB, bool Decorated, bool tractorFeedHoles);
    unsigned static int paperSizes();
    static const char *paperSize(unsigned int i);
    int *getBarsColor(int * fill);
    int getBarsColorIndex();

    static const char *mBarsColorNames[];

public: // TODO: getters/setters
    QString mName;
    QString mPaperSize;
    QString mSize1;
    QString mSize2;
    int     mTopMargin;
    bool    mMillimeters;
    bool    mPortrait;
    QString mBarsColor;
    QString mBarsRGB;
    bool    mDecorated;
    bool    mTractorFeedHoles;

};

#endif // STATIONERY_H
