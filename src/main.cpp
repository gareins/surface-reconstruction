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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QScreen>
#include <QDirIterator>
#include <QRadioButton>
#include <iostream>

#include "triangulation.h"

int main(int argc, char **argv) {

    Triangulation t;
    t.set_in_file("dataset/sphere_1000.obj");
    t.set_mode(Triangulation::alpha_shapes);
    t.calculate();
    TriangleList dummyTrig = t.get_triangles();

    qDebug("Found %d triangles!", (int)t.get_triangles().size());
    //exit(0);

    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("Surface Reconstruction");
    //app.setApplicationVersion("0.1");
#ifndef QT_NO_OPENGL

    QWidget *graph = new QWidget;
    SurfaceGraph *plot = new SurfaceGraph(graph);
    graph->setMinimumSize(700,500);
    plot->setMinimumSize(700,500);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();

    hLayout->addWidget(graph, 1);
    hLayout->addLayout(vLayout);
    vLayout->setAlignment(Qt::AlignTop);

    widget->setWindowTitle(QStringLiteral("Surface Reconstruction"));

    QGroupBox *filesGroupBox = new QGroupBox(QStringLiteral("Dataset"));

    QComboBox *filesList = new QComboBox(widget);

    QDirIterator iter("dataset/", QStringList() << "*.obj", QDir::Files, QDirIterator::Subdirectories);
    while (iter.hasNext()) {
        QString file = iter.next();
        filesList->addItem(file.remove(0,8));
    }

    QGroupBox *methodGroupBox = new QGroupBox(QStringLiteral("Method"));

    QComboBox *methodList = new QComboBox(widget);
    methodList->addItem(QStringLiteral("Čech complex"));
    methodList->addItem(QStringLiteral("Vietoris-Rips"));
    methodList->addItem(QStringLiteral("α-shapes"));

    QGroupBox *selectionGroupBox = new QGroupBox(QStringLiteral("Delta parameter"));

    // TODO: tweak values
    QDoubleSpinBox *deltaSpinner = new QDoubleSpinBox(widget);
    deltaSpinner->setMinimum(0.0);
    deltaSpinner->setMaximum(5.0);
    deltaSpinner->setSingleStep(0.5);
    deltaSpinner->setValue(1.0);

    QGroupBox *samplePercentGroupBox = new QGroupBox(QStringLiteral("Sample percent"));

    QSlider *samplePercentSlider = new QSlider(Qt::Horizontal, widget);
    samplePercentSlider->setMinimum(0);
    samplePercentSlider->setMaximum(100);
    samplePercentSlider->setTickInterval(1);
    samplePercentSlider->setEnabled(true);
    samplePercentSlider->setSliderPosition(100);

    /*
    QGroupBox *dimensionGroupBox = new QGroupBox(QStringLiteral("Dimensions"));

    QSlider *dimensionSlider = new QSlider(Qt::Horizontal, widget);
    dimensionSlider->setMinimum(0);
    dimensionSlider->setMaximum(2);
    dimensionSlider->setTickPosition(QSlider::TicksBelow);
    dimensionSlider->setTickInterval(1);
    dimensionSlider->setEnabled(true);
    dimensionSlider->setSliderPosition(2);
    */

    QRadioButton *transparencyRBttn = new QRadioButton(widget);
    transparencyRBttn->setText(QStringLiteral("Transparent"));

    QPushButton *confirmButton = new QPushButton(widget);
    confirmButton->setText(QStringLiteral("Confirm"));

    vLayout->addWidget(filesGroupBox);
    vLayout->addWidget(filesList);
    vLayout->addWidget(methodGroupBox);
    vLayout->addWidget(methodList);
    vLayout->addWidget(selectionGroupBox);
    vLayout->addWidget(deltaSpinner);
    vLayout->addWidget(samplePercentGroupBox);
    vLayout->addWidget(samplePercentSlider);
    //vLayout->addWidget(dimensionGroupBox);
    //vLayout->addWidget(dimensionSlider);
    vLayout->addWidget(transparencyRBttn);
    vLayout->addWidget(confirmButton);

    // TODO: this should initiate a recalc in triangulation with parameters from above widgets
    // and a reference to *plot instead of calling redraw. When the recalc is done,
    // triangulation should call redraw with a TriangleList as param
    /*
    std::cout << filesList->currentText().toUtf8().constData() << std::endl;
    std::cout << methodList->currentIndex() << std::endl;
    std::cout << deltaSpinner->value() << std::endl;
    std::cout << samplePercentSlider->value() << std::endl;
    std::cout << dimensionSlider->value() << std::endl;
    */

    QObject::connect(confirmButton, &QPushButton::clicked,
                     [=]{plot->redraw(dummyTrig);});

    QObject::connect(transparencyRBttn, &QRadioButton::toggled,
                     [=]{plot->toggleTransparency(transparencyRBttn->isChecked());});

    widget->show();
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
    return app.exec();
}
