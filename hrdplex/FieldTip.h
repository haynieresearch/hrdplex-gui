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

#ifndef FIELDTIP_H
#define FIELDTIP_H

#include <QDialog>
#include <QTimer>

class FieldTip : public QWidget
{
    Q_OBJECT
public:
    explicit FieldTip(QWidget *parent = 0, bool top=false);
    virtual ~FieldTip();
    void showMessage(const QString& message, int timeout=10000);
    void setOwner(QWidget* widget) { attachedField = widget; }

protected:
   virtual void paintEvent(QPaintEvent*);

signals:

public slots:

private:
    bool mTop;
    QWidget *attachedField;
    QTimer expireTimer;

    void calculatePosition();
    QSize findBestSize(const QFontMetrics& fontMetrics, const QString& message);

};

#endif // FIELDTIP_H
