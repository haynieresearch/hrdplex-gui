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

#include "SystemUtils.h"
#include "gui.h"

#include <QFile>

#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef hFramework
#include <unistd.h>
#endif

SystemUtils::SystemUtils()
{
}

SystemUtils::~SystemUtils()
{
}

bool SystemUtils::processIsRunning(int pid)
{
    if (pid <= 1) return false;
    std::stringstream procPath;// TODO: merge with Watchdog (same function!)
    procPath << "/proc/" << pid;
    return (fileExists(procPath.str()));
}

bool SystemUtils::fileExists(const std::string & fileName)
{
    static struct stat dstat;
    return (stat(fileName.c_str(), &dstat) == 0);
}

bool SystemUtils::fileExists(const QString & fileName)
{
    return (QFile::exists(fileName));
}
