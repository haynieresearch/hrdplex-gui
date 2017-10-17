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

#ifndef STRINGTOKENIZER_H
#define STRINGTOKENIZER_H

#include <string>
#include <iostream>

namespace Tokenizer
{
    typedef std::string::size_type handle;
}

class StringTokenizer
{
public:
    static inline std::string getFirstWord(const std::string& line, std::string::size_type &pos, std::string::size_type &lastPos, const std::string& delimiters)
    {
        lastPos = line.find_first_not_of(delimiters, 0);
        if (lastPos == std::string::npos) return ("");
        pos = line.find_first_of(delimiters, lastPos);
        if (pos == std::string::npos) return (line);

        return line.substr(lastPos, pos-lastPos);
    }

    static inline std::string getNextWord(const std::string& line, std::string::size_type &pos, std::string::size_type &lastPos, const std::string& delimiters)
    {
        lastPos = line.find_first_not_of(delimiters, pos);
        if (lastPos == std::string::npos) return ("");
        pos = line.find_first_of(delimiters, lastPos);
        if (pos == std::string::npos) pos=line.length();

        return line.substr(lastPos, pos-lastPos);

    }
};
#endif // STRINGTOKENIZER_H
