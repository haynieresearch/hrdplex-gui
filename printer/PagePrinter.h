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

#ifndef PAGEPRINTER_H
#define PAGEPRINTER_H

#include "PrinterInterface.h"
#include "gui.h"

#include <QPdfWriter>

class PrintRunner;

class PagePrinter : public GenericPagePrinter
{
    Q_OBJECT
public:
    PagePrinter(StationeryPtr &stationery, QPainter& painter);
    virtual void setBarsColor();
    virtual bool print(QByteArray);

protected:
    void setValues(bool);
    virtual void close();

private:
    const DecolationRulesPtr mRules;
    QString mStationeryName;
    QPainter* mPainter;

signals:
    void endPage();
};

class PdfPrinter : public GenericPagePrinter
{
public:
    PdfPrinter(const QString& dir, StationeryPtr& stationery, DecolationRulesPtr& decolation, int linesPerInch, int fontSize);
    virtual void setBarsColor();
    virtual bool print(QByteArray line);
    virtual void close();
    virtual bool endOfJob(QByteArray& line);

private:
    QString decodeFilename();
    bool doPrint(QByteArray& line);
    void setValues(bool eject);


    QMultiMap<int, AccountingField> mAccountingFields;
    QMap<QString, QString> mVariables;
    QPdfWriter * mPdf;
    QString mDir;
    QList<QByteArray> mFirstPage;
    int mFontSize;
    int mCurPage;
    int mCurLine;
    int mLastPages;
    int mLastPagesCountdown;
};

#endif // PAGEPRINTER_H
