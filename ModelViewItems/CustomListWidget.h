#ifndef CustomListWidget_H
#define CustomListWidget_H
/* *****************************************************************************
Copyright (c) 2016-2021, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written by: Stevan Gavrilovic

#include <QTreeView>
#include <QJsonObject>

class QLabel;
class ListTreeModel;
class TreeItem;

class CustomListWidget : public QTreeView
{
    Q_OBJECT

public:
    CustomListWidget(QString headerText = QString(), QWidget *parent = nullptr);

    void clear(void);

    QVariantList getListOfModels(TreeItem* parentItem = nullptr) const;

    QVariantList getListOfWeights(TreeItem* parentItem = nullptr) const;

    QJsonObject getMethods();

    QJsonObject getItemJsonObject(const QString& itemID);

    bool updateItemJsonObject(const QString& itemID, const QJsonObject& obj);

    int getNumberOfItems();

    TreeItem* getCurrentItem();

    TreeItem* getItem(const QModelIndex& index);

    int setCurrentItem(const QString& itemID);

    void selectRow(int i);

    QMap<QString, QJsonObject> getModelsMap() const;

    QStringList getListOfItems(void);

signals:
    void itemRemoved(QJsonObject item);

public slots:

    TreeItem* addItem(const QJsonObject& obj, TreeItem* parent = nullptr);
    TreeItem* addItem(const QString item, QString model, const double weight, TreeItem* parent = nullptr);
    TreeItem* addItem(const QString item, TreeItem* parent = nullptr);

    void update();

    void removeItem(const QString& itemID);

    // Shows the "right-click" menu
    void showPopup(const QPoint &position);

private slots:
    // Runs the action that the user selects on the right-click menu
    void runAction();

private:

    ListTreeModel* treeModel;

    QMap<QString, QJsonObject> modelsMap;    
};

#endif // CustomListWidget_H
