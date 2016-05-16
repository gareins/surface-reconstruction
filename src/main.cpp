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
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("Surface Reconstruction");
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
    QObject::connect(filesList, &QComboBox::currentTextChanged,
                     [&] (const QString& choice) { t.set_in_file(("dataset/" + choice).toUtf8().constData()); });

    QDirIterator iter("dataset/", QStringList() << "*.obj", QDir::Files, QDirIterator::Subdirectories);
    while (iter.hasNext()) {
        QString file = iter.next();
        filesList->addItem(file.remove(0,8));
    }

    QGroupBox *methodGroupBox = new QGroupBox(QStringLiteral("Method"));

    QComboBox *methodList = new QComboBox(widget);
    QString methodViet = "Vietoris-Rips"; methodList->addItem(methodViet);
    //QString methodCech = "Čech complex";  methodList->addItem(methodCech);
    QString methodAlph = "α-shapes";      methodList->addItem(methodAlph);

    QObject::connect(methodList, &QComboBox::currentTextChanged,
        [&] (const QString& choice) {
        if(choice == methodViet)      t.set_mode(Triangulation::rips);
        //else if(choice == methodCech) t.set_mode(Triangulation::cech);
        else                          t.set_mode(Triangulation::alpha_shapes);
    });
    t.set_mode(Triangulation::rips);

    QGroupBox *selectionGroupBox = new QGroupBox(QStringLiteral("Delta parameter"));

    // TODO: tweak values
    QDoubleSpinBox *deltaSpinner = new QDoubleSpinBox(widget);
    deltaSpinner->setMinimum(0.0);
    deltaSpinner->setDecimals(4);
    //deltaSpinner->setMaximum(5.0);
    deltaSpinner->setSingleStep(0.05);
    deltaSpinner->setValue(1.0);

    QObject::connect(deltaSpinner, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [&] (double val) { t.set_distance(val); });

    QGroupBox *samplePercentGroupBox = new QGroupBox(QStringLiteral("Sample percent"));

    QSlider *samplePercentSlider = new QSlider(Qt::Horizontal, widget);
    samplePercentSlider->setMinimum(0);
    samplePercentSlider->setMaximum(100);
    samplePercentSlider->setTickInterval(1);
    samplePercentSlider->setEnabled(true);
    samplePercentSlider->setSliderPosition(100);

    QObject::connect(samplePercentSlider, &QSlider::sliderMoved,
                     [&] (int val) { t.set_point_filtration((float)val / 100); });

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

    QPushButton *transparencyRBttn = new QPushButton(widget);
    transparencyRBttn->setText(QStringLiteral("Transparent"));
    transparencyRBttn->setCheckable(true);

    QPushButton *linesBttn = new QPushButton(widget);
    linesBttn->setText(QStringLiteral("Show Lines"));
    linesBttn->setCheckable(true);

    QPushButton *confirmButton = new QPushButton(widget);
    confirmButton->setText(QStringLiteral("Confirm"));

    QGroupBox *homologyGroupBox = new QGroupBox(QStringLiteral("Homology"));
    QLabel *h0label = new QLabel("H0: 0");
    QLabel *h1label = new QLabel("H1: 0");
    QLabel *h2label = new QLabel("H2: 0");
    QLabel *eulerLabel = new QLabel("Euler: 0");

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
    vLayout->addWidget(linesBttn);
    vLayout->addWidget(confirmButton);
    vLayout->addWidget(homologyGroupBox);
    vLayout->addWidget(h0label);
    vLayout->addWidget(h1label);
    vLayout->addWidget(h2label);
    vLayout->addWidget(eulerLabel);

    /*
    std::cout << filesList->currentText().toUtf8().constData() << std::endl;
    std::cout << methodList->currentIndex() << std::endl;
    std::cout << deltaSpinner->value() << std::endl;
    std::cout << samplePercentSlider->value() << std::endl;
    std::cout << dimensionSlider->value() << std::endl;
    */

    QObject::connect(confirmButton, &QPushButton::clicked,
                     [&] {  t.calculate();
                            qInfo("Found: %d", int(t.get_triangles().size()));
                            plot->redraw(t.get_triangles(), t.get_lines(), t.get_points());
                            h0label->setText("H0: "+QString::number(t.get_homology()[0]));
                            h1label->setText("H1: "+QString::number(t.get_homology()[1]));
                            h2label->setText("H2: "+QString::number(t.get_homology()[2]));
                            eulerLabel->setText("Euler: "+QString::number(t.calc_euler()));
                            plot->toggleLines(linesBttn->isChecked());
                            plot->toggleTransparency(transparencyRBttn->isChecked());});

    QObject::connect(transparencyRBttn, &QRadioButton::toggled,
                     [=]{plot->toggleTransparency(transparencyRBttn->isChecked());});

    QObject::connect(linesBttn, &QRadioButton::toggled,
                     [=]{plot->toggleLines(linesBttn->isChecked());});

    widget->show();
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
    return app.exec();
}
