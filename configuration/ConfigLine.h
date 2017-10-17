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

#ifndef CONFIGLINE_H
#define CONFIGLINE_H

#include <string>
#include <utility>
#include <vector>

typedef std::pair<int,int> TokenPair;

class ConfigLine
{
public:
	ConfigLine(const char *line);
	virtual ~ConfigLine();

	void  replaceLine(const std::string& newLine);
	void  replaceParameters(const std::string& newParm);
	void setDeleted(bool parm);
	const std::string& getLine() const;
	inline bool isRemark() const { return mRemark; }
	inline bool isDeleted() const { return mDeleted; }
	inline bool isNew() const { return mNew; }
	inline bool isUpdated() const { return mUpdated; }
	inline bool inError() const { return mInError; }
	inline int  errorColumn() const { return mErrorColumn; }

	inline void setNew(bool newVal) { mNew=newVal; } ;
	inline void setUpdated(bool updated) { mUpdated=updated; } ;
	inline void setInError(int column) const { mInError=true; mErrorColumn=column; }
	inline void setNoError() const { mInError=false; mErrorColumn=-1; }

	std::string getToken(int i) const;
	std::string getLowercaseToken(int i) const;
	std::string getUppercaseToken(int i) const;
	int         getAbsoluteTokenColumn(int i) const;
	int         getAbsoluteTokenEnd(int i) const;
	std::string getMultiToken(int from, int to) const;
	inline int size() const  {return mSize;}
	void  parseLine();

protected:
	std::string mLine;

private:
	bool   mRemark;
	bool   mDeleted;
	bool   mUpdated;
	bool   mNew;
	int    mSize;
	mutable bool  mInError;
	mutable int   mErrorColumn;
	std::string::size_type    mKeyStart;
	std::string               mKeyword;
	std::vector<TokenPair> mTokens;

	void  tokenize(const std::string& delimiters = " ");
};

#endif // #ifdef CONFIGLINE_H
