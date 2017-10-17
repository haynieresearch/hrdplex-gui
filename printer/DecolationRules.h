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

#ifndef DECOLATIONRULES_H
#define DECOLATIONRULES_H

#include "Trigger.h"

#include <QList>
#include <QList>

typedef QList<Trigger> Triggers;

struct Seperator
{
public:
  int mPages;         // number of consequtive pages which make a separator
  Triggers mTriggers; // sum of triggers that make a separator page
};

struct AccountingField
{
  QString mName;
  int mLine;
  int mColumn;
  int mLength;
};

class  AccountingFields
{
public:
    void add(const AccountingField& fld) { mFields.push_back(fld);}
public:
    QList<AccountingField> mFields;
};



class DecolationRules
{
public:
    DecolationRules();

    const QString& name();
    void setName(const QString &name);
    void setFileNameTemplate(const QString & name);
    QString filenameTemplate();

public:
    Seperator mSeperator;
    AccountingFields mAccounting;
    QString mFileNameTemplate;

private:
    QString mName;
};

#endif // DECOLATIONRULES_H
