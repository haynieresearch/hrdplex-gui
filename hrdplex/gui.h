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

#ifndef GUI_H_
#define GUI_H_

#include <qglobal.h>
#include <QSharedPointer>
#ifndef HS_BUILD_ID
#define HS_BUILD_ID "Unknown"
#endif
#define VERSION "Version: 1.5.0"
#define REVISION "$Revision$"
#define hDEBUG 0
#define UNISTD_INC
#ifdef  Q_WS_WIN
#define hFramework
#define hCRLF
#undef UNISTD_INC
#endif
#include <iostream>

#define hOutDebugNl(level,s)  {if ((level) < hDEBUG ) \
                    {std::cout << s;} \
                    else {}}
#define hOutDebug(level,s)  hOutDebugNl(level, s << std::endl)

#define checkedConnect(button,slot) \
   if ((button) != NULL ) { connect(button, SIGNAL(clicked()), this , SLOT(slot)) ;} else{};

#define mk_shared_ptr(p) class p ; typedef QSharedPointer<p> p##Ptr;
#define mk_shared_constptr(p) class p ; typedef QSharedPointer<const p> p##ConstPtr;

class hsException : public  std::exception
{
public:
   hsException(std::string msg)
   {
	   mWhat = msg.c_str();
   }
   virtual ~hsException() throw()
   {

   }
   virtual const char* what() const throw()
   {
	   return mWhat.c_str();
   }
private:
   std::string mWhat;
};

#endif /* GUI_H_ */
