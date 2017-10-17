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

#ifndef STRINGEDOBJECT_H
#define STRINGEDOBJECT_H

#include <QString>

class StringedObject
{
public:
    StringedObject();

    QString toString();

protected:
    inline QString addSep(QString& sep, const char *name, QString& text, bool comma = true)
    {
        if (comma)
            return name + QString("=") + sep + text + sep + ",";
        else
            return name + QString("=") + sep + text + sep;
    }

    QString addSep(QString sep, const char *name, double number, bool comma = true)
    {
        QString s = QString::number(number);
        return addSep(sep, name, s, comma);
    }

    QString addSep(QString sep, const char *name, bool b, bool comma = true)
    {
        QString s(b ? "true" : "false");
        return addSep(sep, name, s, comma);
    }

    QString addSep(QString sep, const char *name, int number, bool comma = true)
    {
        QString s = QString::number(number);
        return addSep(sep, name, s, comma);
    }

    bool getOneField(QString& left, QString& field, const char sep='\"');
    bool getOneField(QString& left, float& field, const char sep='\"');
    bool getOneField(QString& left, int& field, const char sep='\"');
    bool getOneField(QString& left, bool& field, const char sep='\"');
};

#endif // STRINGEDOBJECT_H
