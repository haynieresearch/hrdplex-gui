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

#ifndef PRINTERPROPERTIES_H
#define PRINTERPROPERTIES_H

#include "ConfigLine.h"
#include "GenericDeviceProperties.h"
#include "gui.h"

#include "ui_PrinterProperties.h"

class PrinterProperties : public GenericDeviceProperties
{
    Q_OBJECT

public:
    PrinterProperties(ConfigLine& line,QWidget *parent = 0);
    virtual ~PrinterProperties();
    void setCrlfEnabled(bool enabled);

private:
    Ui::PrinterPropertiesClass ui;

private slots:
    void ok();
    void cancel();
    void fileNameBrowse();
};

class PrinterFileNameValidator : public QValidator
{
public:
    PrinterFileNameValidator(QObject * parent);
    virtual QValidator::State validate(QString&, int&) const;

private:
    PrinterProperties * mParent;
};

#endif // PRINTERPROPERTIES_H
