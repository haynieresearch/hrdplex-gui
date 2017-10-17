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

#include "DecolationDialog.h"
#include "ui_DecolationDialog.h"
#include "DecolationRules.h"
#include "Trigger.h"
#include "Preferences.h"
#include "FieldTip.h"
#include "gui.h"

#include <QMessageBox>

DecolationDialog::DecolationDialog(QWidget *parent, const QString & item) :
    QDialog(parent),
    ui(new Ui::DecolationDialog),
    mSepModel(NULL),
    mAccModel(NULL)
{
    ui->setupUi(this);

    ui->decolationNameComboBox->insertItems(0, Preferences::getInstance().decolationList());
    mCurrentName = ui->decolationNameComboBox->currentText();

    connect(ui->decolationNameComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(nameChanged(int)));
    connect(ui->decolationNameComboBox, SIGNAL(editTextChanged(QString)), this, SLOT(nameEdited(QString)));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(ui->addSeperatorButton, SIGNAL(clicked()), this, SLOT(insertSeperator()));
    connect(ui->deleteSeperatorButton, SIGNAL(clicked()), this, SLOT(deleteSeperator()));
    connect(ui->addAccountingButton, SIGNAL(clicked()), this, SLOT(insertAccounting()));
    connect(ui->deleteAccountingButton, SIGNAL(clicked()), this, SLOT(deleteAccounting()));
    addSeperator();
    addAccounting();

    if (item.length() > 0)
    {
        ui->decolationNameComboBox->setCurrentText(item);
    }
    else
    {
        ui->decolationNameComboBox->setCurrentIndex(0);
    }
    populate();
    if (!Preferences::getInstance().balloonDecolation()  && builtinDecolation(ui->decolationNameComboBox->currentText()))
    {
        Preferences::getInstance().setBalloonDecolation(true);
        FieldTip *baloon = new FieldTip(this, true);
        baloon->setOwner(ui->decolationNameComboBox);
        baloon->showMessage("Change this name to define you own decolation", 7000);
    }
}

DecolationDialog::~DecolationDialog()
{
    delete ui;
}

void DecolationDialog::cancelClicked()
{
    if (!builtinDecolation(ui->decolationNameComboBox->currentText()))
            save(ui->decolationNameComboBox->currentText());
    deleteLater();
}

bool DecolationDialog::builtinDecolation(QString decName)
{
    if (decName == "zOS")
        return true;
    else
        return false;
}

void DecolationDialog::deleteClicked()
{
    QString decToDelete = ui->decolationNameComboBox->currentText();
    Preferences::getInstance().deleteDecolation(ui->decolationNameComboBox->currentText());
    mCurrentName = "";
    ui->decolationNameComboBox->clear();
    ui->decolationNameComboBox->insertItems(0,Preferences::getInstance().decolationList());
    ui->decolationNameComboBox->setCurrentIndex(0);

}

void DecolationDialog::nameChanged(int)
{
    save(mCurrentName);
    mCurrentName = ui->decolationNameComboBox->currentText();
    addSeperator();
    addAccounting();
    populate();
}

void DecolationDialog::nameEdited(QString name)
{
    setProtected(builtinDecolation(name));
}

void DecolationDialog::populate()
{
    QString name = ui->decolationNameComboBox->currentText();
    DecolationRules * dec = Preferences::getInstance().decolation(name);
    ui->decolationNameComboBox->setCurrentText(dec->name());
    ui->endJobPagesSpinBox->setValue(dec->mSeperator.mPages);
    ui->fileNameTemplate->setText(dec->filenameTemplate());
    mSepModel->clear();
    for (int i=0; i<dec->mSeperator.mTriggers.count(); i++)
    {
        mSepModel->insertRow(mSepModel->rowCount()-1, QModelIndex(),
                             dec->mSeperator.mTriggers[i].line(),
                             dec->mSeperator.mTriggers[i].column(),
                             dec->mSeperator.mTriggers[i].text(),
                             dec->mSeperator.mTriggers[i].triggerType());
    }

    mAccModel->clear();
    for (int i=0; i<dec->mAccounting.mFields.count(); i++)
    {
        mAccModel->insertRow(mAccModel->rowCount()-1, QModelIndex(),
                             dec->mAccounting.mFields[i].mLine,
                             dec->mAccounting.mFields[i].mColumn,
                             dec->mAccounting.mFields[i].mLength,
                             dec->mAccounting.mFields[i].mName);
    }
    setProtected(builtinDecolation(name));
}

void DecolationDialog::setProtected(bool protect)
{
    ui->endJobPagesSpinBox->setEnabled(!protect);
    ui->fileNameTemplate->setEnabled(!protect);
    ui->accountingTableView->setEnabled(!protect);
    ui->seperatorTableView->setEnabled(!protect);
    ui->addAccountingButton->setEnabled(!protect);
    ui->deleteAccountingButton->setEnabled(!protect);
    ui->addSeperatorButton->setEnabled(!protect);
    ui->deleteSeperatorButton->setEnabled(!protect);
    ui->deleteButton->setEnabled(!protect);
}

void DecolationDialog::save(QString name)
{
    if (name.length() == 0) return ;
    DecolationRules dec;

    mSepModel->addToDecolation(dec);
    mAccModel->addToDecolation(dec);
    dec.mSeperator.mPages = ui->endJobPagesSpinBox->value();
    dec.mFileNameTemplate = ui->fileNameTemplate->text();
    dec.setName(name);
    Preferences::getInstance().setDecolation(dec);
}


void DecolationDialog::addSeperator()
{
    QItemSelectionModel *oldModel = ui->seperatorTableView->selectionModel();
    mSepModel = new SepTableModel(this);
    ui->seperatorTableView->setModel(mSepModel);
    delete oldModel;
    ui->seperatorTableView->resizeColumnToContents(0);
}

void DecolationDialog::insertSeperator()
{
    mSepModel->insertRow(mSepModel->rowCount()-1, QModelIndex(),
                         1, 1,"(value)", Trigger::TriggerString);
}

void DecolationDialog::addAccounting()
{
    QItemSelectionModel *oldModel = ui->accountingTableView->selectionModel();
    mAccModel = new AccountingTableModel(this);
    ui->accountingTableView->setModel(mAccModel);
    delete oldModel;
    ui->accountingTableView->resizeColumnToContents(0);
}

void DecolationDialog::insertAccounting()
{
     mAccModel->insertRow(mAccModel->rowCount()-1, QModelIndex(),
                           1, 1, 8, "Job-name");
}

void DecolationDialog::deleteSeperator()
{
    int i = ui->seperatorTableView->currentIndex().row();
    mSepModel->removeRow(i, QModelIndex());
}

void DecolationDialog::deleteAccounting()
{
    int i = ui->accountingTableView->currentIndex().row();
    mAccModel->removeRow(i, QModelIndex());
}

//////////////////////////////////////

DecolationModelBase::DecolationModelBase(QObject * o) :
    QAbstractTableModel(o)
{

}

int DecolationModelBase::rowCount(const QModelIndex &parent) const
{
    hOutDebug(3,"row count:" << zmTable.size());
    if (parent.isValid()) return 0;
    else return zmTable.size();
}


int DecolationModelBase::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else return 4;
}

bool DecolationModelBase::removeRow ( int row, const QModelIndex & parent)
{
    if (parent.isValid()) return false;
    if (row >= zmTable.size()) return false;
    DecTableType::iterator it = zmTable.begin()+row;

    if (row < 0) return false;
    beginRemoveRows(parent,row, row);
    zmTable.erase(it);
    endRemoveRows();
    return true;
}

QVariant DecolationModelBase::data(const QModelIndex &index, int role) const
{
    if (index.row() >= zmTable.size()) return QVariant(QVariant::Invalid);
    DecTableType::const_iterator it = zmTable.begin() + index.row();
    QStringList list = *it;
    if (role != Qt::DisplayRole) return QVariant(QVariant::Invalid);
    Q_ASSERT(list.size() > 3);
    return QVariant(list[index.column()]);
}

QVariant DecolationModelBase::rowData(const QModelIndex &index) const
{
    if (index.row() >= zmTable.size()) return QVariant(QVariant::Invalid);
    DecTableType::const_iterator it = zmTable.begin() + index.row();
    QStringList list = *it;
    return QVariant(list);
}

Qt::ItemFlags DecolationModelBase::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = QAbstractTableModel::flags(index);
    return ret | Qt::ItemIsEditable;
}

bool DecolationModelBase::setData(const QModelIndex &index, const QVariant &value, int)
{
    hOutDebug(4,"set data " << index.row())
    if (zmTable.size() <= index.row()) return false;
    if (!value.isValid() || !value.canConvert(QVariant::StringList)) return false;
    if (value.toString().length() == 0) return false;
    DecTableType::const_iterator it = zmTable.begin() + index.row();

    QStringList list = *it;
    list[index.column()] = value.toString();
    zmTable[index.row()] = list;
    return true;
}

void DecolationModelBase::clear()
{
    zmTable.clear();
}

////////////////////////////////////////
SepTableModel::SepTableModel(QObject *o) : DecolationModelBase(o)
{

}

bool SepTableModel::insertRow(int row, const QModelIndex & index,
                              int line, int column, const QString& value, Trigger::TriggerType type)
{
    hOutDebug(3,"insert " << line << " , "<< column << " " << value.toStdString() <<Trigger::decodeTriggerType(type).toStdString());

    if (row > zmTable.size()) return false;
    QStringList list;
    list << QString::number(line) << QString::number(column) << value << Trigger::decodeTriggerType(type);

    int key = zmTable.size();
    beginInsertRows(index, key+1, key+1);
    zmTable.append(list);
    endInsertRows();
    hOutDebug(3, "new row count " << zmTable.count());

    return true;
}


QVariant SepTableModel::headerData ( int col, Qt::Orientation orientation, int role ) const
{
    static const char * names[] =
    {
        "line",
        "Column",
        "Value",
        "Type"
    };
    hOutDebug(3, "header " << col);
    if (orientation == Qt::Horizontal && role == 0)
    {
        if (col < 4) return QVariant(QString(names[col]));
        else return QVariant(QString("?"));
    }
    else
    {
        return QVariant();
    }
}

void SepTableModel::addToDecolation(DecolationRules &decolationRules)
{
    Seperator& sep = decolationRules.mSeperator;
    sep.mPages=1;
    DecTableType:: iterator it = zmTable.begin();
    for (; it != zmTable.end(); it++)
    {
        Trigger trigger;
        QStringList& l = *it;
        trigger.set(l[0].toInt(), l[1].toInt(), l[3].toInt(), l[2]);
        sep.mTriggers.push_back(trigger);
    }
}


//////////////////////////////////////

AccountingTableModel::AccountingTableModel(QObject *o) : DecolationModelBase(o)
{

}

bool AccountingTableModel::insertRow(int row, const QModelIndex & index,
                              int line, int column, int length, QString name)
{
    hOutDebug(4,"insert " << line << " , "<< column << " " << length <<" " << name.toStdString());

    if (row > zmTable.size()) return false;
    QStringList list;
    list << QString::number(line) << QString::number(column) << QString::number(length) << name;

    int key = zmTable.size();
    beginInsertRows(index, key+1, key+1);
    zmTable.append(list);
    endInsertRows();

    return true;
}

QVariant AccountingTableModel::headerData ( int col, Qt::Orientation orientation, int role ) const
{
    static const char * names[] =
    {
        "line",
        "Column",
        "Length",
        "Field Name"
    };
    hOutDebug(3, "header " << col);
    if (orientation == Qt::Horizontal && role == 0)
    {
        if (col < 4) return QVariant(QString(names[col]));
        else return QVariant(QString("?"));
    }
    else
    {
        return QVariant();
    }
}

void AccountingTableModel::addToDecolation(DecolationRules &decolationRules)
{
    AccountingFields& acc = decolationRules.mAccounting;
    for (int i=0; i<zmTable.size(); i++)
    {
        AccountingField oneField;
        oneField.mName = zmTable[i][3];
        oneField.mLine = zmTable[i][0].toInt();
        oneField.mColumn = zmTable[i][1].toInt();
        oneField.mLength = zmTable[i][2].toInt();
        acc.add(oneField);
    }
}
