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

#ifndef TRIGGER_H
#define TRIGGER_H

#include <QString>

class Trigger
{
public:
    Trigger();
    Trigger(const QString& triggerString);
    enum TriggerType
    {
        TriggerString,
        TriggerRegExp
    };
    void set(int line, int column, TriggerType trigger, QString& text);
    void set(int line, int column, int trigger, QString& text);
    static const QString &decodeTriggerType(TriggerType type);
    static TriggerType encodeTriggerType(const QString &type);

    inline int line() { return mLine; }
    inline int column() { return mColumn; }
    inline TriggerType triggerType() { return mTriggerType; }
    inline QString text() { return mText; }

    inline void setLine(int line) { mLine = line; }
    inline void setColumn(int column) { mColumn = column; }
    inline void setTriggerType(TriggerType triggerType) { mTriggerType = triggerType; }
    inline void setText(QString text) { mText = text; }

private:
    int mLine;                  // line to look for
    int mColumn;                // column to look for
    TriggerType mTriggerType;   // trigger type
    QString mText;              // validation text
};

#endif // TRIGGER_H
