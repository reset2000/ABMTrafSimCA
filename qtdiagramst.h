
#ifndef QTDIAGRAMST_H
#define QTDIAGRAMST_H


#include "TrafficCore/Configuration.h"
#include "qtdiagramconfig.h"
#include <QObject>

///
/// \brief The QtDiagramST class is responsible for saving data necessary to create and display space-time diagrams
///
class QtDiagramST : public QObject
{
    Q_OBJECT
public:
    ///
    /// \brief Parameter storing the base space-time diagram configuration
    /// \see QtDiagramConfig
    ///
    QtDiagramConfig cfg;

    ///
    /// \brief The constructor of the QtDiagramST class
    /// \param parent - parent object of QtDiagramST, null by default
    ///
    explicit QtDiagramST(QObject *parent = nullptr);
    ///
    /// \brief Class responsible for saving data necessary to create and display space-time diagram
    /// \param cfg - the configuration of current simulation
    /// \param cycle - the current iteration that will be analyzed
    /// \param section - the correct section of the space-time diagram string (SHOW_INIT, SHOW_MID, SHOW_FINAL)
    /// \note This method saves data in cfg object.
    ///
    static void ShowSpaceTimeQt(Configuration &cfg, string *cycle, ShowSection section);

signals:

};

#endif // QTDIAGRAMST_H
