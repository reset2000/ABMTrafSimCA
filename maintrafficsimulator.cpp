#include "maintrafficsimulator.h"
#include "./ui_maintrafficsimulator.h"
#include "TrafficCore/Configuration.h"
#include "TrafficCore/Road.h"
#include "qdir.h"
#include "qlegendmarker.h"
#include "qscatterseries.h"
#include "qvalueaxis.h"
#include "qtdiagramst.h"
#include "qtdiagramf.h"

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <QtConcurrent>
#include <iostream>
#include <QCloseEvent>
#include <QMessageBox>
#include <QLineEdit>
#include <QTimer>

#include "Eigen/Dense"
#include <iostream>
#include <cmath>
#include <vector>

bool showDensitiesQt = false;

void InitializeCarLengthTable(Ui::MainTrafficSimulator &ui);
void InitializeDriverProfilesTable(Ui::MainTrafficSimulator &ui);
void polyfit(const std::vector<double> &t,
             const std::vector<double> &v,
             std::vector<double> &coeff,
             int order);

MainTrafficSimulator::MainTrafficSimulator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainTrafficSimulator)
{
    ui->setupUi(this);

    dbm.readSnapshot(snapshot);

    InitializeCarLengthTable(*ui);
    InitializeDriverProfilesTable();

    populateThemeBox();

    ui->lblDPFromArchive->hide();
    ui->tbwDPFromArchive->hide();

    ui->qcvSTDiagramLeft->setRenderHint(QPainter::Antialiasing, true);
    ui->qcvSTDiagramRight->setRenderHint(QPainter::Antialiasing, true);
    ui->qcvFDiagramLeft->setRenderHint(QPainter::Antialiasing, true);
    ui->qcvFDiagramRight->setRenderHint(QPainter::Antialiasing, true);
    ui->qcvFDiagramSum->setRenderHint(QPainter::Antialiasing, true);

    ui->txeConsoleOutput->setReadOnly(true);
    ui->tabWidget->setCurrentIndex(0);
    ui->progressBar->hide();

    ui->btnSaveSTDiagram->setDisabled(true);
    ui->btnSaveFDiagram->setDisabled(true);

    ui->btnStartSimulationConsole->hide();
    ui->btnStartSimulationConsole->setDisabled(true);

    ui->cbLegend->setEnabled(false);
    ui->cbFont->setEnabled(false);
    ui->sbFontSize->setEnabled(false);
    ui->cbTheme->setEnabled(false);

    ui->btnReadData->hide();

    ui->cbApproximate->setEnabled(false);

    ui->lcdNumber->setDigitCount(9);
    ui->lcdNumber->display("00:00.000");
    ui->lcdNumber->setStyleSheet(
        "QLCDNumber{"
        "background-color: rgb(0, 0, 0);"
        "border: 2px solid rgb(113, 113, 113);"
        "border-width: 2px;"
        "border-radius: 5px;"
        "color: rgb(255, 255, 255);"
        "}"
    );

    ui->tabWidget->setTabVisible(1, false);

    updateUiFromSnapshot();
}

void InitializeCarLengthTable(Ui::MainTrafficSimulator &ui) {
    ui.tbwCarLengthProb->setColumnCount( 2 );
    ui.tbwCarLengthProb->setRowCount( MAX_CARS_SET - 1 );
    ui.tbwCarLengthProb->verticalHeader()->setVisible(false);

    QStringList headers = {"Car Length", "Percentage"};
    ui.tbwCarLengthProb->setHorizontalHeaderLabels(headers);
    ui.tbwCarLengthProb->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui.tbwCarLengthProb->setStyleSheet(
        "QTableView::item:selected { color:white; background:#000000; font-weight:900; }"
        "QTableCornerButton::section { background-color:#232326; }"
        "QHeaderView::section { color:white; background-color:#232326; }"
    );

    for (int r = 0; r < ui.tbwCarLengthProb->rowCount(); r++) {
        QTableWidgetItem *carLengthItem = new QTableWidgetItem();
        carLengthItem->setText( QString::number(r + 1) );
        carLengthItem->setFlags(carLengthItem->flags() & ~Qt::ItemIsEditable);
        carLengthItem->setTextAlignment( Qt::AlignCenter );

        ui.tbwCarLengthProb->setItem(r, 0, carLengthItem);

        QSpinBox *sb = new QSpinBox();
        sb->setRange(0, 100);
        sb->setValue(0);
        sb->setSuffix("%");

        sb->setFrame(false);
        sb->setAlignment(Qt::AlignCenter);

        ui.tbwCarLengthProb->setCellWidget(r, 1, sb);
    }
}

void MainTrafficSimulator::InitializeDriverProfilesTable() {
    ui->tbwDriverProfiles->setColumnCount( 7 );

    QList<QString> driverProfilesFromSnapshot = snapshot.driverProfiles.split(";");

    ui->tbwDriverProfiles->verticalHeader()->setVisible(false);

    QStringList headers = {"Driver Type", "Max Speed", "Distance", "Acceleration", "Deceleration", "Percentage", "Remove"};
    ui->tbwDriverProfiles->setHorizontalHeaderLabels(headers);
    ui->tbwDriverProfiles->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tbwDriverProfiles->setStyleSheet(
        "QTableView::item:selected { color:white; background:#000000; font-weight:900; }"
        "QTableCornerButton::section { background-color:#232326; }"
        "QHeaderView::section { color:white; background-color:#232326; }"
    );

    if (snapshot.driverProfiles != "") {
        ui->tbwDriverProfiles->setRowCount( driverProfilesFromSnapshot.size() );

        for (int r = 0; r < driverProfilesFromSnapshot.size(); r++) {
            QStringList rowData = driverProfilesFromSnapshot[r].split(",");

            QTableWidgetItem *driverProfileNameItem = new QTableWidgetItem();
            driverProfileNameItem->setText( rowData[0] );
            driverProfileNameItem->setTextAlignment( Qt::AlignCenter );
            driverProfileNameItem->setFlags(driverProfileNameItem->flags() & ~Qt::ItemIsEditable);

            ui->tbwDriverProfiles->setItem(r, 0, driverProfileNameItem);

            QSpinBox *sbMaxSpeed = new QSpinBox();
            sbMaxSpeed->setRange(0, ::size(SPEEDS_TO_CHAR));
            sbMaxSpeed->setValue( rowData[1].toInt() );

            sbMaxSpeed->setFrame(false);
            sbMaxSpeed->setAlignment(Qt::AlignCenter);

            ui->tbwDriverProfiles->setCellWidget(r, 1, sbMaxSpeed);

            QSpinBox *sbDistance = new QSpinBox();
            sbDistance->setRange(0, 100);
            sbDistance->setValue( rowData[2].toInt() );

            sbDistance->setFrame(false);
            sbDistance->setAlignment(Qt::AlignCenter);

            ui->tbwDriverProfiles->setCellWidget(r, 2, sbDistance);

            QSpinBox *sbAcceleration = new QSpinBox();
            sbAcceleration->setRange(0, 100);
            sbAcceleration->setValue( rowData[3].toInt() );

            sbAcceleration->setFrame(false);
            sbAcceleration->setAlignment(Qt::AlignCenter);

            ui->tbwDriverProfiles->setCellWidget(r, 3, sbAcceleration);

            QSpinBox *sbDeceleration = new QSpinBox();
            sbDeceleration->setRange(0, 100);
            sbDeceleration->setValue( rowData[4].toInt() );

            sbDeceleration->setFrame(false);
            sbDeceleration->setAlignment(Qt::AlignCenter);

            ui->tbwDriverProfiles->setCellWidget(r, 4, sbDeceleration);

            QSpinBox *sbProb = new QSpinBox();
            sbProb->setRange(0, 100);
            sbProb->setValue( rowData[5].toInt() );
            sbProb->setSuffix("%");

            sbProb->setFrame(false);
            sbProb->setAlignment(Qt::AlignCenter);

            ui->tbwDriverProfiles->setCellWidget(r, 5, sbProb);

            QPushButton *btnRemove = new QPushButton();
            btnRemove->setText("Remove");
            btnRemove->setStyleSheet("font-weight: bold; color: white; background-color: #e0102D"); //#ee5e68
            connect(btnRemove, &QPushButton::clicked, this, &MainTrafficSimulator::deleteThisLine);

            ui->tbwDriverProfiles->setCellWidget(r, 6, btnRemove);
        }
    }
}

void MainTrafficSimulator::deleteThisLine()
{
    QWidget *w = qobject_cast<QWidget *>(sender()->parent());
    if(w) {
        int row = ui->tbwDriverProfiles->currentRow();
        ui->tbwDriverProfiles->removeRow(row);

        snapshot.driverProfiles.clear();
        for (int r = 0; r < ui->tbwDriverProfiles->rowCount(); r++) {
            snapshot.driverProfiles.append( r == 0 ? "" : ";" );
            snapshot.driverProfiles.append( ui->tbwDriverProfiles->item(r, 0)->text() );
            snapshot.driverProfiles.append(",");
            snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 1))->value() ) );
            snapshot.driverProfiles.append(",");
            snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 2))->value() ) );
            snapshot.driverProfiles.append(",");
            snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 3))->value() ) );
            snapshot.driverProfiles.append(",");
            snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 4))->value() ) );
            snapshot.driverProfiles.append(",");
            snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 5))->value() ) );
        }

        InitializeDriverProfilesTable();
    }
}

void MainTrafficSimulator::populateThemeBox()
{
    ui->cbTheme->addItem("Light", QChart::ChartThemeLight);
    ui->cbTheme->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    ui->cbTheme->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    ui->cbTheme->addItem("High Contrast", QChart::ChartThemeHighContrast);
}

void MainTrafficSimulator::updateUiFromSnapshot () {
    ui->sbSegmentsNum->setValue(snapshot.roadLength);
    ui->sbCyclesNum->setValue(snapshot.cyclesNum);
    ui->sbCyclesToSkip->setValue(snapshot.cyclesToSkip);
    ui->cbChangeLane->setChecked(snapshot.changeLane);
    ui->sbCurrentDensity->setValue(snapshot.currentDensity);
    ui->cmbDensityPoints->setCurrentIndex( ui->cmbDensityPoints->findText( QString::number( snapshot.densityPoints ) ) );
    ui->sbTestsNum->setValue(snapshot.densityTests);

    QStringList qSplitProb = snapshot.carsProbability.split(",");
    for (int i = 0; i < qSplitProb.size(); i++) {
        qobject_cast<QSpinBox*>(ui->tbwCarLengthProb->cellWidget(i, 1))->setValue( qSplitProb[i].toInt() );
    }

    if (snapshot.driverProfiles != "") {
        QStringList qSplitProfiles = snapshot.driverProfiles.split(";");
        for (int i = 0; i < qSplitProfiles.size(); i++) {
            QStringList qSplitProfile = qSplitProfiles[i].split(",");

            ui->tbwDriverProfiles->item(i, 0)->setText( qSplitProfile[0] );
            qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(i, 1))->setValue( qSplitProfile[1].toInt() );
            qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(i, 2))->setValue( qSplitProfile[2].toInt() );
            qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(i, 3))->setValue( qSplitProfile[3].toInt() );
            qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(i, 4))->setValue( qSplitProfile[4].toInt() );
            qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(i, 5))->setValue( qSplitProfile[5].toInt() );
        }
    }
}

void MainTrafficSimulator::updateDiagramStyleFromSnapshot() {
    if( firstStyleLoad ) {
        ui->cbLegend->setCurrentIndex(snapshot.legend);
        ui->cbFont->setCurrentIndex( ui->cbFont->findText( snapshot.font ) );
        ui->sbFontSize->setValue(snapshot.fontSize);
        ui->cbTheme->setCurrentIndex(snapshot.theme);

        firstStyleLoad = !firstStyleLoad;
    }
}

void MainTrafficSimulator::updateSnapshotFromUi () {
    snapshot.roadLength = ui->sbSegmentsNum->value();
    snapshot.cyclesNum = ui->sbCyclesNum->value();
    snapshot.cyclesToSkip = ui->sbCyclesToSkip->value();
    snapshot.changeLane = ui->cbChangeLane->isChecked();

    snapshot.currentDensity = ui->sbCurrentDensity->value();

    snapshot.densityPoints = ui->cmbDensityPoints->currentText().toInt();
    snapshot.densityTests = ui->sbTestsNum->value();

    snapshot.carsProbability.clear();
    for (int r = 0; r < ui->tbwCarLengthProb->rowCount(); r++) {
        snapshot.carsProbability.append( r == 0 ? "" : "," );
        snapshot.carsProbability.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwCarLengthProb->cellWidget(r, 1))->value() ) );
    }

    snapshot.driverProfiles.clear();
    for (int r = 0; r < ui->tbwDriverProfiles->rowCount(); r++) {
        snapshot.driverProfiles.append( r == 0 ? "" : ";" );
        snapshot.driverProfiles.append( ui->tbwDriverProfiles->item(r, 0)->text() );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 1))->value() ) );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 2))->value() ) );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 3))->value() ) );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 4))->value() ) );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 5))->value() ) );
    }

    snapshot.legend = ui->cbLegend->currentIndex();
    snapshot.font = ui->cbFont->currentText();
    snapshot.fontSize = ui->sbFontSize->value();
    snapshot.theme = ui->cbTheme->currentIndex();
}

QString GenerateRoad(Ui::MainTrafficSimulator &ui, Configuration &config) {
    QString msg;

    config.SetSegmentsNum( ui.sbSegmentsNum->value() );
    config.SetCurrentDensity( ui.sbCurrentDensity->value() );
    config.SetCalculateDensities(showDensitiesQt);
    config.SetUseChangeLane( ui.cbChangeLane->isChecked() );

    config.SetCyclesToSkip( ui.sbCyclesToSkip->value() );
    config.SetCyclesNum( ui.sbCyclesNum->value() + config.GetCyclesToSkip());

    if (config.GetCalculateDensities()) {
        config.AddShowFunction(QtDiagramF::ShowDensitiesQtApprox);
    } else {
        config.AddShowFunction(QtDiagramST::ShowSpaceTimeQt);
    }

    config.SetTestsNumber( ui.sbTestsNum->value() );

    int carLengthProbSum = 0;
    for (int r = 0; r < ui.tbwCarLengthProb->rowCount(); r++) {
        carLengthProbSum += qobject_cast<QSpinBox*>(ui.tbwCarLengthProb->cellWidget(r, 1))->value();
        config.AddCarProbability(
            ui.tbwCarLengthProb->item(r, 0)->text().toInt(),
            qobject_cast<QSpinBox*>(ui.tbwCarLengthProb->cellWidget(r, 1))->value() / 100.0
        );
    }

    DriverProfile *driverProfiles[MAX_NUMBER_OF_DRIVER_TYPES];

    int driverProfileProbSum = 0;
    int probThresholdSum = 0;
    for (int r = 0; r < ui.tbwDriverProfiles->rowCount(); r++) {
        driverProfileProbSum += qobject_cast<QSpinBox*>(ui.tbwDriverProfiles->cellWidget(r, 5))->value();

        if( qobject_cast<QSpinBox*>(ui.tbwDriverProfiles->cellWidget(r, 5))->value() != 0 ) {
            probThresholdSum += qobject_cast<QSpinBox*>(ui.tbwDriverProfiles->cellWidget(r, 5))->value() * 10;
            config.SetVDriverType(
                new DriverProfile(
                    ui.tbwDriverProfiles->item(r, 0)->text().toStdString(),
                    qobject_cast<QSpinBox*>(ui.tbwDriverProfiles->cellWidget(r, 1))->value(),
                    qobject_cast<QSpinBox*>(ui.tbwDriverProfiles->cellWidget(r, 2))->value(),
                    qobject_cast<QSpinBox*>(ui.tbwDriverProfiles->cellWidget(r, 3))->value(),
                    qobject_cast<QSpinBox*>(ui.tbwDriverProfiles->cellWidget(r, 4))->value(),
                    qobject_cast<QSpinBox*>(ui.tbwDriverProfiles->cellWidget(r, 5))->value() / 100.0,
                    probThresholdSum
                )
            );
        }
    }

    if (carLengthProbSum != 100 && driverProfileProbSum != 100) {
        msg = "Car length and driver profile probability must sum up to 100%";
    } else if(carLengthProbSum != 100) {
        msg = "Car length probability must sum up to 100%";
    } else if(driverProfileProbSum != 100) {
        msg = "Driver profile probability must sum up to 100%";
    }

    if (msg.size() != 0) {
        ui.statusbar->setStyleSheet(" QStatusBar{ color:#e0102D; font-weight:bold; } ");
        return msg;
    }

    int densitiesPoints = ui.cmbDensityPoints->currentText().toInt();
    config.ClearDensities();
    if (config.GetCalculateDensities()) {
        if (densitiesPoints == 10) {
            for (int i = 10; i <= 100; i += 10) {
                config.AddDensity(i);
            }
        } else {
            for (int i = 1; i <= 100; i++) {
                config.AddDensity(i);
            }
        }

        Road::GenerateDensities(config);
    } else {
       Road road(config);
       road.GenerateCycles();
    }

    return msg;
}

MainTrafficSimulator::~MainTrafficSimulator()
{
    delete ui;
}

bool MainTrafficSimulator::runSimulation(Configuration &config) {
    QString msg = "";
    msg.append( GenerateRoad(*ui, config) );
    if(msg.size() > 0) {
        ui->statusbar->showMessage( QString().append("  ").append(msg) , 5000);
        return false;
    }

    return true;
}

void MainTrafficSimulator::on_btnStartSimulationConsole_clicked()
{
    Configuration config;

    showDensitiesQt = false;
    if ( runSimulation(config) ) {
        ui->btnSaveSTDiagram->setDisabled(false);

        ui->tabWidget->setCurrentIndex(1);
    }
}

bool MainTrafficSimulator::openNewDriverProfileDialog() {
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Add new profile");

    QGridLayout *layout = new QGridLayout(dialog);

    QLabel *lbName = new QLabel;
    lbName->setText("Name: ");
    lbName->setAlignment(Qt::AlignRight);
    layout->addWidget(lbName, 0, 0);

    QLabel *lbMaxSpeed = new QLabel;
    lbMaxSpeed->setText("Max speed: ");
    lbMaxSpeed->setAlignment(Qt::AlignRight);
    layout->addWidget(lbMaxSpeed, 1, 0);

    QLabel *lbDistance = new QLabel;
    lbDistance->setText("Distance: ");
    lbDistance->setAlignment(Qt::AlignRight);
    layout->addWidget(lbDistance, 2, 0);

    QLabel *lbAcceleration = new QLabel;
    lbAcceleration->setText("Acceleration: ");
    lbAcceleration->setAlignment(Qt::AlignRight);
    layout->addWidget(lbAcceleration, 3, 0);

    QLabel *lbDeceleration = new QLabel;
    lbDeceleration->setText("Deceleration: ");
    lbDeceleration->setAlignment(Qt::AlignRight);
    layout->addWidget(lbDeceleration, 4, 0);

    QLineEdit *leName = new QLineEdit;
    layout->addWidget(leName, 0, 1);

    QSpinBox *sbMaxSpeed = new QSpinBox;
    sbMaxSpeed->setRange(0, ::size(SPEEDS_TO_CHAR));
    sbMaxSpeed->setValue( 0 );
    layout->addWidget(sbMaxSpeed, 1, 1);

    QSpinBox *sbDistance = new QSpinBox;
    sbDistance->setRange(0, 100);
    sbDistance->setValue( 0 );
    layout->addWidget(sbDistance, 2, 1);

    QSpinBox *sbAcceleration = new QSpinBox;
    sbAcceleration->setRange(0, 100);
    sbAcceleration->setValue( 0 );
    layout->addWidget(sbAcceleration, 3, 1);

    QSpinBox *sbDeceleration = new QSpinBox;
    sbDeceleration->setRange(0, 100);
    sbDeceleration->setValue( 0 );
    layout->addWidget(sbDeceleration, 4, 1);

    QStatusBar *qsb = new QStatusBar;
    layout->addWidget(qsb, 6, 0, 1, 2);

    QPushButton *btnSave = new QPushButton();
    btnSave->setText("Add profile");
    connect(btnSave, &QPushButton::clicked, this, [=] {
        for (int r = 0; r < ui->tbwDriverProfiles->rowCount(); r++) {
            if ( leName->text() == ui->tbwDriverProfiles->item(r, 0)->text() ) {
                qsb->setStyleSheet(" QStatusBar{ color:#e0102D; } ");
                qsb->showMessage("Driver profile name must be unique", 5000);

                return;
            }
        }

        snapshot.driverProfiles.append( ";" );
        snapshot.driverProfiles.append( leName->text() );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( sbMaxSpeed->value() ) );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( sbDistance->value() ) );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( sbAcceleration->value() ) );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( sbDeceleration->value() ) );
        snapshot.driverProfiles.append(",");
        snapshot.driverProfiles.append( QString::number( 0 ) );

        dialog->close();
    } );
    layout->addWidget(btnSave, 5, 0, 1, 2);

    dialog->exec();

    return true;
}

void MainTrafficSimulator::on_btnAddDriverProfile_clicked()
{
    openNewDriverProfileDialog();
    InitializeDriverProfilesTable();
}

void MainTrafficSimulator::InitializeArchiveTable() {
    ui->tbwArchiveDiagramF->setColumnCount( 10 );
    ui->tbwArchiveDiagramF->setRowCount( qFList.size() );
    ui->tbwArchiveDiagramF->verticalHeader()->setVisible(false);

    QStringList headers = {
        "Date",
        "Road length",
        "Number of cycles",
        "Start-up cycles",
        "Lane change",
        "Density points",
        "Tests per point",
        "Load configuration",
        "Show diagram",
        "Delete record"
    };

    ui->tbwArchiveDiagramF->setHorizontalHeaderLabels(headers);
    ui->tbwArchiveDiagramF->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tbwArchiveDiagramF->setStyleSheet(
        "QTableView::item:selected { color:white; background:#000000; font-weight:900; }"
        "QTableCornerButton::section { background-color:#232326; }"
        "QHeaderView::section { color:white; background-color:#232326; }"
        );

    for (int r = 0; r < ui->tbwArchiveDiagramF->rowCount(); r++) {
        QTableWidgetItem *dateItem = new QTableWidgetItem();
        dateItem->setText( qFList[r]->cfg.date.toString() );
        dateItem->setFlags( dateItem->flags() & ~Qt::ItemIsEditable );
        dateItem->setTextAlignment( Qt::AlignCenter );

        ui->tbwArchiveDiagramF->setItem(r, 0, dateItem);


        QTableWidgetItem *roadLengthItem = new QTableWidgetItem();
        roadLengthItem->setText( QString::number( qFList[r]->cfg.roadLength ) );
        roadLengthItem->setFlags( roadLengthItem->flags() & ~Qt::ItemIsEditable );
        roadLengthItem->setTextAlignment( Qt::AlignCenter );

        ui->tbwArchiveDiagramF->setItem(r, 1, roadLengthItem);


        QTableWidgetItem *cyclesNumItem = new QTableWidgetItem();
        cyclesNumItem->setText( QString::number( qFList[r]->cfg.cyclesNum ) );
        cyclesNumItem->setFlags( cyclesNumItem->flags() & ~Qt::ItemIsEditable );
        cyclesNumItem->setTextAlignment( Qt::AlignCenter );

        ui->tbwArchiveDiagramF->setItem(r, 2, cyclesNumItem);


        QTableWidgetItem *cyclesToSkipItem = new QTableWidgetItem();
        cyclesToSkipItem->setText( QString::number( qFList[r]->cfg.cyclesToSkip ) );
        cyclesToSkipItem->setFlags( cyclesToSkipItem->flags() & ~Qt::ItemIsEditable );
        cyclesToSkipItem->setTextAlignment( Qt::AlignCenter );

        ui->tbwArchiveDiagramF->setItem(r, 3, cyclesToSkipItem);


        QCheckBox *cbChangeLaneItem = new QCheckBox();
        cbChangeLaneItem->setChecked( qFList[r]->cfg.changeLane );
        cbChangeLaneItem->setDisabled(true);

        QHBoxLayout *layout = new QHBoxLayout();
        layout->setAlignment( Qt::AlignCenter );

        QWidget *widget = new QWidget();
        widget->setLayout( layout );
        layout->addWidget( cbChangeLaneItem );

        ui->tbwArchiveDiagramF->setCellWidget(r, 4, widget);


        QTableWidgetItem *densityPointsItem = new QTableWidgetItem();
        densityPointsItem->setText( QString::number( qFList[r]->cfg.densityPoints ) );
        densityPointsItem->setFlags( densityPointsItem->flags() & ~Qt::ItemIsEditable );
        densityPointsItem->setTextAlignment( Qt::AlignCenter );

        ui->tbwArchiveDiagramF->setItem(r, 5, densityPointsItem);


        QTableWidgetItem *testsNumItem = new QTableWidgetItem();
        testsNumItem->setText( QString::number( qFList[r]->cfg.densityTests ) );
        testsNumItem->setFlags( cyclesToSkipItem->flags() & ~Qt::ItemIsEditable );
        testsNumItem->setTextAlignment( Qt::AlignCenter );

        ui->tbwArchiveDiagramF->setItem(r, 6, testsNumItem);


        QPushButton *btnUpdateConfig = new QPushButton();
        btnUpdateConfig->setText("Load");
        btnUpdateConfig->setStyleSheet("font-weight: bold; color: white; background-color: #555555"); //#ee5e68
        connect(btnUpdateConfig, &QPushButton::clicked, this, &MainTrafficSimulator::updateConfig);

        ui->tbwArchiveDiagramF->setCellWidget(r, 7, btnUpdateConfig);


        QPushButton *btnShow = new QPushButton();
        btnShow->setText("Show");
        btnShow->setStyleSheet("font-weight: bold; color: white; background-color: #33b249"); //#ee5e68
        connect(btnShow, &QPushButton::clicked, this, &MainTrafficSimulator::showDiagramFData);

        ui->tbwArchiveDiagramF->setCellWidget(r, 8, btnShow);


        QPushButton *btnDelete = new QPushButton();
        btnDelete->setText("Delete");
        btnDelete->setStyleSheet("font-weight: bold; color: white; background-color: #e0102D"); //#ee5e68
        connect(btnDelete, &QPushButton::clicked, this, &MainTrafficSimulator::deleteRecordFromTable);

        ui->tbwArchiveDiagramF->setCellWidget(r, 9, btnDelete);
    }
}

void MainTrafficSimulator::updateConfig()
{
    QWidget *w = qobject_cast<QWidget *>(sender()->parent());
    if(w) {
        QMessageBox *msb = new QMessageBox();
        msb->setText("Current configuration data will be overwriten.");
        msb->setInformativeText("Do you want to continue?");
        msb->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (msb->exec() == QMessageBox::No ) {
            return;
        }

        int row = ui->tbwArchiveDiagramF->currentRow();

        ui->sbSegmentsNum->setValue( qFList[row]->cfg.roadLength );
        ui->sbCyclesNum->setValue( qFList[row]->cfg.cyclesNum );
        ui->sbCyclesToSkip->setValue( qFList[row]->cfg.cyclesToSkip );
        ui->cbChangeLane->setChecked( qFList[row]->cfg.changeLane );

        int idx = ui->cmbDensityPoints->findText( QString::number( qFList[row]->cfg.densityPoints ) );
        ui->cmbDensityPoints->setCurrentIndex( idx );
        ui->sbTestsNum->setValue( qFList[row]->cfg.densityTests );

        QStringList qSplit = qFList[row]->cfg.data.split(";")[1].split(",");

        for (int r = 0; r < ui->tbwCarLengthProb->rowCount(); r++) {
            qobject_cast<QSpinBox*>(ui->tbwCarLengthProb->cellWidget(r, 1))->setValue( qSplit[r + 1].toInt() );
        }

        ui->tabWidget->setCurrentIndex(0);

        ui->lblDPFromArchive->show();
        ui->tbwDPFromArchive->show();
        InitializeDriverProfilesFromArchiveTable(qFList[row]->cfg.driverProfiles);
    }
}

void MainTrafficSimulator::InitializeDriverProfilesFromArchiveTable(QString dpList) {
    ui->tbwDPFromArchive->setColumnCount( 8 );

    QList<QString> driverProfiles = dpList.split(";");

    ui->tbwDPFromArchive->verticalHeader()->setVisible(false);

    QStringList headers = {"Driver Type", "Max Speed", "Distance", "Acceleration", "Deceleration", "Percentage", "Remove", "Add Profile"};
    ui->tbwDPFromArchive->setHorizontalHeaderLabels(headers);
    ui->tbwDPFromArchive->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tbwDPFromArchive->setStyleSheet(
        "QTableView::item:selected { color:white; background:#000000; font-weight:900; }"
        "QTableCornerButton::section { background-color:#232326; }"
        "QHeaderView::section { color:white; background-color:#232326; }"
        );

    if (dpList != "") {
        ui->tbwDPFromArchive->setRowCount( driverProfiles.size() );

        for (int r = 0; r < driverProfiles.size(); r++) {
            QStringList rowData = driverProfiles[r].split(",");

            QTableWidgetItem *driverProfileNameItem = new QTableWidgetItem();
            driverProfileNameItem->setText( rowData[0] );
            driverProfileNameItem->setTextAlignment( Qt::AlignCenter );
            driverProfileNameItem->setFlags(driverProfileNameItem->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsEnabled);

            ui->tbwDPFromArchive->setItem(r, 0, driverProfileNameItem);

            QSpinBox *sbMaxSpeed = new QSpinBox();
            sbMaxSpeed->setRange(0, ::size(SPEEDS_TO_CHAR));
            sbMaxSpeed->setValue( rowData[1].toInt() );

            sbMaxSpeed->setFrame(false);
            sbMaxSpeed->setAlignment(Qt::AlignCenter);
            sbMaxSpeed->setEnabled(false);

            ui->tbwDPFromArchive->setCellWidget(r, 1, sbMaxSpeed);

            QSpinBox *sbDistance = new QSpinBox();
            sbDistance->setRange(0, 100);
            sbDistance->setValue( rowData[2].toInt() );

            sbDistance->setFrame(false);
            sbDistance->setAlignment(Qt::AlignCenter);
            sbDistance->setEnabled(false);

            ui->tbwDPFromArchive->setCellWidget(r, 2, sbDistance);

            QSpinBox *sbAcceleration = new QSpinBox();
            sbAcceleration->setRange(0, 100);
            sbAcceleration->setValue( rowData[3].toInt() );

            sbAcceleration->setFrame(false);
            sbAcceleration->setAlignment(Qt::AlignCenter);
            sbAcceleration->setEnabled(false);

            ui->tbwDPFromArchive->setCellWidget(r, 3, sbAcceleration);

            QSpinBox *sbDeceleration = new QSpinBox();
            sbDeceleration->setRange(0, 100);
            sbDeceleration->setValue( rowData[4].toInt() );

            sbDeceleration->setFrame(false);
            sbDeceleration->setAlignment(Qt::AlignCenter);
            sbDeceleration->setEnabled(false);

            ui->tbwDPFromArchive->setCellWidget(r, 4, sbDeceleration);

            QSpinBox *sbProb = new QSpinBox();
            sbProb->setRange(0, 100);
            sbProb->setValue( rowData[5].toInt() );
            sbProb->setSuffix("%");

            sbProb->setFrame(false);
            sbProb->setAlignment(Qt::AlignCenter);
            sbProb->setEnabled(false);

            ui->tbwDPFromArchive->setCellWidget(r, 5, sbProb);

            QPushButton *btnRemove = new QPushButton();
            btnRemove->setText("Remove");
            btnRemove->setStyleSheet("font-weight: bold; color: white; background-color: #e0102D"); //#ee5e68
            connect(btnRemove, &QPushButton::clicked, this, &MainTrafficSimulator::deleteProfileFromArchive);

            ui->tbwDPFromArchive->setCellWidget(r, 6, btnRemove);

            QPushButton *btnAddProfile = new QPushButton();
            btnAddProfile->setText("Add Profile");
            btnAddProfile->setStyleSheet("font-weight: bold; color: white; background-color: #33b249"); //#ee5e68
            connect(btnAddProfile, &QPushButton::clicked, this, [=] {
                QDialog *dialog = new QDialog;
                dialog->setWindowTitle("Add profile from archive");

                QGridLayout *layout = new QGridLayout(dialog);

                QLabel *lbName = new QLabel;
                lbName->setText("Name: ");
                lbName->setAlignment(Qt::AlignRight);
                layout->addWidget(lbName, 0, 0);

                QLabel *lbMaxSpeed = new QLabel;
                lbMaxSpeed->setText("Max speed: ");
                lbMaxSpeed->setAlignment(Qt::AlignRight);
                layout->addWidget(lbMaxSpeed, 1, 0);

                QLabel *lbDistance = new QLabel;
                lbDistance->setText("Distance: ");
                lbDistance->setAlignment(Qt::AlignRight);
                layout->addWidget(lbDistance, 2, 0);

                QLabel *lbAcceleration = new QLabel;
                lbAcceleration->setText("Acceleration: ");
                lbAcceleration->setAlignment(Qt::AlignRight);
                layout->addWidget(lbAcceleration, 3, 0);

                QLabel *lbDeceleration = new QLabel;
                lbDeceleration->setText("Deceleration: ");
                lbDeceleration->setAlignment(Qt::AlignRight);
                layout->addWidget(lbDeceleration, 4, 0);

                QLabel *lbPercentage = new QLabel;
                lbPercentage->setText("Percentage: ");
                lbPercentage->setAlignment(Qt::AlignRight);
                layout->addWidget(lbPercentage, 5, 0);

                QLineEdit *leName = new QLineEdit;
                leName->setText( rowData[0] );
                layout->addWidget(leName, 0, 1);

                QSpinBox *sbMaxSpeed = new QSpinBox;
                sbMaxSpeed->setRange(0, ::size(SPEEDS_TO_CHAR));
                sbMaxSpeed->setValue( rowData[1].toInt() );
                layout->addWidget(sbMaxSpeed, 1, 1);

                QSpinBox *sbDistance = new QSpinBox;
                sbDistance->setRange(0, 100);
                sbDistance->setValue( rowData[2].toInt() );
                layout->addWidget(sbDistance, 2, 1);

                QSpinBox *sbAcceleration = new QSpinBox;
                sbAcceleration->setRange(0, 100);
                sbAcceleration->setValue( rowData[3].toInt() );
                layout->addWidget(sbAcceleration, 3, 1);

                QSpinBox *sbDeceleration = new QSpinBox;
                sbDeceleration->setRange(0, 100);
                sbDeceleration->setValue( rowData[4].toInt() );
                layout->addWidget(sbDeceleration, 4, 1);

                QSpinBox *sbPercentage = new QSpinBox;
                sbPercentage->setRange(0, 100);
                sbPercentage->setValue( rowData[5].toInt() );
                layout->addWidget(sbPercentage, 5, 1);

                QStatusBar *qsb = new QStatusBar;
                layout->addWidget(qsb, 7, 0, 1, 2);

                QPushButton *btnSave = new QPushButton();
                btnSave->setText("Add profile");
                connect(btnSave, &QPushButton::clicked, this, [=] {
                    for (int r = 0; r < ui->tbwDriverProfiles->rowCount(); r++) {
                        if ( leName->text() == ui->tbwDriverProfiles->item(r, 0)->text() ) {
                            QMessageBox *msb = new QMessageBox();
                            msb->setText("Driver with the same name already exists.");
                            msb->setInformativeText("Do you want to overwrite existing driver profile?");
                            msb->setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

                            switch (msb->exec()) {
                            case QMessageBox::Yes:
                                qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 1))->setValue( sbMaxSpeed->value() );
                                qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 2))->setValue( sbDistance->value() );
                                qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 3))->setValue( sbAcceleration->value() );
                                qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 4))->setValue( sbDeceleration->value() );
                                qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 5))->setValue( sbPercentage->value() );

                                snapshot.driverProfiles.clear();
                                for (int r = 0; r < ui->tbwDriverProfiles->rowCount(); r++) {
                                    snapshot.driverProfiles.append( r == 0 ? "" : ";" );
                                    snapshot.driverProfiles.append( ui->tbwDriverProfiles->item(r, 0)->text() );
                                    snapshot.driverProfiles.append(",");
                                    snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 1))->value() ) );
                                    snapshot.driverProfiles.append(",");
                                    snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 2))->value() ) );
                                    snapshot.driverProfiles.append(",");
                                    snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 3))->value() ) );
                                    snapshot.driverProfiles.append(",");
                                    snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 4))->value() ) );
                                    snapshot.driverProfiles.append(",");
                                    snapshot.driverProfiles.append( QString::number( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 5))->value() ) );
                                }

                                dialog->close();

                                deleteProfileFromArchive();
                                ui->tbwDriverProfiles->update();

                                return;
                            case QMessageBox::No:
                                dialog->close();

                                return;
                            case QMessageBox::Cancel:
                                return;
                            }
                        }
                    }

                    snapshot.driverProfiles.append( ";" );
                    snapshot.driverProfiles.append( leName->text() );
                    snapshot.driverProfiles.append(",");
                    snapshot.driverProfiles.append( QString::number( sbMaxSpeed->value() ) );
                    snapshot.driverProfiles.append(",");
                    snapshot.driverProfiles.append( QString::number( sbDistance->value() ) );
                    snapshot.driverProfiles.append(",");
                    snapshot.driverProfiles.append( QString::number( sbAcceleration->value() ) );
                    snapshot.driverProfiles.append(",");
                    snapshot.driverProfiles.append( QString::number( sbDeceleration->value() ) );
                    snapshot.driverProfiles.append(",");
                    snapshot.driverProfiles.append( QString::number( 0 ) );

                    dialog->close();

                    deleteProfileFromArchive();
                    InitializeDriverProfilesTable();
                } );
                layout->addWidget(btnSave, 6, 0, 1, 2);

                dialog->exec();

            } );

            ui->tbwDPFromArchive->setCellWidget(r, 7, btnAddProfile);
        }
    }
}

void MainTrafficSimulator::deleteProfileFromArchive()
{
    QWidget *w = qobject_cast<QWidget *>(sender()->parent());
    if(w) {
        int row = ui->tbwDPFromArchive->currentRow();
        ui->tbwDPFromArchive->removeRow(row);

        if( ui->tbwDPFromArchive->rowCount() == 0 ) {
            ui->lblDPFromArchive->hide();
            ui->tbwDPFromArchive->hide();
        }
    }
}

void MainTrafficSimulator::showDiagramFData()
{
    QWidget *w = qobject_cast<QWidget *>(sender()->parent());
    if(w) {
        QMessageBox *msb = new QMessageBox();
        msb->setText("Current diagram data will be overwriten.");
        msb->setInformativeText("Do you want to continue?");
        msb->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (msb->exec() == QMessageBox::No ) {
            return;
        }

        int row = ui->tbwArchiveDiagramF->currentRow();

        updateQtDiagramConfigData(qF.cfg, qFList[row]->cfg);

        paintFDiagram();

        ui->btnSaveFDiagram->setEnabled(true);
        ui->cbApproximate->setEnabled(true);

        ui->tabWidget->setCurrentIndex(3);

        dbm.readDb(qFList);
        InitializeArchiveTable();
    }
}

void MainTrafficSimulator::deleteRecordFromTable()
{
    QWidget *w = qobject_cast<QWidget *>(sender()->parent());
    if(w) {
        QMessageBox *msb = new QMessageBox();
        msb->setText("Archived data will be deleted permanently.");
        msb->setInformativeText("Do you want to continue?");
        msb->setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (msb->exec() == QMessageBox::No ) {
            return;
        }

        int row = ui->tbwArchiveDiagramF->currentRow();

        dbm.deleteDb(*qFList[row]);

        dbm.readDb(qFList);
        InitializeArchiveTable();
    }
}

void MainTrafficSimulator::on_btnReadData_clicked()
{
    dbm.readDb(qFList);
    InitializeArchiveTable();
}

void MainTrafficSimulator::on_tabWidget_currentChanged(int index)
{
    if (ui->tabWidget->currentIndex() == 4) {
        ui->statusbar->setStyleSheet(" QStatusBar{ color:#000000; font-weight:bold; } ");
        ui->statusbar->showMessage("Loading data from archive...");

        dbm.readDb(qFList);
        InitializeArchiveTable();

        ui->statusbar->showMessage("Archive data loaded successfully!", 5000);
    }
}

void MainTrafficSimulator::updateQtDiagramConfigData(QtDiagramConfig &qtCfg, Configuration &cfg) {
    qtCfg.date = QDateTime::currentDateTime();
    qtCfg.data = QString::fromStdString(cfg.data);

    qtCfg.roadLength = cfg.GetSegmentsNum();
    qtCfg.cyclesNum = cfg.GetCyclesNum() - cfg.GetCyclesToSkip();
    qtCfg.cyclesToSkip = cfg.GetCyclesToSkip();
    qtCfg.changeLane = cfg.GetUseChangeLane();

    qtCfg.currentDensity = cfg.GetCurrentDensity();
    qtCfg.densityPoints = cfg.GetDensitiesSize();
    qtCfg.densityTests = cfg.GetTestsNumber();

    qtCfg.driverProfiles.clear();
    for (int r = 0; r < cfg.vDriverProfiles.size(); r++) {
        qtCfg.driverProfiles.append( r == 0 ? "" : ";" );
        qtCfg.driverProfiles.append( cfg.vDriverProfiles[r].GetName() );
        qtCfg.driverProfiles.append(",");
        qtCfg.driverProfiles.append( std::to_string( cfg.vDriverProfiles[r].GetMaxSpeed() ) );
        qtCfg.driverProfiles.append(",");
        qtCfg.driverProfiles.append( std::to_string( cfg.vDriverProfiles[r].GetDR() ) );
        qtCfg.driverProfiles.append(",");
        qtCfg.driverProfiles.append( std::to_string( cfg.vDriverProfiles[r].GetAccPlus() )  );
        qtCfg.driverProfiles.append(",");
        qtCfg.driverProfiles.append( std::to_string( cfg.vDriverProfiles[r].GetAccMinus() ) );
        qtCfg.driverProfiles.append(",");
        qtCfg.driverProfiles.append( std::to_string( (int)(cfg.vDriverProfiles[r].GetProbability() * 100) ) );
    }
}

void MainTrafficSimulator::updateQtDiagramConfigData(QtDiagramConfig &qtCfg, QtDiagramConfig &cfg) {
    qtCfg.id = cfg.id;

    qtCfg.date = cfg.date;
    qtCfg.data = cfg.data;

    qtCfg.roadLength = cfg.roadLength;
    qtCfg.cyclesNum = cfg.cyclesNum;
    qtCfg.cyclesToSkip = cfg.cyclesToSkip;
    qtCfg.changeLane = cfg.cyclesToSkip;

    qtCfg.currentDensity = cfg.currentDensity;
    qtCfg.densityPoints = cfg.densityPoints;
    qtCfg.densityTests = cfg.densityTests;

    qtCfg.driverProfiles = cfg.driverProfiles;
}

void MainTrafficSimulator::msgBoxTEST() {
    QMessageBox msgBox;
    msgBox.setText("The document has been modified.");
    msgBox.exec();
}

QString MainTrafficSimulator::checkCorrectProbabilitesAndCounts() {
    QString msg = "";

    int carLengthProbSum = 0;
    int carLengthProbCount = 0;
    for (int r = 0; r < ui->tbwCarLengthProb->rowCount(); r++) {
        carLengthProbSum += qobject_cast<QSpinBox*>(ui->tbwCarLengthProb->cellWidget(r, 1))->value();

        if ( qobject_cast<QSpinBox*>(ui->tbwCarLengthProb->cellWidget(r, 1))->value() != 0 ) {
            carLengthProbCount++;
        }
    }

    int driverProfileProbSum = 0;
    int driverProfileProbCount = 0;
    for (int r = 0; r < ui->tbwDriverProfiles->rowCount(); r++) {
        driverProfileProbSum += qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 5))->value();

        if ( qobject_cast<QSpinBox*>(ui->tbwDriverProfiles->cellWidget(r, 5))->value() != 0 ) {
            driverProfileProbCount++;
        }
    }

    if( carLengthProbCount > 10 && driverProfileProbCount > 10 ) {
        msg = "Maximum of 10 car lengths or driver profiles may have assigned non-zero percentage";
    } else if ( carLengthProbCount > 10 ) {
        msg = "Maximum of 10 car lengths may have assigned non-zero percentage";
    } else if ( driverProfileProbCount > 10 ) {
        msg = "Maximum of 10 driver profiles may have assigned non-zero percentage";

    }

    if ( carLengthProbSum != 100 && driverProfileProbSum != 100 ) {
        msg = "Car length and driver profile probability must sum up to 100%";
    } else if( carLengthProbSum != 100 ) {
        msg = "Car length probability must sum up to 100%";
    } else if( driverProfileProbSum != 100 ) {
        msg = "Driver profile probability must sum up to 100%";
    }

    return msg;
}

void MainTrafficSimulator::on_btnSTDiagram_clicked()
{
    static Configuration *config = nullptr;
    if (config != nullptr) {
        delete config;
    }
    config = new Configuration;

    QString msg = checkCorrectProbabilitesAndCounts();

    if (msg.size() != 0) {
        ui->statusbar->setStyleSheet(" QStatusBar{ color:#e0102D; font-weight:bold; } ");
        ui->statusbar->showMessage(msg, 5000);

        return;
    }

    bool wasBtnSaveFDiagramChanged = false;
    bool wasCbApproximateChanged = false;

    ui->btnFDiagram->setEnabled(false);
    ui->btnFDiagramConfig->setEnabled(false);
    if( ui->btnSaveFDiagram->isEnabled() ) {
        ui->btnSaveFDiagram->setEnabled(false);
        wasBtnSaveFDiagramChanged = true;
    };

    if( ui->cbApproximate->isEnabled() ) {
        ui->cbApproximate->setEnabled(false);
        wasCbApproximateChanged = true;
    };

    ui->btnSTDiagram->setEnabled(false);
    ui->btnSTDiagramConfig->setEnabled(false);
    ui->btnSaveSTDiagram->setEnabled(false);

    showDensitiesQt = false;

    ui->statusbar->setStyleSheet(" QStatusBar{ color:#000000; font-weight:bold; } ");
    ui->statusbar->showMessage("Simulation is currently running...");

    QTimer *timer = new QTimer(this);
    QElapsedTimer *elapsedTimer = new QElapsedTimer;
    elapsedTimer->start();

    connect(timer, &QTimer::timeout, this, [=] {
        int diffTime = elapsedTimer->elapsed();
        QString strTime = QString("%1:%2.%3").arg( (diffTime / 1000) / 60, 2, 10, QChar('0')).arg( (diffTime / 1000) % 60, 2, 10, QChar('0')).arg(diffTime % 1000, 3, 10, QChar('0'));

        ui->lcdNumber->display(strTime);
    } );
    timer->start(1);

    futureWatcher = new QFutureWatcher<void>();
    futureWatcher->setFuture( QtConcurrent::run([=]{ GenerateRoad(*ui, *config); }) );

    QObject::connect( futureWatcher, &QFutureWatcher<void>::finished, this, [=] {
        disconnect(timer, &QTimer::timeout, 0, 0);

        paintSTDiagram(config->data);
        ui->tabWidget->setCurrentIndex(2);

        ui->btnFDiagram->setEnabled(true);
        ui->btnFDiagramConfig->setEnabled(true);
        if( !ui->btnSaveFDiagram->isEnabled() && wasBtnSaveFDiagramChanged ) {
            ui->btnSaveFDiagram->setEnabled(true);
        };

        if( !ui->cbApproximate->isEnabled() && wasCbApproximateChanged ) {
            ui->cbApproximate->setEnabled(true);
        };

        ui->btnSTDiagram->setEnabled(true);
        ui->btnSTDiagramConfig->setEnabled(true);
        ui->btnSaveSTDiagram->setEnabled(true);

        int diffTime = elapsedTimer->elapsed();
        ui->statusbar->showMessage("Simulation completed!", 10000);
    } );
}

void MainTrafficSimulator::on_btnSTDiagramConfig_clicked()
{
    static Configuration *config = nullptr;
    if (config != nullptr) {
        delete config;
    }
    config = new Configuration;

    QString msg = checkCorrectProbabilitesAndCounts();

    if (msg.size() != 0) {
        ui->statusbar->setStyleSheet(" QStatusBar{ color:#e0102D; font-weight:bold; } ");
        ui->statusbar->showMessage(msg, 5000);

        return;
    }

    bool wasBtnSaveFDiagramChanged = false;
    bool wasCbApproximateChanged = false;

    ui->btnFDiagram->setEnabled(false);
    ui->btnFDiagramConfig->setEnabled(false);
    if( ui->btnSaveFDiagram->isEnabled() ) {
        ui->btnSaveFDiagram->setEnabled(false);
        wasBtnSaveFDiagramChanged = true;
    };

    if( ui->cbApproximate->isEnabled() ) {
        ui->cbApproximate->setEnabled(false);
        wasCbApproximateChanged = true;
    };

    ui->btnSTDiagram->setEnabled(false);
    ui->btnSTDiagramConfig->setEnabled(false);
    ui->btnSaveSTDiagram->setEnabled(false);

    showDensitiesQt = false;

    ui->statusbar->setStyleSheet(" QStatusBar{ color:#000000; font-weight:bold; } ");
    ui->statusbar->showMessage("Simulation is currently running...");

    QTimer *timer = new QTimer(this);
    QElapsedTimer *elapsedTimer = new QElapsedTimer;
    elapsedTimer->start();

    connect(timer, &QTimer::timeout, this, [=] {
        int diffTime = elapsedTimer->elapsed();
        QString strTime = QString("%1:%2.%3").arg( (diffTime / 1000) / 60, 2, 10, QChar('0')).arg( (diffTime / 1000) % 60, 2, 10, QChar('0')).arg(diffTime % 1000, 3, 10, QChar('0'));

        ui->lcdNumber->display(strTime);
    } );
    timer->start(1);


    futureWatcher = new QFutureWatcher<void>();
    futureWatcher->setFuture( QtConcurrent::run([=]{ GenerateRoad(*ui, *config); }) );

    QObject::connect( futureWatcher, &QFutureWatcher<void>::finished, this, [=]{
        disconnect(timer, &QTimer::timeout, 0, 0);

        paintSTDiagram(config->data);
        ui->tabWidget->setCurrentIndex(2);

        ui->btnFDiagram->setEnabled(true);
        ui->btnFDiagramConfig->setEnabled(true);
        if( !ui->btnSaveFDiagram->isEnabled() && wasBtnSaveFDiagramChanged ) {
            ui->btnSaveFDiagram->setEnabled(true);
        };

        if( !ui->cbApproximate->isEnabled() && wasCbApproximateChanged ) {
            ui->cbApproximate->setEnabled(true);
        };

        ui->btnSTDiagram->setEnabled(true);
        ui->btnSTDiagramConfig->setEnabled(true);
        ui->btnSaveSTDiagram->setEnabled(true);

        ui->statusbar->showMessage("Simulation completed!", 10000);
    } );
}

string MainTrafficSimulator::GetCurrentDate() {
    time_t now = time(NULL);
    tm *gmtm = localtime(&now);
    char timeStr[32 + 1] = { 0 };

    sprintf_s(timeStr, "%04d_%02d_%02d__%02d_%02d_%02d", 1900 + gmtm->tm_year, gmtm->tm_mon + 1, gmtm->tm_mday, gmtm->tm_hour, gmtm->tm_min, gmtm->tm_sec);

    return string(timeStr);
}

void MainTrafficSimulator::on_btnSaveSTDiagram_clicked()
{
    ui->btnSTDiagram->hide();
    ui->btnSaveSTDiagram->hide();

    string dirName = "./Space_Time_Diagram_Screenshots";
    QDir().mkdir(dirName.c_str());

    QString currentDate = "";
    currentDate.append(dirName);
    currentDate.append("/");
    currentDate.append("Space_Time_Diagram__");
    currentDate.append(QString::fromStdString(GetCurrentDate()));
    currentDate.append(".png");

    ui->tabSTDiagram->grab().save(currentDate);

    ui->btnSTDiagram->show();
    ui->btnSaveSTDiagram->show();

    ui->btnSaveSTDiagram->setDisabled(false);

    ui->statusbar->setStyleSheet(" QStatusBar{ color:#000000; font-weight:bold; } ");
    QString msg = "Diagram was saved successfully! Loctaion: ";
    msg.append( QDir::currentPath() );
    msg.append( currentDate.remove(0, 1) );

    ui->statusbar->showMessage(msg, 10000);
}

void MainTrafficSimulator::on_btnFDiagram_clicked()
{
    static Configuration *config = nullptr;
    if (config != nullptr) {
        delete config;
    }
    config = new Configuration;

    QString msg = checkCorrectProbabilitesAndCounts();

    if (msg.size() != 0) {
        ui->statusbar->setStyleSheet(" QStatusBar{ color:#e0102D; font-weight:bold; } ");
        ui->statusbar->showMessage(msg, 5000);

        return;
    }

    bool wasBtnSaveSTiagramChanged = false;

    ui->btnFDiagram->setEnabled(false);
    ui->btnFDiagramConfig->setEnabled(false);
    ui->btnSaveFDiagram->setEnabled(false);

    ui->cbApproximate->setEnabled(false);

    ui->btnSTDiagram->setEnabled(false);
    ui->btnSTDiagramConfig->setEnabled(false);
    if( ui->btnSaveSTDiagram->isEnabled() ) {
        ui->btnSaveSTDiagram->setEnabled(false);
        wasBtnSaveSTiagramChanged = true;
    };

    showDensitiesQt = true;

    ui->statusbar->setStyleSheet(" QStatusBar{ color:#000000; font-weight:bold; } ");
    ui->statusbar->showMessage("Simulation is currently running...");

    QTimer *timer = new QTimer(this);
    QElapsedTimer *elapsedTimer = new QElapsedTimer;
    elapsedTimer->start();

    connect(timer, &QTimer::timeout, this, [=] {
        int diffTime = elapsedTimer->elapsed();
        QString strTime = QString("%1:%2.%3").arg( (diffTime / 1000) / 60, 2, 10, QChar('0')).arg( (diffTime / 1000) % 60, 2, 10, QChar('0')).arg(diffTime % 1000, 3, 10, QChar('0'));

        ui->lcdNumber->display(strTime);
    } );
    timer->start(1);

    futureWatcher = new QFutureWatcher<void>();
    futureWatcher->setFuture( QtConcurrent::run([=]{ GenerateRoad(*ui, *config); }) );

    QObject::connect( futureWatcher, &QFutureWatcher<void>::finished, this, [=]{
        disconnect(timer, &QTimer::timeout, 0, 0);

        updateQtDiagramConfigData(qF.cfg, *config);
        paintFDiagram();
        dbm.saveDb(qF);
        ui->tabWidget->setCurrentIndex(3);

        ui->btnFDiagram->setEnabled(true);
        ui->btnFDiagramConfig->setEnabled(true);
        ui->btnSaveFDiagram->setEnabled(true);

        ui->cbApproximate->setEnabled(true);

        ui->btnSTDiagram->setEnabled(true);
        ui->btnSTDiagramConfig->setEnabled(true);
        if( !ui->btnSaveSTDiagram->isEnabled() && wasBtnSaveSTiagramChanged ) {
            ui->btnSaveSTDiagram->setEnabled(true);
        };

        ui->statusbar->showMessage("Simulation completed and saved to archive!", 10000);
    } );
}

void MainTrafficSimulator::on_btnFDiagramConfig_clicked()
{
    static Configuration *config = nullptr;
    if (config != nullptr) {
        delete config;
    }
    config = new Configuration;

    QString msg = checkCorrectProbabilitesAndCounts();

    if (msg.size() != 0) {
        ui->statusbar->setStyleSheet(" QStatusBar{ color:#e0102D; font-weight:bold; } ");
        ui->statusbar->showMessage(msg, 5000);

        return;
    }

    bool wasBtnSaveSTiagramChanged = false;

    ui->btnFDiagram->setEnabled(false);
    ui->btnFDiagramConfig->setEnabled(false);
    ui->btnSaveFDiagram->setEnabled(false);

    ui->cbApproximate->setEnabled(false);

    ui->btnSTDiagram->setEnabled(false);
    ui->btnSTDiagramConfig->setEnabled(false);
    if( ui->btnSaveSTDiagram->isEnabled() ) {
        ui->btnSaveSTDiagram->setEnabled(false);
        wasBtnSaveSTiagramChanged = true;
    };

    showDensitiesQt = true;

    ui->statusbar->setStyleSheet(" QStatusBar{ color:#000000; font-weight:bold; } ");
    ui->statusbar->showMessage("Simulation is currently running...");

    QTimer *timer = new QTimer(this);
    QElapsedTimer *elapsedTimer = new QElapsedTimer;
    elapsedTimer->start();

    connect(timer, &QTimer::timeout, this, [=] {
        int diffTime = elapsedTimer->elapsed();
        QString strTime = QString("%1:%2.%3").arg( (diffTime / 1000) / 60, 2, 10, QChar('0')).arg( (diffTime / 1000) % 60, 2, 10, QChar('0')).arg(diffTime % 1000, 3, 10, QChar('0'));

        ui->lcdNumber->display(strTime);
    } );
    timer->start(1);

    futureWatcher = new QFutureWatcher<void>();
    futureWatcher->setFuture( QtConcurrent::run([=]{ GenerateRoad(*ui, *config); }) );

    QObject::connect( futureWatcher, &QFutureWatcher<void>::finished, this, [=]{
        disconnect(timer, &QTimer::timeout, 0, 0);

        updateQtDiagramConfigData(qF.cfg, *config);
        paintFDiagram();
        dbm.saveDb(qF);

        ui->tabWidget->setCurrentIndex(3);

        ui->btnFDiagram->setEnabled(true);
        ui->btnFDiagramConfig->setEnabled(true);
        ui->btnSaveFDiagram->setEnabled(true);

        ui->cbApproximate->setEnabled(true);

        ui->btnSTDiagram->setEnabled(true);
        ui->btnSTDiagramConfig->setEnabled(true);
        if( !ui->btnSaveSTDiagram->isEnabled() && wasBtnSaveSTiagramChanged ) {
            ui->btnSaveSTDiagram->setEnabled(true);
        };

        ui->statusbar->showMessage("Simulation completed and saved to archive!", 10000);
    } );
}

void MainTrafficSimulator::on_btnSaveFDiagram_clicked()
{
    ui->btnFDiagram->hide();
    ui->btnSaveFDiagram->hide();
    ui->cbApproximate->hide();

    string dirName = "./Fundamental_Diagram_Screenshots";
    QDir().mkdir(dirName.c_str());

    QString currentDate = "";
    currentDate.append(dirName);
    currentDate.append("/");
    currentDate.append("Fundamental_Diagram__");
    currentDate.append(QString::fromStdString(GetCurrentDate()));
    currentDate.append(".png");

    ui->tabFDiagram->grab().save(currentDate);

    ui->btnFDiagram->show();
    ui->btnSaveFDiagram->show();
    ui->cbApproximate->show();

    ui->statusbar->setStyleSheet(" QStatusBar{ color:#000000; font-weight:bold; } ");
    QString msg = "Diagram was saved successfully! Loctaion: ";
    msg.append( QDir::currentPath() );
    msg.append( currentDate.remove(0, 1) );

    ui->statusbar->showMessage(msg, 10000);
}

void MainTrafficSimulator::paintSTDiagram(string data) {
    QString qData = QString::fromStdString(data);
    QStringList qSplit = qData.split(";");
    qSplit.pop_front();

    int cyclesNum = qSplit.size() / 2;
    int segmentsNum = qSplit[1].size();

    const int SERIES_HEADS = 0;
    const int SERIES_TAILS = 1;
    const int SERIES_TAILS_CHANGE_LANE = 2;
    const int MAX_SERIES = 3;

    QChart static chart[MAX_ROAD_LANES];
    QScatterSeries static *series[MAX_ROAD_LANES][MAX_SERIES];

    int y;

    QStringList laneNames = { "Left Lane", "Right Lane" };

    for (int i = 0; i < MAX_ROAD_LANES; i++) {
        for (int j = 0; j < MAX_SERIES; j++) {
            series[i][j] = new QScatterSeries;

            switch(j) {
            case SERIES_HEADS:
                    series[i][j]->setMarkerSize(2);
                    series[i][j]->setMarkerShape( QScatterSeries::MarkerShapeCircle );
                    series[i][j]->setBorderColor( QColor::fromString("#1f77b4") );
                    series[i][j]->setColor( QColor::fromString("#1f77b4") );

                    break;
            case SERIES_TAILS:
                    series[i][j]->setMarkerSize(1);
                    series[i][j]->setMarkerShape( QScatterSeries::MarkerShapeCircle );
                    series[i][j]->setBorderColor( QColor::fromString("#1f77b4") ) ;
                    series[i][j]->setColor( QColor::fromString("#1f77b4") ) ;

                    break;
            case SERIES_TAILS_CHANGE_LANE:
                    series[i][j]->setMarkerSize(1);
                    series[i][j]->setMarkerShape( QScatterSeries::MarkerShapeCircle );
                    series[i][j]->setBorderColor( QColor::fromString("#1f77b4") ) ;
                    series[i][j]->setColor( QColor::fromString("#1f77b4") ) ;

                    break;
            }
        }
    }

    y = cyclesNum;

    for (int i = 0; i < qSplit.size(); i += 2) {
        for (int l = 0; l < MAX_ROAD_LANES; l++) {
            for (int s = 0; s < segmentsNum; s++) {
                switch (qSplit[i + l][s].toLatin1()) {
                case '.':
                        break;
                case '>':
                        if ( series[l][SERIES_TAILS] != nullptr ) {
                            series[l][SERIES_TAILS]->append(s, y);
                        }

                        break;
                case '}':
                        if ( series[l][SERIES_TAILS_CHANGE_LANE] != nullptr ) {
                            series[l][SERIES_TAILS_CHANGE_LANE]->append(s, y);
                        }

                        break;
                default:
                        if ( series[l][SERIES_HEADS] != nullptr ) {
                            series[l][SERIES_HEADS]->append(s, y);
                        }

                        break;
                }
            }
        }

        y--;
    }

    ui->qcvSTDiagramLeft->setChart(&chart[LEFT_LANE]);
    ui->qcvSTDiagramRight->setChart(&chart[RIGHT_LANE]);

    ui->qcvSTDiagramLeft->chart()->removeAllSeries();
    ui->qcvSTDiagramRight->chart()->removeAllSeries();

    for (int i = 0; i < MAX_ROAD_LANES; i++) {
        for (int j = 0; j < MAX_SERIES; j++) {
            switch(j) {
            case SERIES_HEADS:
                    series[i][j]->setMarkerSize(2);
                    series[i][j]->setMarkerShape( QScatterSeries::MarkerShapeCircle );
                    series[i][j]->setBorderColor( QColor::fromString("#1f77b4") );
                    series[i][j]->setColor( QColor::fromString("#1f77b4") );

                    break;
            case SERIES_TAILS:
                    series[i][j]->setMarkerSize(1);
                    series[i][j]->setMarkerShape( QScatterSeries::MarkerShapeCircle );
                    series[i][j]->setBorderColor( QColor::fromString("#7f7f7f") ) ;
                    series[i][j]->setColor( QColor::fromString("#7f7f7f") ) ;

                    break;
            case SERIES_TAILS_CHANGE_LANE:
                    series[i][j]->setMarkerSize(1);
                    series[i][j]->setMarkerShape( QScatterSeries::MarkerShapeCircle );
                    series[i][j]->setBorderColor( QColor::fromString("#d62728") ) ;
                    series[i][j]->setColor( QColor::fromString("#d62728") ) ;

                    break;
            }

            chart[i].addSeries(series[i][j]);
        }
    }

    for (int i = 0; i < MAX_ROAD_LANES; i++) {
        chart[i].legend()->hide();

        chart[i].setTitle(laneNames[i]);
        chart[i].createDefaultAxes();

        QValueAxis *axisX = qobject_cast<QValueAxis*>(chart[i].axes(Qt::Horizontal).first());
        axisX->setRange(0, segmentsNum);
        axisX->setRange(0, segmentsNum);
        axisX->setTitleText("Road segments");
        axisX->setLabelFormat("%.0d");
        axisX->setTickType(QValueAxis::TicksDynamic);
        axisX->setTickAnchor(0.0);
        axisX->setTickInterval( segmentsNum * 0.1 );
        axisX->setGridLineVisible(false);

        QValueAxis *axisY = qobject_cast<QValueAxis*>(chart[i].axes(Qt::Vertical).first());
        axisY->setRange(0, cyclesNum);
        axisY->setTitleText("Cycles");
        axisY->setLabelFormat("%.0d");
        axisY->setTickType(QValueAxis::TicksDynamic);
        axisY->setTickAnchor(0.0);
        axisY->setTickInterval( cyclesNum * 0.1 );
        axisY->setGridLineVisible(false);
    }
}

void MainTrafficSimulator::paintFDiagram() {
    QString qData = qF.cfg.data;
    QStringList qSplit = qData.split(";");

    std::vector<double> densityPointsX;
    std::vector<double> densityPointsY[MAX_ROAD_LANES + 1][MAX_CARS_SET];
    std::vector<double> densityPointsYApprox[MAX_ROAD_LANES + 1][MAX_CARS_SET];

    float densitiesSize = ( qSplit.size() - 1 ) / 2;
    float densityInterval = 100.0 / float( densitiesSize );
    float currentDensity = 0.0;

    float carsFlowsSum[MAX_CARS_SET] = {0.0};
    float maxY = 0.0;

    int colorCnt;

    for (int l = 0; l < MAX_ROAD_LANES + 1; l++) {
        colorCnt = 0;
        for (int j = 1; j < MAX_CARS_SET; j++) {
            qF.scatterSeries[l][j] = new QScatterSeries;
            qF.splineSeries[l][j] = new QSplineSeries;
            qF.splineSeriesApprox[l][j] = new QSplineSeries;

            densityPointsY[l][j].clear();

            QPen penSpline = qF.splineSeries[l][j]->pen();
            penSpline.setWidth(2);
            qF.splineSeries[l][j]->setPen(penSpline);

            QPen penApprox = qF.splineSeriesApprox[l][j]->pen();
            penApprox.setWidth(2);
            qF.splineSeriesApprox[l][j]->setPen(penApprox);

            if ( densitiesSize <= 10 ) {
                qF.scatterSeries[l][j]->setMarkerSize(7);
            } else if ( densitiesSize <= 100 ) {
                qF.scatterSeries[l][j]->setMarkerSize(5);
            } else {
                qF.scatterSeries[l][j]->setMarkerSize(3);
            }

            if( qSplit[0][j] == '1' ) {
                densityPointsY[l][j].push_back(0);

                 QColor qColor = qF.carsColors[colorCnt];

                 qF.scatterSeries[l][j]->setBorderColor( qColor );
                 qF.scatterSeries[l][j]->setColor( qColor );

                 qF.splineSeries[l][j]->setColor( qColor );
                 qF.splineSeriesApprox[l][j]->setColor( qColor );

                colorCnt++;
            }
        }
    }

    densityPointsX.push_back(0);
    for (int i = 2; i < qSplit.size(); i += 2) {
        memset(carsFlowsSum, 0, sizeof(carsFlowsSum));

        currentDensity += densityInterval;
        densityPointsX.push_back( currentDensity );

        for (int l = 0; l < MAX_ROAD_LANES; l++) {
            QString qs = qSplit[i + l];
            QStringList strCars = qs.split(",");

            for (int j = 1; j < MAX_CARS_SET; j++) {
                if( strCars[j] != "" ) {
                    int y = strCars[j].toFloat();

                    if ( qSplit[0][j] == '1' ) {
                        densityPointsY[l][j].push_back(y);

                        carsFlowsSum[j] += y;
                    }
                }
            }
        }

        for (int j = 1; j < MAX_CARS_SET; j++) {
            if ( qSplit[0][j] == '1' ) {
                densityPointsY[MAX_ROAD_LANES][j].push_back(carsFlowsSum[j]);
            }

            if( carsFlowsSum[j] > maxY ) {
                maxY = carsFlowsSum[j];
            }
        }
    }

    ui->qcvFDiagramLeft->setChart(&qF.chart[LEFT_LANE]);
    ui->qcvFDiagramRight->setChart(&qF.chart[RIGHT_LANE]);
    ui->qcvFDiagramSum->setChart(&qF.chart[QtDiagramF::SUM_LANES]);

    ui->qcvFDiagramLeft->chart()->removeAllSeries();
    ui->qcvFDiagramRight->chart()->removeAllSeries();
    ui->qcvFDiagramSum->chart()->removeAllSeries();

    std::vector<double> coeff[MAX_ROAD_LANES + 1][MAX_CARS_SET];
    for (int l = 0; l < MAX_ROAD_LANES + 1; l++) {
        for (int j = 1; j < MAX_CARS_SET; j++) {
            if( qSplit[0][j] == '1' ) {
                polyfit(densityPointsX, densityPointsY[l][j], coeff[l][j], 8);

                for(int p = 0; p < densityPointsX.size(); ++ p) {
                    double vfitted = 0;
                    for(int i = 0; i < coeff[l][j].size(); i++) {
                        vfitted += coeff[l][j][i] * (pow(densityPointsX.at(p), i));
                    }

                    densityPointsYApprox[l][j].push_back(vfitted);
                }
            }
        }
    }

    for (int l = 0; l < MAX_ROAD_LANES + 1; l++) {
        qF.chart[l].setTitle(qF.laneNames[l]);

        for (int j = 1; j < MAX_CARS_SET; j++) {
            if( qSplit[0][j] == '1' ) {
                qF.chart[l].addSeries(qF.scatterSeries[l][j]);
                qF.chart[l].addSeries(qF.splineSeries[l][j]);
                qF.chart[l].addSeries(qF.splineSeriesApprox[l][j]);

                qF.chart[l].legend()->setAlignment(Qt::AlignBottom);
                qF.chart[l].legend()->setShowToolTips(true);

                qF.chart[l].legend()->markers(qF.scatterSeries[l][j])[0]->setVisible(false);
                qF.chart[l].legend()->markers(qF.splineSeries[l][j])[0]->setVisible(false);
                qF.chart[l].legend()->markers(qF.splineSeriesApprox[l][j])[0]->setVisible(false);

                QString labelStr;
                labelStr.append("Car length: ");
                labelStr.append( QString::number(j) );

                qF.chart[l].legend()->markers(qF.scatterSeries[l][j])[0]->setVisible(true);
                qF.chart[l].legend()->markers(qF.scatterSeries[l][j])[0]->setLabel(labelStr);
            }

            for (int d = 0; d < densityPointsYApprox[l][j].size() && densityPointsYApprox[l][j].size() != 0; d++) {
                qF.scatterSeries[l][j]->append(densityPointsX[d], densityPointsY[l][j][d]);
                qF.splineSeries[l][j]->append(densityPointsX[d], densityPointsY[l][j][d]);
                qF.splineSeriesApprox[l][j]->append(densityPointsX[d], densityPointsYApprox[l][j][d]);

                qF.splineSeries[l][j]->setVisible(false);
            }
        }

        qF.chart[l].createDefaultAxes();

        QValueAxis *axisX = qobject_cast<QValueAxis*>(qF.chart[l].axes(Qt::Horizontal).first());
        axisX->setRange(0, 100);
        axisX->setTitleText("Density [%]");
        axisX->setLabelFormat("%.0d");
        axisX->setTickType(QValueAxis::TicksDynamic);
        axisX->setTickAnchor(0.0);
        axisX->setTickInterval(10);

        QValueAxis *axisY = qobject_cast<QValueAxis*>(qF.chart[l].axes(Qt::Vertical).first());
        axisY->setRange(0, maxY * 1.1);
        axisY->setTitleText("Flow");
        axisY->setTickType(QValueAxis::TicksDynamic);
        axisY->setTickAnchor(0.0);
        axisY->setTickInterval(maxY * 0.1);
    }

    ui->cbLegend->setEnabled(true);
    ui->cbFont->setEnabled(true);
    ui->sbFontSize->setEnabled(true);
    ui->cbTheme->setEnabled(true);

    ui->cbApproximate->setCheckState(Qt::Checked);

    updateDiagramStyleFromSnapshot();

    changeLegend();
    changeFont();
    changeTheme();
}

void MainTrafficSimulator::on_cbApproximate_stateChanged(int arg1)
{
    QString qData = qF.cfg.data;
    QStringList qSplit = qData.split(";");

    switch( ui->cbApproximate->checkState() ) {
    case Qt::Checked:
        for (int l = 0; l < MAX_ROAD_LANES + 1; l++) {
            for (int j = 1; j < MAX_CARS_SET; j++) {
                this->qF.splineSeries[l][j]->setVisible(false);
                this->qF.splineSeriesApprox[l][j]->setVisible(true);

                if( qSplit[0][j] == '1' ) {
                    qF.chart[l].legend()->markers(qF.splineSeriesApprox[l][j])[0]->setVisible(false);
                }
            }
        }

        break;
    case Qt::PartiallyChecked:
        for (int l = 0; l < MAX_ROAD_LANES + 1; l++) {
            for (int j = 1; j < MAX_CARS_SET; j++) {
                this->qF.splineSeries[l][j]->setVisible(true);
                this->qF.splineSeriesApprox[l][j]->setVisible(false);

                if( qSplit[0][j] == '1' ) {
                    qF.chart[l].legend()->markers(qF.splineSeries[l][j])[0]->setVisible(false);
                }
            }
        }

        break;
    case Qt::Unchecked:
        for (int l = 0; l < MAX_ROAD_LANES + 1; l++) {
            for (int j = 1; j < MAX_CARS_SET; j++) {
                this->qF.splineSeries[l][j]->setVisible(false);
                this->qF.splineSeriesApprox[l][j]->setVisible(false);
            }
        }

        break;
    };
}

void MainTrafficSimulator::changeLegend() {
    QList<QChart *> qChartsFDiagram;

    qChartsFDiagram.append(ui->qcvFDiagramLeft->chart());
    qChartsFDiagram.append(ui->qcvFDiagramRight->chart());
    qChartsFDiagram.append(ui->qcvFDiagramSum->chart());

    for(int i = 0; i < qChartsFDiagram.size(); i++) {
        Qt::Alignment alignment;
        switch(ui->cbLegend->currentIndex()) {
        case 0:
            alignment = Qt::AlignTop;
            qChartsFDiagram[i]->legend()->show();
            qChartsFDiagram[i]->legend()->setAlignment(alignment);
            break;
        case 1:
            alignment = Qt::AlignBottom;
            qChartsFDiagram[i]->legend()->show();
            qChartsFDiagram[i]->legend()->setAlignment(alignment);
            break;
        case 2:
            alignment = Qt::AlignLeft;
            qChartsFDiagram[i]->legend()->show();
            qChartsFDiagram[i]->legend()->setAlignment(alignment);
            break;
        case 3:
            alignment = Qt::AlignRight;
            qChartsFDiagram[i]->legend()->show();
            qChartsFDiagram[i]->legend()->setAlignment(alignment);
            break;
        case 4:
            qChartsFDiagram[i]->legend()->hide();
            break;
        }
    }
}

void MainTrafficSimulator::on_cbLegend_currentIndexChanged(int index)
{
    changeLegend();
}

void MainTrafficSimulator::changeFont() {
    QList<QChart *> qChartsFDiagram;

    qChartsFDiagram.append(ui->qcvFDiagramLeft->chart());
    qChartsFDiagram.append(ui->qcvFDiagramRight->chart());
    qChartsFDiagram.append(ui->qcvFDiagramSum->chart());

    for(int i = 0; i < qChartsFDiagram.size(); i++) {
        QFont font = ui->cbFont->currentFont();
        int fontSize = ui->sbFontSize->value();
        font.setPixelSize(fontSize);

        qChartsFDiagram[i]->setTitleFont(font);
        qChartsFDiagram[i]->legend()->setFont(font);

        QValueAxis *axisX = qobject_cast<QValueAxis*>(qChartsFDiagram[i]->axes(Qt::Horizontal).first());
        QValueAxis *axisY = qobject_cast<QValueAxis*>(qChartsFDiagram[i]->axes(Qt::Vertical).first());

        axisX->setLabelsFont(font);
        axisY->setLabelsFont(font);

        axisX->setTitleFont(font);
        axisY->setTitleFont(font);

        if( fontSize > 13 ) {
            axisX->setTickInterval(20);
        } else {
            axisX->setTickInterval(10);
        }
    }
}

void MainTrafficSimulator::on_cbFont_currentFontChanged(const QFont &f)
{
    changeFont();
}


void MainTrafficSimulator::on_sbFontSize_valueChanged(int arg1)
{
    changeFont();
}

void MainTrafficSimulator::changeTheme() {
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>( ui->cbTheme->itemData(ui->cbTheme->currentIndex()).toInt() );

    ui->qcvFDiagramLeft->chart()->setTheme(theme);
    ui->qcvFDiagramRight->chart()->setTheme(theme);
    ui->qcvFDiagramSum->chart()->setTheme(theme);

    QString qData = qF.cfg.data;
    QStringList qSplit = qData.split(";");

    int colorCnt = 0;
    for (int l = 0; l < MAX_ROAD_LANES + 1; l++) {
        colorCnt = 0;
        for (int j = 1; j < MAX_CARS_SET; j++) {
            if ( qSplit[0][j] == '1' ) {
                QColor qColor = qF.carsColors[colorCnt];

                qF.scatterSeries[l][j]->setBorderColor( qColor );
                qF.scatterSeries[l][j]->setColor( qColor );

                qF.splineSeries[l][j]->setColor( qColor );

                qF.splineSeriesApprox[l][j]->setColor( qColor );

                colorCnt++;
            }
        }
    }
}

void MainTrafficSimulator::on_cbTheme_currentIndexChanged(int index)
{
    changeTheme();
}

void polyfit(const std::vector<double> &t, const std::vector<double> &v, std::vector<double> &coeff, int order) {
    Eigen::MatrixXd T(t.size(), order + 1);
    Eigen::VectorXd V = Eigen::VectorXd::Map(&v.front(), v.size());
    Eigen::VectorXd result;

    assert(t.size() == v.size());
    assert(t.size() >= order + 1);

    for(size_t i = 0 ; i < t.size(); ++i) {
        for(size_t j = 0; j < order + 1; ++j) {
            T(i, j) = pow(t.at(i), j);
        }
    }

    result = T.householderQr().solve(V);
    coeff.resize(order+1);
    for (int k = 0; k < order+1; k++) {
        coeff[k] = result[k];
    }
}

void MainTrafficSimulator::closeEvent (QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "TrafficSimulator",
                                                               tr("Are you sure?\n"),
                                                               QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        updateSnapshotFromUi();
        dbm.updateSnapshot(snapshot);

        event->accept();
    }
}
