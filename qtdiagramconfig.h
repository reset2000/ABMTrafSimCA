
#ifndef QTDIAGRAMCONFIG_H
#define QTDIAGRAMCONFIG_H


#include "qdatetime.h"
#include <QObject>


class QtDiagramConfig : public QObject
{
    Q_OBJECT
public:
    int id;

    QDateTime date;
    QString data;

    int roadLength;
    int cyclesNum;
    int cyclesToSkip;
    bool changeLane;

    int currentDensity;
    int densityPoints;
    int densityTests;

    QString carsProbability;
    QString driverProfiles;

    int legend;
    QString font;
    int fontSize;
    int theme;

    explicit QtDiagramConfig(QObject *parent = nullptr);
};

#endif // QTDIAGRAMCONFIG_H
