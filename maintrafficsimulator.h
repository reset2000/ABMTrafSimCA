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

///
/// \brief The MainTrafficSimulator class is responsible for displaying and managing application window and user interface
///
class MainTrafficSimulator : public QMainWindow

{
    Q_OBJECT

public:
    ///
    /// \brief The constructor of the MainTrafficSimulator class
    /// \param parent - parent object of MainTrafficSimulator, null by default
    ///
    MainTrafficSimulator(QWidget *parent = nullptr);
    ///
    /// \brief The destructor of MainTrafficSimulator class
    ///
    ~MainTrafficSimulator();    

private slots:
    ///
    /// This method is responsible for starting simulation and generating space-time diagram from "Console" tab
    /// \brief Method that is activated after "Generate space-time diagram" button on "Console" tab is clicked
    ///
    void on_btnStartSimulationConsole_clicked();

    ///
    /// This method is responsible for starting simulation and generating space-time diagram from "Space-time diagram" tab
    /// \brief Method that is activated after "Generate space-time diagram" button on "Space-time Diagram" tab is clicked
    ///
    void on_btnSTDiagram_clicked();

    ///
    /// This method is responsible for starting simulation and generating fundamental diagram from "Fundamental diagram" tab
    /// \brief Method that is activated after "Generate fundamental diagram" button on "Fundamental Diagram" tab is clicked
    ///
    void on_btnFDiagram_clicked();

    ///
    /// This method is responsible for starting simulation and generating fundamental diagram from "Fundamental diagram" tab
    /// \brief Method that is activated after "Add driver profile" button on "Configuration" tab is clicked
    ///
    void on_btnAddDriverProfile_clicked();

    ///
    /// This method is responsible for starting simulation and generating space-time diagram from "Configuration" tab
    /// \brief Method that is activated after "Generate space-time diagram" button on "Configuration" tab is clicked
    ///
    void on_btnSTDiagramConfig_clicked();

    ///
    /// This method is responsible for starting simulation and generating fundamental diagram from "Configuration" tab
    /// \brief Method that is activated after "Generate fundamental diagram" button on "Configuration" tab is clicked
    ///
    void on_btnFDiagramConfig_clicked();

    ///
    /// This method is responsible for reading database data and displaying it in "Configuration" tab
    /// \brief Method that activates after "Read data" button on "Archive" tab is clicked
    ///
    void on_btnReadData_clicked();

    ///
    /// This method is responsible for changing tab views
    /// \brief Method that activates after current tab is changed
    /// \param index - index of the selected tab
    ///
    void on_tabWidget_currentChanged(int index);

    ///
    /// This method is responsible for saving space-time diagram data as an image
    /// \brief Method that activates after "Save diagram" button on "Space-time diagram" tab is clicked
    ///
    void on_btnSaveSTDiagram_clicked();

    ///
    /// This method is responsible for saving fundamental diagram data as an image
    /// \brief Method that activates after "Save diagram" button on "Fundamental diagram" tab is clicked
    ///
    void on_btnSaveFDiagram_clicked();

    ///
    /// This method is responsible for changing the state of the trend lines shown on "Fundamental diagram" tab.
    /// It alternates between 3 possible states: no line, line connecting all the density points or approximated line (with the help of Eigen library)
    /// \brief Method that activates after changing the state of the "Approximate" checkbox on the "Fundamental diagram" tab
    ///
    void on_cbApproximate_stateChanged();

    ///
    /// This method changes the position of the legend according to the value chosen in "Legend" filed
    /// \brief Method that activates after changing "Legend" field value in the top bar
    ///
    void on_cbLegend_currentIndexChanged();

    ///
    /// This method changes the font style according to the value chosen in "Font" filed
    /// \brief Method that activates after changing "Font" field value in the top bar
    ///
    void on_cbFont_currentFontChanged();

    ///
    /// This method changes the font size according to the value chosen in "Font size" filed
    /// \brief Method that activates after changing "Fonnt size" field value in the top bar
    ///
    void on_sbFontSize_valueChanged();

    ///
    /// This method changes the diagram theme according to the value chosen in "Theme" filed
    /// \brief Method that activates after changing "Theme" field value in the top bar
    ///
    void on_cbTheme_currentIndexChanged();

private:
    ///
    /// \brief The default UI Qt component
    ///
    Ui::MainTrafficSimulator *ui;

    ///
    /// \brief Database object
    /// \see DbManager
    ///
    DbManager dbm;

    ///
    /// \brief Object storing current application configuration (data set by user in "Configuration" tab)
    /// \see QtDiagramConfig
    ///
    QtDiagramConfig snapshot;

    ///
    /// \brief Object that stores fundamental diagram data
    /// \see QtDiagramF
    ///
    QtDiagramF qF;
    ///
    /// \brief Object that stores space-time diagram data
    /// \see QtDiagramST
    ///
    QtDiagramST qST;

    ///
    /// \brief Qt List of fundamental diagrams that were read from the archive database
    /// \see QtDiagramF
    ///
    QList<QtDiagramF*> qFList;
    ///
    /// \brief Qt List of space-time diagrams that were read from the archive database
    /// \see QtDiagramST
    ///
    QList<QtDiagramST*> qSTList;

    ///
    /// \brief Variable that determines if the font will be read from the latest snapshot or if it will be set to default
    ///
    bool firstStyleLoad = true;

    ///
    /// \brief Ot object necessary for multi-threaded operations
    ///
    QFutureWatcher<void> *futureWatcher;

    ///
    /// This method checks if all the probabilities for car lengths and driver profiles sum up to 100%.
    /// Additionally it also verifies if the number of different chosen lengths or driver profiles doesn't exceed maximum value of 10 (counted separately).
    /// \brief A method that checks the probabilities and counts for car lengths and driver profiles
    /// \return String with warning message (null if operation was compleated successfully)
    ///
    QString checkCorrectProbabilitesAndCounts();

    ///
    /// This methods opens a dialog that allows users to enter new driver profiles with all the necessary data
    /// \brief Method that activates after clicking the "Add driver profile" button on the "Configuration" tab
    /// \return Flag that indicates if the operation was successful or not
    ///
    bool openNewDriverProfileDialog();

    ///
    /// \brief Method that deletes driver profiles
    ///
    void deleteThisLine();

    ///
    /// \brief Method that initializes "Driver profiles" table at the start of the application
    ///
    void InitializeDriverProfilesTable();

    ///
    /// \brief Method that initializes "Archive" table at the start of the application
    ///
    void InitializeArchiveTable();

    ///
    /// \brief Method that deletes records from archive database
    ///
    void deleteRecordFromTable();

    ///
    /// \brief Method that updates saved configuration snapshot every time the application is closed via "Close" button in the title bar
    ///
    void updateUiFromSnapshot();

    ///
    /// \brief Method that updates diagram style based on last saved snapshot
    ///
    void updateDiagramStyleFromSnapshot();

    ///
    /// \brief Method that updates all fields in the "Configuration" tab based on last saved snapshot
    ///
    void updateSnapshotFromUi();

    ///
    /// \brief Method that reads fundamental diagram from a chosen saved archived record
    /// \note This action will overwrite existing fundamental diagram
    ///
    void showDiagramFData();

    ///
    /// \brief Method that updates configuration based on chosen archived record
    /// \note This action will overwrite existing configuration
    ///
    void updateConfig();

    ///
    /// \brief Method that initializes driver profiles from a chosen achived record
    /// \param dpList - string which stores diagram profiles saved in archived record
    ///
    void InitializeDriverProfilesFromArchiveTable(QString dpList);

    ///
    /// \brief Method that deletes record from the archive database
    ///
    void deleteProfileFromArchive();

    ///
    /// \brief Method that starts the simulation based on configuration set in the "Configuration" tab
    /// \param config - configuration data from the "Configuration" tab
    /// \see Configuration
    /// \return Flag that indicates if the operation was successful or not
    ///
    bool runSimulation(Configuration &config);

    ///
    /// \brief Method that draws space-time diagram in the "Space-time diagram" tab after the simulation is completed.
    /// \param data - data of the space-time diagram
    ///
    void paintSTDiagram(string data);

    ///
    /// \brief Method that creates a string used as a record title based on current date
    /// \return String with current date for archive records titles
    ///
    string GetCurrentDate();

    ///
    /// \brief Method that draws fundamental diagram in the "Space-time diagram" tab after the simulation is completed.
    ///
    void paintFDiagram();

    ///
    /// \brief Method that modifies diagram legend based on the current selection
    ///
    void changeLegend();

    ///
    /// \brief Method that modifies font style and size based on the current selection
    ///
    void changeFont();

    ///
    /// \brief Method that modifies theme based on the current selection
    ///
    void changeTheme();

    ///
    /// \brief Method that updated configuration data for both space-time and fundamental diagrams based on current data from "Configuration" tab
    /// \param qtCfg - configuration that will be updated
    /// \param cfg - configuration, from which the data will be recieved
    /// \see QtDiagramConfig
    /// \see Configuration
    ///
    void updateQtDiagramConfigData(QtDiagramConfig &qtCfg, Configuration &cfg);

    ///
    /// \brief Method that updated configuration data for both space-time and fundamental diagrams based on another diagram configuration data
    /// \param qtCfg - configuration that will be updated
    /// \param cfg - configuration, from which the data will be recieved
    /// \see QtDiagramConfig
    /// \see QtDiagramConfig
    ///
    void updateQtDiagramConfigData(QtDiagramConfig &qtCfg, QtDiagramConfig &cfg);

    ///
    /// \brief Method that closes ongoing Qt events
    /// \param event - event to close
    ///
    void closeEvent (QCloseEvent *event);

    ///
    /// \brief Method for testing generating messages for the user (depricated)
    ///
    void msgBoxTEST();
};

#endif // MAINTRAFFICSIMULATOR_H
