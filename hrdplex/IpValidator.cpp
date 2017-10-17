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

#include "IpValidator.h"
#include "gui.h"

#include <QLineEdit>
#include <QMessageBox>

IpValidator::IpValidator(QWidget * parent) :
QDialog(parent)
{
}

bool IpValidator::validateIp(QLineEdit * ipLineEdit, bool allowNull, bool quite)
{
    const std::string ip = ipLineEdit->text().toStdString();
    bool ret = true;
    if (allowNull && ip.compare("...") == 0)
        return ret;
    QString qip = ip.c_str();
    int pos=0;
    for (int i=0; i<4 && ret; i++)
    {
        hOutDebug(5, "IP:" << ip << " pos:" << pos << " i=" << i << std::endl);
        if (i>0)
        {
            if (qip.toLatin1().data()[pos] != '.')
            {
                ret = false;
                break;
            }
            pos++;
        }
        int num = 0;
        int ppos=pos;
        while(ip[pos] >= '0' && ip[pos] <= '9')
        {
            num = num*10 + ip[pos] - '0';
            pos++;
        }
        if (pos == ppos || num > 255)
        {
            ret = false;
            break;
        }
    }
    if (!ret)
    {
        if (!quite)
        {
            QString msg = ip.c_str();
            msg += " is an illegel IP address";
            QMessageBox::warning(this, "Illegal IP address", msg , QMessageBox::Ok);
            ipLineEdit->setFocus();
        }
        return false;
    }
    return true;
}
