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

#include "gui.h"
#include "Arguments.h"

#include <string>

Arguments Arguments::instance;

Arguments::Arguments() :
	mHelp(false)
{
}

Arguments::~Arguments()
{
}

Arguments& Arguments::getInstance()
{
	return instance;
}

bool Arguments::helpRequested() const
{
	return mHelp;
}

const std::string& Arguments::configFileName() const
{
	return mConfigFile;
}

const std::string& Arguments::resourceFileName() const
{
	return mResourceFile;
}

const std::string& Arguments::titleName() const
{
	return mTitle;
}

int Arguments::parse(int argc, char * argv[])
{
	while(--argc)
    {
		argv++;
	    if (argv[0][0] != '-') return(-1);

	    switch(argv[0][1])
	    {
		case('h'):
            mHelp = true;
            break;

		case('f'):
        	--argc;
            ++argv;
            if (argc > 0) mConfigFile = argv[0];
            break;

		case('r'):
			--argc;
			++argv;
			if (argc > 0) mResourceFile = argv[0];
			break;

		case('t'):
			--argc;
			++argv;
			if (argc > 0) mTitle = argv[0];
			break;

		default:
			break;
	    }

    }
    hOutDebug(3,"resource file:'" << mResourceFile << "'");
	return 0;
}

void Arguments::printUsage()
{
    hOutDebug(3, "Usage:" << "\n\n"
            << "\thrdplex [ -f <config-file> ] [ -r <run-commands-file> ] [ -h ]");
}
