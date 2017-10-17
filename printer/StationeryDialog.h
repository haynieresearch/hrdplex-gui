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

#ifndef PAPERDIALOG_H
#define PAPERDIALOG_H

#include "Stationery.h"

#include <QDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QAbstractButton>

namespace Ui {
class PaperDialog;
}

class InchesSpinBox : public QSpinBox
{
public:
    InchesSpinBox(QWidget * parent=0);
    void setValue(int val);
    int value();

protected:
    virtual int valueFromText(const QString &text) const;
    virtual QString textFromValue(int val) const;
private:
    mutable int mNumerator; // deniminator is always 16
};

class StationeryDialog;

class PaperPreview : public QWidget
{
    Q_OBJECT
public:
    PaperPreview(StationeryDialog * dialog, QWidget *parent = 0);
    inline void setBarsRgb(int rgb)
    {
        mBarsRgb = rgb;
    }

protected:
    virtual void paintEvent(QPaintEvent*);

private:
    StationeryDialog* mDialog;
    int mBarsRgb;    // bars color
};

class StationeryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StationeryDialog(QWidget *parent = 0, const QString& item=QString());
    ~StationeryDialog();

public slots:
    void stationeryNameChanged(QString newName);
    void stationeryNameEdited(QString newName);
    void deleteClicked();
    void cancelClicked();
    void paperSizeChanged(QString value);
    void barsColorChanged(QString value);
    void inchesMm(bool checked);
    void landscape(bool checked);
    void decorationToggeled();
    void feedHolesToggeled();
    void customColorChanged();

private:
    StationeryPtr createRecord();
    void populate();
    void save(QString name);
    bool builtinStationery(QString stationeryName);
    void setProtected(bool disabled);


    Ui::PaperDialog *ui;
    InchesSpinBox* mTopMarginSpinBox;//TODO
    PaperPreview *mPaperPreview;//TODO
    int mRgb;
    QString mCurrentName;

    enum class PaperSizes  //TODO move to right place, fix sizes
    {
        Letter,	      // 8.5x11
        Legal,        // 8.5x14
        Ledger,       // 11x17
        Tabloid,      // 11x17
        LetterFanFold,// 9.5x11
        LegalFanFold, // 15x11
        USStdFanFold, // 14.875x11
        A0,
        A1,
        A2,
        A3,
        A4,
        Custom
    };

    friend class PaperPreview;
};

#endif // PAPERDIALOG_H
