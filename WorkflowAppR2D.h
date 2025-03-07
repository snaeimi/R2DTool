#ifndef WORKFLOW_APP_R2D_H
#define WORKFLOW_APP_R2D_H
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

// Written by: Stevan Gavrilovic, Frank McKenna

#include "WorkflowAppWidget.h"

#include <QMap>

class AnalysisWidget;
class Application;
class LocalApplication;
class AssetsWidget;
class DLWidget;
class EngDemandParameterWidget;
class GeneralInformationWidgetR2D;
class HazardToAssetWidget;
class HazardsWidget;
class UQWidget;
class SystemPerformanceWidget;
class RecoveryWidget;
class ModelWidget;
class QNetworkAccessManager;
class QNetworkReply;
class QStackedWidget;
class RandomVariablesContainer;
class RemoteJobManager;
class RemoteService;
class ResultsWidget;
class LoadResultsDialog;
class RunWidget;
class SimCenterAppWidget;
class SimCenterAppEventSelection;
class SimCenterComponentSelection;
class UQOptions;
class UQWidget;
class UQ_Results;
class QGISVisualizationWidget;
class PerformanceWidget;
class LocalMappingWidget;
class ToolDialog;

class WorkflowAppR2D : public WorkflowAppWidget
{
    Q_OBJECT
public:
    explicit WorkflowAppR2D(RemoteService *theService, QWidget *parent = 0);
    ~WorkflowAppR2D();

    static WorkflowAppR2D *getInstance(void);

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);

    void onRunButtonClicked();
    void onRemoteRunButtonClicked();
    void onRemoteGetButtonClicked();
    void onExitButtonClicked();
    int getMaxNumParallelTasks();
    //int createCitation(QJsonObject &citation, QString &citeFile);
    int createCitation(QJsonObject &citation, QString citeFile);  
    void initialize(void);
    
    void setActiveWidget(SimCenterAppWidget* widget);

    QGISVisualizationWidget *getVisualizationWidget() const;
    AssetsWidget *getAssetsWidget() const;
    GeneralInformationWidgetR2D *getGeneralInformationWidget() const;
    DLWidget *getTheDamageAndLossWidget() const;
    SystemPerformanceWidget *getTheSystemPerformanceWidget() const;
    RecoveryWidget *getTheRecoveryWidget() const;
    ResultsWidget* getTheResultsWidget() const;

    LocalApplication *getLocalApp() const;

signals:

public slots:  
    void clear(void);
    void loadResults(void);
    void setUpForApplicationRun(QString &, QString &);
    void processResults(QString &dirResults);
    int loadFile(QString &filename);
    void replyFinished(QNetworkReply*);
    void assetSelectionChanged(QString, bool);

    void statusMessage(QString message);
    void infoMessage(QString message);
    void errorMessage(QString message);
    void fatalMessage(QString message);
    void runComplete();

private:

    // Sidebar container selection
    SimCenterComponentSelection *theComponentSelection;

    //
    // Widgets that go in sidebar
    //

    ToolDialog* theToolDialog = nullptr;
    QGISVisualizationWidget* theVisualizationWidget;
    GeneralInformationWidgetR2D* theGeneralInformationWidgetR2D;
    HazardsWidget* theHazardsWidget;
    AssetsWidget* theAssetsWidget;
    HazardToAssetWidget* theHazardToAssetWidget;
    ModelWidget* theModelingWidget;
    AnalysisWidget *theAnalysisWidget;
    DLWidget* theDamageAndLossWidget;
    UQWidget* theUQWidget;
    SystemPerformanceWidget *theSystemPerformanceWidget;
    RandomVariablesContainer* theRVs;
    ResultsWidget* theResultsWidget;
    LoadResultsDialog* resultsDialog;
    PerformanceWidget* thePerformanceWidget;
    RecoveryWidget* theRecoveryWidget;
    //LocalMappingWidget* theLocalMappingWidget;  
    SimCenterAppEventSelection* theLocalEvent;  
    //
    // Objects for running the workflow and obtaining results
    //

    RunWidget *theRunWidget;
    LocalApplication *localApp;
    Application *remoteApp;
    RemoteJobManager *theJobManager;

    QJsonObject *jsonObjOrig;
    QNetworkAccessManager *manager;
    QString commonFilePath;
    static WorkflowAppR2D *theInstance;

};

#endif // WORKFLOW_APP_R2D_H
