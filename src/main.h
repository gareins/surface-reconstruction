#ifndef MAIN_H
#define MAIN_H

#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>

#ifndef QT_NO_OPENGL
#include "surfacegraph.h"
#endif

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QScreen>
#include <QDirIterator>
#include <QRadioButton>
#include <QtWidgets/QPushButton>
#include <QThread>
#include <iostream>

#include "triangulation.h"


class Compute: public QThread
{
    Q_OBJECT
public:
    Compute(Triangulation* t, std::vector<QLabel*> labels, QPushButton* confirmButton)
        :t(t), labels(labels), confirmButton(confirmButton) {}

Q_SIGNALS:
    void drawTriangulation();

private:
    Triangulation* t;
    std::vector<QLabel*> labels;
    QPushButton* confirmButton;

    void run();

};

class Main: public QApplication
{
private:
    Triangulation t;
    QSurfaceFormat format;

    QWidget *graph;
    SurfaceGraph *plot;

    QWidget *widget;
    QHBoxLayout *hLayout;
    QVBoxLayout *vLayout;

    QGroupBox *filesGroupBox;
    QComboBox *filesList;
    QDirIterator* iter;

    QString methodViet;
    //QString methodCech;
    QString methodAlph;

    QGroupBox *methodGroupBox;
    QComboBox *methodList;

    QGroupBox *selectionGroupBox;
    QDoubleSpinBox *deltaSpinner;

    QGroupBox *samplePercentGroupBox;
    QSlider *samplePercentSlider;

    QPushButton *transparencyRBttn;
    QPushButton *linesBttn;

    QPushButton *confirmButton;
    QGroupBox *homologyGroupBox;

    QLabel *h0label;
    QLabel *h1label;
    QLabel *h2label;
    QLabel *eulerLabel;
    QLabel *statusLabel;

    Q_OBJECT
public:
    Main(int argc, char **argv);

public Q_SLOTS:
    void drawTriangulation();
};

int main(int argc, char **argv);

#endif // MAIN_H
