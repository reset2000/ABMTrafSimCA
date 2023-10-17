
#ifndef QTDIAGRAMST_H
#define QTDIAGRAMST_H


#include "TrafficCore/Configuration.h"
#include "qtdiagramconfig.h"
#include <QObject>


class QtDiagramST : public QObject
{
    Q_OBJECT
public:
    QtDiagramConfig cfg;

    explicit QtDiagramST(QObject *parent = nullptr);
    static void ShowSpaceTimeQt(Configuration &cfg, string *cycle, ShowSection section);

signals:

};

#endif // QTDIAGRAMST_H
