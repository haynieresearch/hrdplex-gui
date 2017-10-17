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

#ifndef PRINTERINTERFACE_H
#define PRINTERINTERFACE_H

#include "gui.h"
#include "Stationery.h"
#include "DecolationRules.h"

#include <QHostAddress>
#include <QPainter>

mk_shared_ptr(Stationery)
mk_shared_ptr(DecolationRules)
class BasicPrinter : public QObject
{
    Q_OBJECT
public:
    BasicPrinter();
    virtual bool print(QByteArray data) = 0;
    virtual void close() = 0;

protected:
    QString mName;
    int mLinesPerInch;
    int mFontSize;
    QString mSpoolDir;
};

class GenericPagePrinter : public BasicPrinter
{
public:
    GenericPagePrinter(StationeryPtr& stationery, float scaling);
    virtual bool print(QByteArray line) = 0;
    virtual void preparePage(bool eject);
protected:
    virtual void setValues(bool eject) = 0;
    virtual void setBarsColor() = 0;


    QMultiMap<int, Trigger> mEndJobTrigger;
    StationeryPtr mStationery;
    DecolationRulesPtr mDecolation;
    int mPixelsPerLine;
    int mPaperR;
    int mPaperG;
    int mPaperB;
    int mLinesPerPage;
    int mMargin;
    int mLeftMargin;
    int mBottomMargin;
    float mPaperHeight;
    float mPaperWidth;
    int mWidthInPixels;
    int mHeightInPixels;
    int mLogicalDpiX;
    int mLogicalDpiY;
    int mBarsColor[3];
    QPainter * mPen;
    QPixmap mLogo;
    int mLogoWidth;
    int mLogoHeight;
    float mScaling;
};

#endif // PRINTERINTERFACE_H
