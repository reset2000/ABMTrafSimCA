
#ifndef QTDIAGRAMF_H
#define QTDIAGRAMF_H


#include "TrafficCore/Configuration.h"
#include "qchart.h"
#include "qsplineseries.h"
#include "qscatterseries.h"
#include "qtdiagramconfig.h"
#include <QObject>


class QtDiagramF : public QObject
{
    Q_OBJECT

public:
    static const int SUM_LANES = 2;

    QtDiagramConfig cfg;

    QChart chart[MAX_ROAD_LANES + 1];
    QScatterSeries *scatterSeries[MAX_ROAD_LANES + 1][MAX_CARS_SET];
    QSplineSeries *splineSeries[MAX_ROAD_LANES + 1][MAX_CARS_SET];
    QSplineSeries *splineSeriesApprox[MAX_ROAD_LANES + 1][MAX_CARS_SET];

    QStringList laneNames = { "Left Lane", "Right Lane", "Both Lanes" };

    QColor carsColors[MAX_CARS_SET] = {
        QColor::fromString("#1f77b4"),
        QColor::fromString("#ff7f0e"),
        QColor::fromString("#2ca02c"),
        QColor::fromString("#d62728"),
        QColor::fromString("#9467bd"),
        QColor::fromString("#8c564b"),
        QColor::fromString("#e377c2"),
        QColor::fromString("#7f7f7f"),
        QColor::fromString("#bcbd22"),
        QColor::fromString("#17becf"),
    };

    explicit QtDiagramF(QObject *parent = nullptr);
    static void ShowDensitiesQtApprox(Configuration &cfg, string *flowsStr, ShowSection section);

signals:
    void progressBarValueChanged(int value);

};

#endif // QTDIAGRAMF_H
