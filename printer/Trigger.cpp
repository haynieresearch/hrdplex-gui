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

#include "Trigger.h"

Trigger::Trigger()
{

}

void Trigger::set(int line, int column, TriggerType trigger, QString &text)
{
    mLine = line;
    mColumn = column;
    mTriggerType = trigger;
    mText = text;
}

void Trigger::set(int line, int column, int trigger, QString &text)
{
    set(line, column, static_cast<TriggerType>(trigger), text);
}

const QString& Trigger::decodeTriggerType(TriggerType type)
{
    static const QString string("String");
    static const QString regexp("Regular Expression");
    if (type == TriggerString) return string;
    else return regexp;
}

Trigger::TriggerType Trigger::encodeTriggerType(const QString& type)
{
    static const QString string("String");
    static const QString regexp("Regular Expression");
    if (type.compare(string) == 0) return TriggerString;
    else if (type.compare(regexp) == 1) return TriggerRegExp;
    Q_ASSERT_X(false, "encodeTriggerType", "bad trigger name"); //force error
    return TriggerString;
}
