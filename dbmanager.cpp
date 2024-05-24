
#include "dbmanager.h"
#include "qsqlquery.h"

DbManager::DbManager(QObject *parent) : QObject{parent} {
    db = QSqlDatabase::addDatabase("QSQLITE");

    QDir dir(QDir::currentPath() + "/db");

    if(!dir.exists()) {
        dir.mkdir(".");
    }

    db.setDatabaseName( dir.absolutePath() + "/" + DB_NAME );

    createTables();
}

void DbManager::createTables() {
    openDb();

    if(db.isOpen()) {
        QSqlQuery createDiagramSTQuery(db);
        createDiagramSTQuery.exec(
            "CREATE TABLE IF NOT EXISTS 'diagramST' ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "roadLength INTEGER,"
            "cyclesNum INTEGER,"
            "cyclesToSkip INTEGER,"
            "changeLane INTEGER,"
            "currentDensity INTEGER,"
            "date TEXT,"
            "diagramData TEXT"
            ");"
        );

        QSqlQuery createDiagramFQuery(db);
        createDiagramFQuery.exec(
            "CREATE TABLE IF NOT EXISTS 'diagramF' ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "roadLength INTEGER,"
            "cyclesNum INTEGER,"
            "cyclesToSkip INTEGER,"
            "changeLane INTEGER,"
            "date TEXT,"
            "diagramData TEXT,"
            "densityPoints INTEGER,"
            "densityTests INTEGER,"
            "driverProfiles TEXT"
            ");"
        );

        QSqlQuery createSnapshot(db);
        createDiagramFQuery.exec(
            "CREATE TABLE IF NOT EXISTS 'snapshot' ("
            "id INTEGER PRIMARY KEY,"
            "roadLength INTEGER,"
            "cyclesNum INTEGER,"
            "cyclesToSkip INTEGER,"
            "changeLane INTEGER,"
            "currentDensity INTEGER,"
            "densityPoints INTEGER,"
            "densityTests INTEGER,"
            "carsProbability TEXT,"
            "driverProfiles TEXT,"
            "legend INTEGER,"
            "font TEXT,"
            "fontSize INTEGER,"
            "theme INTEGER"
            ");"
        );

        if( countSnapshotRows() == 0 ) {
            QSqlQuery insertQuery(db);
            insertQuery.prepare(
                "INSERT INTO 'snapshot' ("
                "id,"
                "roadLength,"
                "cyclesNum,"
                "cyclesToSkip,"
                "changeLane,"
                "currentDensity,"
                "densityPoints,"
                "densityTests,"
                "carsProbability,"
                "driverProfiles,"
                "legend,"
                "font,"
                "fontSize,"
                "theme"
                ") "
                "VALUES ("
                ":id,"
                ":roadLength,"
                ":cyclesNum,"
                ":cyclesToSkip,"
                ":changeLane,"
                ":currentDensity,"
                ":densityPoints,"
                ":densityTests,"
                ":carsProbability,"
                ":driverProfiles,"
                ":legend,"
                ":font,"
                ":fontSize,"
                ":theme"
                ");"
                );

            insertQuery.bindValue(":id", 1);
            insertQuery.bindValue(":roadLength", 400);
            insertQuery.bindValue(":cyclesNum", 300);
            insertQuery.bindValue(":cyclesToSkip", 200);
            insertQuery.bindValue(":changeLane", 0);

            insertQuery.bindValue(":currentDensity", 50);
            insertQuery.bindValue(":densityPoints", 100);
            insertQuery.bindValue(":densityTests", 1);

            insertQuery.bindValue(":carsProbability", "0,0,0,0,0,100,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");
            insertQuery.bindValue(":driverProfiles", "Standard Driver,24,5,2,2,100;Learning Driver,0,0,0,0,0;Aggresive Driver,0,0,0,0,0");

            insertQuery.bindValue(":legend", 1);
            insertQuery.bindValue(":font", "Segoe UI");
            insertQuery.bindValue(":fontSize", 13);
            insertQuery.bindValue(":theme", 0);

            insertQuery.exec();
        }
    }

    closeDb();
}

void DbManager::openDb() {
    if(!db.isOpen()) {
        db.open();
    }
}


void DbManager::closeDb() {
    if(db.isOpen()) {
        db.close();
    }
}

void DbManager::saveDb(QtDiagramF &qF) {
    openDb();

    if(db.isOpen()) {
        QSqlQuery insertQuery(db);
        insertQuery.prepare(
            "INSERT INTO 'diagramF' ("
            "roadLength,"
            "cyclesNum,"
            "cyclesToSkip,"
            "changeLane,"
            "date,"
            "diagramData,"
            "densityPoints,"
            "densityTests,"
            "driverProfiles"
            ") "
            "VALUES ("
            ":roadLength,"
            ":cyclesNum,"
            ":cyclesToSkip,"
            ":changeLane,"
            ":date,"
            ":diagramData,"
            ":densityPoints,"
            ":densityTests,"
            ":driverProfiles"
            ");"
        );

        insertQuery.bindValue(":roadLength", qF.cfg.roadLength);
        insertQuery.bindValue(":cyclesNum", qF.cfg.cyclesNum);
        insertQuery.bindValue(":cyclesToSkip", qF.cfg.cyclesToSkip);
        insertQuery.bindValue(":changeLane", qF.cfg.changeLane);
        insertQuery.bindValue(":date", qF.cfg.date);
        insertQuery.bindValue(":diagramData", qF.cfg.data);
        insertQuery.bindValue(":densityPoints", qF.cfg.densityPoints);
        insertQuery.bindValue(":densityTests", qF.cfg.densityTests);
        insertQuery.bindValue(":driverProfiles", qF.cfg.driverProfiles);

        insertQuery.exec();
    }

    closeDb();
}

void DbManager::readDb(QList<QtDiagramF*> &qFList)
{
    openDb();

    if(db.isOpen()) {
        qFList.clear();

        QSqlQuery query;
        query.exec("SELECT * FROM diagramF");

        while (query.next()) {
            QtDiagramF *qF = new QtDiagramF;

            qF->cfg.id = query.value("id").toInt();

            qF->cfg.date = query.value("date").toDateTime();
            qF->cfg.data = query.value("diagramData").toString();

            qF->cfg.roadLength = query.value("roadLength").toInt();
            qF->cfg.cyclesNum = query.value("cyclesNum").toInt();

            qF->cfg.cyclesToSkip = query.value("cyclesToSkip").toInt();
            qF->cfg.changeLane = query.value("changeLane").toInt();;

            qF->cfg.densityPoints = query.value("densityPoints").toInt();
            qF->cfg.densityTests = query.value("densityTests").toInt();

            qF->cfg.driverProfiles = query.value("driverProfiles").toString();

            qFList.append(qF);
        }
    }

    closeDb();
}

void DbManager::deleteDb(QtDiagramF &qF) {
    openDb();

    if(db.isOpen()) {
        QSqlQuery query;

        query.prepare("DELETE FROM diagramF WHERE id = :id");
        query.bindValue(":id", qF.cfg.id);

        query.exec();
    }

    closeDb();
}

int DbManager::countSnapshotRows() {
    QSqlQuery countRows(db);
    countRows.exec(
        "SELECT COUNT(*) from 'snapshot';"
    );

    countRows.first();
    return countRows.value(0).toInt();
}

void DbManager::readSnapshot(QtDiagramConfig &snapshot) {
    openDb();

    if(db.isOpen()) {
        QSqlQuery query;
        query.exec("SELECT * FROM snapshot WHERE id = 1");

        if(query.next()) {
            snapshot.id = query.value("id").toInt();

            snapshot.roadLength = query.value("roadLength").toInt();
            snapshot.cyclesNum = query.value("cyclesNum").toInt();
            snapshot.cyclesToSkip = query.value("cyclesToSkip").toInt();
            snapshot.changeLane = query.value("changeLane").toInt();;

            snapshot.currentDensity = query.value("currentDensity").toInt();
            snapshot.densityPoints = query.value("densityPoints").toInt();
            snapshot.densityTests = query.value("densityTests").toInt();

            snapshot.carsProbability = query.value("carsProbability").toString();
            snapshot.driverProfiles = query.value("driverProfiles").toString();

            snapshot.legend = query.value("legend").toInt();
            snapshot.font = query.value("font").toString();
            snapshot.fontSize = query.value("fontSize").toInt();
            snapshot.theme = query.value("theme").toInt();
        }
    }

    closeDb();
}

void DbManager::updateSnapshot(QtDiagramConfig &snapshot) {
    openDb();

    if(db.isOpen()) {
        QSqlQuery updateQuery(db);
        updateQuery.prepare( "UPDATE 'snapshot' SET roadLength = :roadLength, cyclesNum = :cyclesNum, cyclesToSkip = :cyclesToSkip, changeLane = :changeLane, currentDensity = :currentDensity, densityPoints = :densityPoints, densityTests = :densityTests, carsProbability = :carsProbability, driverProfiles = :driverProfiles, legend = :legend, font = :font, fontSize = :fontSize, theme = :theme WHERE id = 1;" );

        updateQuery.bindValue(":roadLength", snapshot.roadLength);
        updateQuery.bindValue(":cyclesNum", snapshot.cyclesNum);
        updateQuery.bindValue(":cyclesToSkip", snapshot.cyclesToSkip);
        updateQuery.bindValue(":changeLane", snapshot.changeLane);

        updateQuery.bindValue(":currentDensity", snapshot.currentDensity);
        updateQuery.bindValue(":densityPoints", snapshot.densityPoints);
        updateQuery.bindValue(":densityTests", snapshot.densityTests);

        updateQuery.bindValue(":carsProbability", snapshot.carsProbability);
        updateQuery.bindValue(":driverProfiles", snapshot.driverProfiles);

        updateQuery.bindValue(":legend", snapshot.legend);
        updateQuery.bindValue(":font", snapshot.font);
        updateQuery.bindValue(":fontSize", snapshot.fontSize);
        updateQuery.bindValue(":theme", snapshot.theme);

        updateQuery.exec();
    }

    closeDb();
}
