
#ifndef QTDIAGRAMF_H
#define QTDIAGRAMF_H


#include "TrafficCore/Configuration.h"
#include "qchart.h"
#include "qsplineseries.h"
#include "qscatterseries.h"
#include "qtdiagramconfig.h"
#include <QObject>

///
/// \brief The QtDiagramF class is responsible for saving data necessary to create and display fundamental diagrams
///
class QtDiagramF : public QObject
{
    Q_OBJECT

public:
    ///
    /// \brief The constant that determines maximal number of road lanes
    ///
    static const int SUM_LANES = 2;

    ///
    /// \brief Parameter storing the base fundamental diagram configuration
    /// \see QtDiagramConfig
    ///
    QtDiagramConfig cfg;

    ///
    /// \brief Qt chart data
    ///
    QChart chart[MAX_ROAD_LANES + 1];
    ///
    /// \brief The series that stores scatter data that will be displayed on fundamental diagram
    ///
    QScatterSeries *scatterSeries[MAX_ROAD_LANES + 1][MAX_CARS_SET];
    ///
    /// \brief The series that stores spline data that will be displayed on fundamental diagram
    ///
    QSplineSeries *splineSeries[MAX_ROAD_LANES + 1][MAX_CARS_SET];
    ///
    /// \brief The series that stores spline data (approximated with Eigen) that will be displayed on fundamental diagram
    ///
    QSplineSeries *splineSeriesApprox[MAX_ROAD_LANES + 1][MAX_CARS_SET];

    ///
    /// \brief A Qt list with diagram titles
    ///
    QStringList laneNames = { "Left Lane", "Right Lane", "Both Lanes" };

    ///
    /// \brief The array of car colors
    ///
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

    ///
    /// \brief The constructor of the QtDiagramF class
    /// \param parent - parent object of QtDiagramF, null by default
    ///
    explicit QtDiagramF(QObject *parent = nullptr);
    ///
    /// \brief Class responsible for saving data necessary to create and display fundamental diagram
    /// \param cfg - the configuration of current simulation
    /// \param flowsStr - the current simulation data that will be analyzed
    /// \param section - the correct section of the fundamental diagram string (SHOW_INIT, SHOW_MID, SHOW_FINAL)
    ///
    static void ShowDensitiesQtApprox(Configuration &cfg, string *flowsStr, ShowSection section);

signals:

};

#endif // QTDIAGRAMF_H
