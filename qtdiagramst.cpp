
#include "qtdiagramst.h"

QtDiagramST::QtDiagramST(QObject *parent)
    : QObject{parent}
{

}

void QtDiagramST::ShowSpaceTimeQt(Configuration &cfg, string *cycle, ShowSection section) {
    switch (section) {
    case SHOW_INIT:
        cfg.data.clear();

        break;
    case SHOW_MID:
        for (int l = 0; l < MAX_ROAD_LANES; l++) {
            cfg.data.append(";");
            cfg.data.append(cycle[l]);
        }

        break;
    case SHOW_FINAL:

        break;
    }
}
