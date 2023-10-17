
#include "qtdiagramf.h"

QtDiagramF::QtDiagramF(QObject *parent)
    : QObject{parent}
{

}

void QtDiagramF::ShowDensitiesQtApprox(Configuration &cfg, string *flowsStr, ShowSection section) {
    static float densityInterval;
    static float currentDensity;

    switch (section) {
    case SHOW_INIT:
        cfg.data.clear();

        densityInterval = 100.0 / float( cfg.GetDensitiesSize() );
        currentDensity = 0.0;

        for (int c = 0; c < MAX_CARS_SET; c++) {
            cfg.data.append( cfg.GetCarProbability(c) != 0 ? "1" : "0" );
        }

        cfg.data.append( ";" );

        for (int c = 0; c < MAX_CARS_SET; c++) {
            cfg.data.append( c == 0 ? "" : "," );
            cfg.data.append( std::to_string( (int)(cfg.GetCarProbability(c) * 100) ) );
        }

        break;
    case SHOW_MID:
        for (int l = 0; l < MAX_ROAD_LANES; l++) {
            cfg.data.append(";");
            cfg.data.append(flowsStr[l]);
        }

        currentDensity += densityInterval;

        break;
    case SHOW_FINAL:

        break;
    }
}
