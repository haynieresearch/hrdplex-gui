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

#ifndef DECOLATIONDIALOG_H
#define DECOLATIONDIALOG_H

#include "DecolationRules.h"

#include <QDialog>
#include <QAbstractTableModel>
#include <QList>

class DecolationRules;

#define DecTableType QList<QStringList>
class DecolationModelBase : public QAbstractTableModel
{
public:
    DecolationModelBase(QObject * parent = 0);
    virtual int      rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int      columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool     removeRow ( int row, const QModelIndex & parent = QModelIndex() );
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant rowData(const QModelIndex &index) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool      setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual void      clear();

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const = 0;
    virtual void     addToDecolation(DecolationRules & decolationRules) = 0;

protected:
    DecTableType zmTable;
};

class SepTableModel : public DecolationModelBase
{
public:
    SepTableModel(QObject * parent = 0);

    virtual bool     insertRow(int row, const QModelIndex &, int line, int column, const QString &value, Trigger::TriggerType type);
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual void     addToDecolation(DecolationRules & decolationRules);
};

class AccountingTableModel : public DecolationModelBase
{
public:
    AccountingTableModel(QObject * parent = 0);

    virtual bool     insertRow(int row, const QModelIndex &, int line, int column, int length, QString name);
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual void     addToDecolation(DecolationRules & decolationRules);
};


namespace Ui {
class DecolationDialog;
}

class DecolationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DecolationDialog(QWidget *parent = 0, const QString& item=QString());
    ~DecolationDialog();

private:
    void populate();
    void save(QString name);
    bool builtinDecolation(QString decName);
    void setProtected(bool protect);

    Ui::DecolationDialog *ui;

    SepTableModel * mSepModel;
    AccountingTableModel * mAccModel;
    QString mCurrentName;

private slots:
    void deleteClicked();
    void cancelClicked();
    void nameChanged(int index);
    void nameEdited(QString name);
    void addSeperator();
    void addAccounting();
    void insertSeperator();
    void insertAccounting();
    void deleteSeperator();
    void deleteAccounting();
};

#endif // DECOLATIONDIALOG_H
