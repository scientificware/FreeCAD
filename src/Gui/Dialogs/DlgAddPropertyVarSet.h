/****************************************************************************
 *   Copyright (c) 2024 Ondsel <development@ondsel.com>                     *
 *   Copyright (c) 2025 Pieter Hijma <info@pieterhijma.net>                 *
 *                                                                          *
 *   This file is part of the FreeCAD CAx development system.               *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Library General Public            *
 *   License as published by the Free Software Foundation; either           *
 *   version 2 of the License, or (at your option) any later version.       *
 *                                                                          *
 *   This library  is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Library General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU Library General Public      *
 *   License along with this library; see the file COPYING.LIB. If not,     *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,          *
 *   Suite 330, Boston, MA  02111-1307, USA                                 *
 *                                                                          *
 ****************************************************************************/

#ifndef GUI_DIALOG_DLG_ADD_PROPERTY_VARSET_H
#define GUI_DIALOG_DLG_ADD_PROPERTY_VARSET_H

#include <qcompleter.h>

#include <QDialog>
#include <QComboBox>
#include <QFormLayout>

#include <FCGlobal.h>

#include <App/VarSet.h>

#include "propertyeditor/PropertyItem.h"

namespace Gui {

class ViewProviderVarSet;

namespace Dialog {

class EditFinishedComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit EditFinishedComboBox(QWidget *parent = nullptr) : QComboBox(parent) {
        setEditable(true);
        connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EditFinishedComboBox::onIndexChanged);
        connect(this->lineEdit(), &QLineEdit::editingFinished, this, &EditFinishedComboBox::onEditingFinished);
    }

Q_SIGNALS:
    void editFinished();

private:
    void onEditingFinished() {
        Q_EMIT editFinished();
    }

    void onIndexChanged() {
        Q_EMIT editFinished();
    }
};

class Ui_DlgAddPropertyVarSet;

class GuiExport DlgAddPropertyVarSet : public QDialog
{
    Q_OBJECT

public:
    static const std::string GroupBase;

public:
    DlgAddPropertyVarSet(QWidget* parent, ViewProviderVarSet* viewProvider);

    DlgAddPropertyVarSet(const DlgAddPropertyVarSet&) = delete;
    DlgAddPropertyVarSet(DlgAddPropertyVarSet&&) = delete;
    DlgAddPropertyVarSet& operator=(const DlgAddPropertyVarSet&) = delete;
    DlgAddPropertyVarSet& operator=(DlgAddPropertyVarSet&&) = delete;

    ~DlgAddPropertyVarSet() override;

    void changeEvent(QEvent* e) override;
    void accept() override;
    void reject() override;

public Q_SLOTS:
    void valueChanged();

private:
    enum class TransactionOption : bool {
        Commit = false,
        Abort = true
    };

    enum class FieldChange : std::uint8_t {
        Name,
        Type
    };

    int findLabelRow(const char* labelName, QFormLayout* layout);
    void removeExistingWidget(QFormLayout* layout, int labelRow);
    void setWidgetForLabel(const char* labelName, QWidget* widget);
    void initializeGroup();

    std::vector<Base::Type> getSupportedTypes();
    void initializeTypes();

    void removeSelectionEditor();
    void addEditor(PropertyEditor::PropertyItem* propertyItem);
    bool isTypeWithEditor(const Base::Type& type);
    bool isTypeWithEditor(const std::string& type);
    void createEditorForType(const Base::Type& type);
    void initializeValue();

    void setTitle();
    void setOkEnabled(bool enabled);
    void initializeWidgets(ViewProviderVarSet* viewProvider);

    bool propertyExists(const std::string& name);
    bool isNameValid();
    bool isGroupValid();
    bool isTypeValid();
    bool areFieldsValid();

    void setEditor(bool valueNeedsReset);
    void buildForUnbound(bool valueNeedsReset);
    void setPropertyItem(App::Property* prop);
    void buildForBound(bool valueNeedsReset);
    bool clearBoundProperty();
    bool clear(FieldChange fieldChange);
    void onNameChanged(const QString& text);
    void onGroupFinished();
    void onTypeChanged(const QString& text);

    void showStatusMessage();

    void removeEditor();

    void openTransaction();
    void critical(const QString& title, const QString& text);
    App::Property* createProperty();
    void closeTransaction(TransactionOption option);
    void clearFields();
    void addDocumentation();

private:
    App::VarSet* varSet;
    std::unique_ptr<Ui_DlgAddPropertyVarSet> ui;

    EditFinishedComboBox comboBoxGroup;
    QCompleter completerType;

    std::unique_ptr<QWidget> editor;
    std::unique_ptr<PropertyEditor::PropertyItem> propertyItem;
    std::unique_ptr<App::ObjectIdentifier> objectIdentifier;

    // a transactionID of 0 means that there is no active transaction.
    int transactionID;

    QMetaObject::Connection connComboBoxGroup;
    QMetaObject::Connection connComboBoxType;
    QMetaObject::Connection connLineEditNameTextChanged;
};

} // namespace Dialog
} // namespace Gui

#endif // GUI_DIALOG_DLG_ADD_PROPERTY_VARSET_H
