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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include "ComponentDatabase.h"
#include "Utils/ProgramOutputDialog.h"

#include <qgsfeature.h>
#include <qgsfeaturerequest.h>

ComponentDatabase::ComponentDatabase(QString type) : offset(0), componentType(type)
{
    messageHandler = ProgramOutputDialog::getInstance();
}


bool ComponentDatabase::isEmpty(void)
{
    if(mainLayer == nullptr)
        return true;

    if(mainLayer->featureCount() == 0)
        return true;

    return false;
}


void ComponentDatabase::clear(void)
{
    mainLayer = nullptr;
    selectedFeaturesSet.clear();
    offset = 0;
    selectedLayer = nullptr;
}


QgsFeature ComponentDatabase::getFeature(const qint64 id)
{
    if(mainLayer == nullptr)
        return QgsFeature();

    auto fid = id+offset;

    if(FID_IS_NULL(fid))
        return QgsFeature();

    return mainLayer->getFeature(fid);
}


QgsVectorLayer *ComponentDatabase::getSelectedLayer() const
{
    return selectedLayer;
}


QgsVectorLayer *ComponentDatabase::getMainLayer() const
{
    return mainLayer;
}


void ComponentDatabase::startEditing(void)
{
    selectedLayer->startEditing();
}


void ComponentDatabase::commitChanges(void)
{
    selectedLayer->commitChanges(true);
    selectedLayer->updateExtents();
}


void ComponentDatabase::setMainLayer(QgsVectorLayer *value)
{
    mainLayer = value;
}


void ComponentDatabase::setSelectedLayer(QgsVectorLayer *value)
{
    selectedLayer = value;
}


bool ComponentDatabase::addFeaturesToSelectedLayer(const std::set<int> ids)
{
    if(!selectedFeaturesSet.isEmpty())
    {
        selectedFeaturesSet.clear();
        this->clearSelectedLayer();
    }

    selectedFeaturesSet.reserve(ids.size());

    for(auto&& id : ids)
        selectedFeaturesSet.insert(id+offset);

    auto featIt = mainLayer->getFeatures(selectedFeaturesSet);

    QgsFeatureList featList;
    featList.reserve(ids.size());

    QgsFeature feat;
    while (featIt.nextFeature(feat))
    {
        auto id = feat.id();
        featList.push_back(feat);
    }

    if(featList.size() != ids.size())
        return false;

    auto res = selectedLayer->dataProvider()->addFeatures(featList, QgsFeatureSink::FastInsert);

    selectedLayer->updateExtents();

    return res;
}


bool ComponentDatabase::addFeatureToSelectedLayer(const int id)
{
    auto fid = id+offset;

    if(selectedFeaturesSet.contains(fid))
        return true;

    auto feature = this->getFeature(fid);
    if(feature.isValid() == false)
    {
        messageHandler->appendErrorMessage("Error getting the feature from the database");
        return false;
    }

    return this->addFeatureToSelectedLayer(feature);
}


bool ComponentDatabase::addFeatureToSelectedLayer(QgsFeature& feature)
{
    auto fid = feature.id();

    auto res = selectedLayer->dataProvider()->addFeature(feature, QgsFeatureSink::FastInsert);

    // auto res = selectedLayer->addFeature(feature/*, QgsFeatureSink::FastInsert*/);

    if(res == false)
    {
        messageHandler->appendErrorMessage("Error adding feature to selected feature layer");
        return false;
    }

    selectedFeaturesSet.insert(fid);

    return true;
}


void ComponentDatabase::setOffset(int value)
{
    offset = value;
}


bool ComponentDatabase::removeFeaturesFromSelectedLayer(QgsFeatureIds& featureIds)
{
    auto res = selectedLayer->dataProvider()->deleteFeatures(featureIds);

    return res;
}


bool ComponentDatabase::clearSelectedLayer(void)
{
    auto res = selectedLayer->dataProvider()->truncate();

    return res;
}


bool ComponentDatabase::addNewComponentAttributes(const QStringList& fieldNames, const QVector<QgsAttributes>& values, QString& error)
{
    if(selectedLayer == nullptr)
    {
        error = "Error, could not find the 'selected assets layer' containing the assets that were selected for analysis. Could not add new fields";
        return false;
    }

    auto numSelectedFeatures = selectedFeaturesSet.size();

    auto numFeatSelLayer = selectedLayer->featureCount();

    if(values.size() != numSelectedFeatures || numSelectedFeatures != numFeatSelLayer)
    {
        error = "Error, the number of assets in the imported data ("+QString::number(values.size())+ ") should be equal to the number of assets in the 'selected assets layer' (" + QString::number(numSelectedFeatures)+ "). /n Failed to batch add new attributes. Please ensure all assets are loaded and added to the selected features layer in the input file stage.";
        return false;
    }


    if(values.empty())
    {
        error = "Error, empty values.";
        return false;
    }


    auto numNewFields = fieldNames.size();
    auto firstRow = values.first();

    if(firstRow.size() != numNewFields)
    {
        error = "Error, the number of values must match the number of fields";
        return false;
    }


    QgsFields existingFields = selectedLayer->dataProvider()->fields();

    for(int i = 0; i <numNewFields; ++i)
        existingFields.append(QgsField(fieldNames[i], firstRow.at(i).type()));


    QgsFeatureRequest featRequest (selectedFeaturesSet);

    // All this just to get the feature request to return everything with ascending ids
    QgsFeatureRequest::OrderByClause orderByClause(QString("id"),true);
    QList<QgsFeatureRequest::OrderByClause> obcList = {orderByClause};
    QgsFeatureRequest::OrderBy orderBy(obcList);
    featRequest.setOrderBy(orderBy);

    auto featIt = mainLayer->getFeatures(featRequest);

    QgsFeatureList featList;
    featList.reserve(values.size());

    int count = 0;
    QgsFeature feat;
    while (featIt.nextFeature(feat))
    {
        auto existingAtrb = feat.attributes();

        // auto id = feat.id();
        feat.setFields(existingFields, true);

        auto newAtrb = values[count];

        existingAtrb.append(newAtrb);

        // The number of provided attributes need to exactly match the number of the feature's fields.
        feat.setAttributes(existingAtrb);

        featList.push_back(feat);
        ++count;
    }

    if(values.size() != featList.size())
    {
        error = "Error, inconsist sizes of values to update and number of updated features. Please contact developers. Could not add fields to "+selectedLayer->name();
        return false;
    }

    auto res = selectedLayer->dataProvider()->truncate();

    if(!res)
    {
        error = "Error, failed to remove existing features in the 'Selected Asset Layer' data provider. Please contact developers. Could not add fields to "+selectedLayer->name();
        return false;
    }

    res = selectedLayer->dataProvider()->addAttributes(existingFields.toList());

    if(!res)
    {
         error = "Error adding attributes to the layer" + selectedLayer->name();
         return false;
    }

    selectedLayer->updateFields(); // tell the vector layer to fetch changes from the provider

    res = selectedLayer->dataProvider()->addFeatures(featList);

    if(!res)
    {
        error = "Error, failed to add features to the 'Selected Asset Layer' data provider. Please contact developers. Could not add fields to "+selectedLayer->name();
        return false;
    }

    selectedLayer->updateExtents();

    return res;
}



bool ComponentDatabase::updateComponentAttributes(const QString& fieldName, const QVector<QVariant>& values, QString& error)
{
    if(selectedLayer == nullptr)
    {
        error = "Error, could not find the 'selected assets layer' containing the assets that were selected for analysis. Could not batch update field: "+fieldName;
        return false;
    }

    auto numSelectedFeatures = selectedFeaturesSet.size();

    auto numFeatSelLayer = selectedLayer->featureCount();

    if(values.size() != numSelectedFeatures || numSelectedFeatures != numFeatSelLayer)
    {
        error = "Error, the number of assets in the imported data ("+QString::number(values.size())+ ") should be equal to the number of assets in the 'selected assets layer' (" + QString::number(numSelectedFeatures)+ "). /n Failed to batch update attribute: "+fieldName+". Please ensure all assets are loaded and added to the selected features layer in the input file stage.";
        return false;
    }

    auto field = selectedLayer->dataProvider()->fieldNameIndex(fieldName);

    if(field == -1)
    {
        error = "Error, failed to find the field "+fieldName+" in the imported results";
        return false;
    }

    QgsFeatureRequest featRequest (selectedFeaturesSet);

    // All this just to get the feature request to return everything with ascending ids
    QgsFeatureRequest::OrderByClause orderByClause(QString("id"),true);
    QList<QgsFeatureRequest::OrderByClause> obcList = {orderByClause};
    QgsFeatureRequest::OrderBy orderBy(obcList);
    featRequest.setOrderBy(orderBy);

    auto featIt = mainLayer->getFeatures(featRequest);

    QgsFeatureList featList;
    featList.reserve(values.size());

    int count = 0;
    QgsFeature feat;
    while (featIt.nextFeature(feat))
    {
        // auto id = feat.id();
        auto attrb = values[count];

        feat.setAttribute(field,attrb);

        featList.push_back(feat);
        ++count;
    }

    if(values.size() != featList.size())
    {
        error = "Error, inconsist sizes of values to update and number of updated features. Please contact developers. Could not batch update field: "+fieldName;
        return false;
    }

    auto res = selectedLayer->dataProvider()->truncate();

    if(!res)
    {
        error = "Error, failed to remove existing features in the 'Selected Asset Layer' data provider. Please contact developers. Could not batch update field: "+fieldName;
        return false;
    }

    res = selectedLayer->dataProvider()->addFeatures(featList);

    if(!res)
    {
        error = "Error, failed to add features to the 'Selected Asset Layer' data provider. Please contact developers. Could not batch update field: "+fieldName;
        return false;
    }

    selectedLayer->updateExtents();

    return res;
}


bool ComponentDatabase::updateComponentAttribute(const qint64 id, const QString& attribute, const QVariant& value)
{

    auto fid = id+offset;

    auto field = selectedLayer->dataProvider()->fieldNameIndex(attribute);

    if(FID_IS_NULL(fid) || field == -1)
        return false;

    mainLayer->startEditing();

    auto res = mainLayer->changeAttributeValue(fid,field,value);

    mainLayer->commitChanges(true);

    if(!res)
        return res;

    // Update the selected layer if there is one...
    if(selectedLayer != nullptr)
    {
        auto fidSel = selectedFeaturesSet.values().indexOf(id);

        // Still return true if feature is not in the set
        if(fidSel == -1)
            return true;

        auto res2 = selectedLayer->changeAttributeValue(fidSel,field,value);

        if(!res2)
            return res2;
    }

    return true;
}


QVariant ComponentDatabase::getAttributeValue(const qint64 id, const QString& attribute, const QVariant defaultVal)
{
    QVariant val(defaultVal);
    auto fid = id+offset;

    if(FID_IS_NULL(fid))
        return val;

    auto feature = mainLayer->getFeature(fid);

    if(feature.isValid())
        val = feature.attribute(attribute);

    if(!val.isValid())
        return defaultVal;

    return val;
}
