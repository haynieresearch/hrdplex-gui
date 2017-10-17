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

#include "ConfigLine.h"
#include "gui.h"

#include <iostream>
#include <algorithm>

ConfigLine::ConfigLine(const char *line) :
	mLine(line), mRemark(false), mDeleted(false), mUpdated(false), mNew(false), mInError(false), mTokens()
{
	if (mLine.find_first_of("\n") == std::string::npos)
		mLine = mLine + "\n";
	parseLine();
}

ConfigLine::~ConfigLine()
{

}

const std::string& ConfigLine::getLine() const
{
	return mLine;
}

std::string ConfigLine::getToken(int i) const
{
	if (i >= mSize) return "";
	std::string result = mLine.substr(mTokens[i].first, mTokens[i].second ) ;
	return result;
}

std::string ConfigLine::getLowercaseToken(int i) const
{
	std::string result = getToken(i);
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

std::string ConfigLine::getUppercaseToken(int i) const
{
	std::string result = getToken(i);
	std::transform(result.begin(), result.end(), result.begin(), ::toupper);
	return result;
}

std::string ConfigLine::getMultiToken(int from, int to) const
{
	if (from >= mSize) return "";
	if (to >= mSize || to == 0) to = mSize-1;

	return mLine.substr(mTokens[from].first, mTokens[to].first+mTokens[to].second-mTokens[from].first ) ;
}

int ConfigLine::getAbsoluteTokenColumn(int i) const
{
	if (i >= mSize) return -1;
	int result = mTokens[i].first;
	return result;
}

int ConfigLine::getAbsoluteTokenEnd(int i) const
{
	if (i >= mSize) return -1;
	int result = mTokens[i].second;
	return result;
}

void ConfigLine::replaceLine(const std::string& newLine)
{
    hOutDebug(5,"configLine: new Line '" << newLine << "'" << std::endl);
    hOutDebug(5,"configLine: old Line  '" << mLine <<     "'" << std::endl);
	mLine = newLine;
	mUpdated = true;
	parseLine();
}

void ConfigLine::replaceParameters(const std::string& newParm)
{
	std::string newString = "  " + getToken(0) + "    " + newParm + "                   #edited By HRDPLEX\n";
    hOutDebug(5,"configLine: line is now '" << newString << "'" << std::endl);
    hOutDebug(5,"configLine: old string  '" << mLine <<     "'" << std::endl);
	mLine = newString;
	mUpdated = true;
	mNew = false;
	parseLine();
}

void ConfigLine::parseLine()
{
	tokenize(" \t\n\r");
	mRemark = false;
	if (mTokens.size() > 0 &&
		mLine[mTokens[0].first] == '#')
	{
		mRemark = true;
		return;
	}

	mKeyStart=mLine.find_first_not_of(" \t\n\r");
	if (mKeyStart == std::string::npos) // blank line
	{
	mRemark = true;
	return;
	}
}

void ConfigLine::tokenize(const std::string& delimiters)
{
	std::string::size_type lastPos = mLine.find_first_not_of(delimiters, 0);
	std::string::size_type pos     = mLine.find_first_of(delimiters, lastPos);
	mTokens.clear();
	mSize = 0;

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		if (pos == std::string::npos) pos = mLine.length() - lastPos;
		TokenPair newToken(lastPos, pos - lastPos);
		mTokens.push_back(newToken);
		mSize++;
		lastPos = mLine.find_first_not_of(delimiters, pos);
		pos = mLine.find_first_of(delimiters, lastPos);
		if (lastPos != std::string::npos && mLine[lastPos] == '#') break;
	}
}

void ConfigLine::setDeleted(bool parm)
{
	mDeleted = parm;
}
