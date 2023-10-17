#ifndef DBMANAGER_H
#define DBMANAGER_H


#include "qsqldatabase.h"
#include "qtdiagramf.h"
#include "qtdiagramst.h"
#include <QObject>
#include <QDir>


class DbManager : public QObject
{
    Q_OBJECT

public:
    explicit DbManager(QObject *parent = nullptr);

    void createTables();

    void saveDb(QtDiagramF &qF);
    void saveDb(QtDiagramST &qST);

    void readDb(QList<QtDiagramF*> &qFList);

    void deleteDb(QtDiagramF &qF);
    void deleteDb(QtDiagramST &qST);

    void readSnapshot(QtDiagramConfig &snapshot);
    void updateSnapshot(QtDiagramConfig &snapshot);

private:
    QSqlDatabase db;

    const QString DB_NAME = "traffic.sqlite";

    void openDb();
    void closeDb();

    int countSnapshotRows();
};

#endif // DBMANAGER_H
