
#ifndef QTDIAGRAMCONFIG_H
#define QTDIAGRAMCONFIG_H


#include "qdatetime.h"
#include <QObject>

///
/// \brief The QtDiagramConfig class is responsible for saving configuration snapshots after the application is closed
///
class QtDiagramConfig : public QObject
{
    Q_OBJECT
public:
    ///
    /// \brief Database identifier of the configuration snapshot
    ///
    int id;

    ///
    /// \brief Date and time the configuration was saved on
    ///
    QDateTime date;
    ///
    /// \brief The saved configuration data in the form of a string
    ///
    QString data;

    ///
    /// \brief The saved length of the road in segments
    ///
    int roadLength;
    ///
    /// \brief The saved number of iterations of the simulation
    ///
    int cyclesNum;
    ///
    /// \brief The saved number of iterations that will not be displayed
    /// \note These iterations will not be displayed, but the simulation is still generated for them, allowing cars to spread out
    ///
    int cyclesToSkip;
    ///
    /// \brief The saved value defining if the lange changes will occur during the simulation
    ///
    bool changeLane;

    ///
    /// \brief The saved road density percentage.
    /// \note Density represents the percentage value of the cars on the road during the whole simulation (closed loop)
    ///
    int currentDensity;
    ///
    /// \brief The saved number of points of displayed on fundamental diagram
    ///
    int densityPoints;
    ///
    /// \brief The saved number of simulations that are averaged for a single density point on the fundamental diagram
    ///
    int densityTests;

    ///
    /// \brief The saved string representing the probabilities of generation of different sized cars (from 1 to 24 segments)
    ///
    QString carsProbability;
    ///
    /// \brief The saves string representing the probabilities of generation of cars with different characteristics previously set by users.
    ///
    QString driverProfiles;

    ///
    /// \brief This variable contains saved legend display position
    ///
    int legend;
    ///
    /// \brief This variable contains saved font type
    ///
    QString font;
    ///
    /// \brief This variable contains saved font size
    ///
    int fontSize;
    ///
    /// \brief This variable contains saved diagram theme
    ///
    int theme;

    ///
    /// \brief The constructor of the QtDiagramConfig class
    /// \param parent - parent object of QtDiagramConfig, null by default
    ///
    explicit QtDiagramConfig(QObject *parent = nullptr);
};

#endif // QTDIAGRAMCONFIG_H
