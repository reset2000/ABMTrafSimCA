#ifndef MAINTRAFFICSIMULATOR_H
#define MAINTRAFFICSIMULATOR_H

#include "TrafficCore/Configuration.h"
#include "dbmanager.h"
#include "qfuturewatcher.h"
#include "qtdiagramf.h"
#include "qtdiagramst.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainTrafficSimulator; }
QT_END_NAMESPACE

QString GenerateRoad(Ui::MainTrafficSimulator &ui, Configuration &config);

class MainTrafficSimulator : public QMainWindow

{
    Q_OBJECT

public:
    MainTrafficSimulator(QWidget *parent = nullptr);
    ~MainTrafficSimulator();    

private slots:
    void on_btnStartSimulationConsole_clicked();

    void on_btnSTDiagram_clicked();

    void on_btnFDiagram_clicked();

    void on_btnAddDriverProfile_clicked();

    void on_btnSTDiagramConfig_clicked();

    void on_btnFDiagramConfig_clicked();

    void on_btnReadData_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_btnSaveSTDiagram_clicked();

    void on_btnSaveFDiagram_clicked();

    void on_cbApproximate_stateChanged(int arg1);

    void on_cbLegend_currentIndexChanged(int index);

    void on_cbFont_currentFontChanged(const QFont &f);

    void on_sbFontSize_valueChanged(int arg1);

    void on_cbTheme_currentIndexChanged(int index);

private:
    Ui::MainTrafficSimulator *ui;

    DbManager dbm;

    QtDiagramConfig snapshot;

    QtDiagramF qF;
    QtDiagramST qST;

    QList<QtDiagramF*> qFList;
    QList<QtDiagramST*> qSTList;

    bool firstStyleLoad = true;

    QFutureWatcher<void> *futureWatcher;

    QString checkCorrectProbabilitesAndCounts();

    bool openNewDriverProfileDialog();

    void deleteThisLine();

    void InitializeDriverProfilesTable();

    void InitializeArchiveTable();

    void deleteRecordFromTable();

    void updateUiFromSnapshot();

    void updateDiagramStyleFromSnapshot();

    void updateSnapshotFromUi();

    void showDiagramFData();

    void updateConfig();

    void InitializeDriverProfilesFromArchiveTable(QString dpList);

    void deleteProfileFromArchive();

    bool runSimulation(Configuration &config);

    void paintSTDiagram(string data);

    string GetCurrentDate();

    void paintFDiagram();

    void changeLegend();

    void changeFont();

    void changeTheme();

    void populateThemeBox();

    void updateQtDiagramConfigData(QtDiagramConfig &qtCfg, Configuration &cfg);

    void updateQtDiagramConfigData(QtDiagramConfig &qtCfg, QtDiagramConfig &cfg);

    void closeEvent (QCloseEvent *event);

    void msgBoxTEST();
};

#endif // MAINTRAFFICSIMULATOR_H
