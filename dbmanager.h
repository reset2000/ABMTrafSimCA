#ifndef DBMANAGER_H
#define DBMANAGER_H


#include "qsqldatabase.h"
#include "qtdiagramf.h"
#include "qtdiagramst.h"
#include <QObject>
#include <QDir>

///
/// \brief The DbManager class is responsible for managing database interactions, such as saving and reading diagram data and configuration snapshots
///
class DbManager : public QObject
{
    Q_OBJECT

public:
    ///
    /// \brief The constructor of DBManager class
    /// \param parent - parent object of DbManager, null by default
    ///
    explicit DbManager(QObject *parent = nullptr);

    ///
    /// \brief This method is responsible for initializing all tables necessary to archive the data generated through simulations and save configuration snapshots
    ///
    void createTables();

    ///
    /// \brief Method for saving fundamental diagrams to the database
    /// \param qF - fundamental diagarm that is going to be saved
    ///
    void saveDb(QtDiagramF &qF);
    ///
    /// \brief Method for saving space-time diagrams to the database
    /// \param qST - space-time diagram that is going to be saved
    ///
    void saveDb(QtDiagramST &qST);

    ///
    /// \brief Method for reading fundamental diagrams from the database
    /// \param qFList - pointer to the list of fundamental diagrams, to which QtDiagramF objects from database will be read
    ///
    void readDb(QList<QtDiagramF*> &qFList);

    ///
    /// \brief Method for deleting saved fundamaental diagram records from the database
    /// \param qF - fundamental diagram to be deleted
    ///
    void deleteDb(QtDiagramF &qF);
    ///
    /// \brief Method for deleting space-time diagrams from the database
    /// \param qST - space-time diagram to be deleted
    ///
    void deleteDb(QtDiagramST &qST);

    ///
    /// \brief Method for reading saved snapshot with configuration saved during the previous application session
    /// \param snapshot - object, to which the snapshot will be read
    ///
    void readSnapshot(QtDiagramConfig &snapshot);
    ///
    /// \brief Method for updating snapshots with configuration
    /// \param snapshot - object containing snapshot that will be updated
    ///
    void updateSnapshot(QtDiagramConfig &snapshot);

private:
    ///
    /// \brief QSqlDatabase object necessary for storing data
    ///
    QSqlDatabase db;

    ///
    /// \brief Constant that stores the name of database file
    ///
    const QString DB_NAME = "traffic.sqlite";

    ///
    /// \brief Method for opening database
    ///
    void openDb();
    ///
    /// \brief Method for closing database
    ///
    void closeDb();

    ///
    /// \brief Method that returns the index of the most recent snapshot
    /// \return The most recent snapshot
    ///
    int countSnapshotRows();
};

#endif // DBMANAGER_H
