#include "main.h"

void Compute::run()
{
    QLabel* h0label = labels[0];
    QLabel* h1label = labels[1];
    QLabel* h2label = labels[2];
    QLabel* eulerLabel = labels[3];
    QLabel* statusLabel = labels[4];

    statusLabel->setText("Calculating...");
    confirmButton->setEnabled(false);
    t->calculate();

    statusLabel->setText("Drawing...");
    h0label->setText("H0: "+QString::number(t->get_homology()[0]));
    h1label->setText("H1: "+QString::number(t->get_homology()[1]));
    h2label->setText("H2: "+QString::number(t->get_homology()[2]));
    eulerLabel->setText("Euler: "+QString::number(t->calc_euler()));

    qInfo("Found: %d", int(t->get_triangles().size()));
    confirmButton->setEnabled(true);

    Q_EMIT drawTriangulation();
}

Main::Main(int argc, char **argv) : QApplication(argc, argv)
{
    setApplicationName("Surface Reconstruction");

    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);
#ifndef QT_NO_OPENGL

    graph = new QWidget;
    plot = new SurfaceGraph(graph);
    plot->makeCurrent();

    graph->setMinimumSize(700,500);
    plot->setMinimumSize(700,500);

    widget = new QWidget;
    hLayout = new QHBoxLayout(widget);
    vLayout = new QVBoxLayout();

    hLayout->addWidget(graph, 1);
    hLayout->addLayout(vLayout);
    vLayout->setAlignment(Qt::AlignTop);

    widget->setWindowTitle(QStringLiteral("Surface Reconstruction"));

    filesGroupBox = new QGroupBox(QStringLiteral("Dataset"));

    filesList = new QComboBox(widget);
    QObject::connect(filesList, &QComboBox::currentTextChanged,
                     [&] (const QString& choice) { t.set_in_file(("dataset/" + choice).toUtf8().constData()); });


    iter = new QDirIterator("dataset/", QStringList() << "*.obj", QDir::Files, QDirIterator::Subdirectories);
    while (iter->hasNext()) {
        filesList->addItem(iter->next().remove(0,8));
    }

    methodGroupBox = new QGroupBox(QStringLiteral("Method"));

    methodList = new QComboBox(widget);
    methodViet = "Vietoris-Rips"; methodList->addItem(methodViet);
    //methodCech = "Čech complex";  methodList->addItem(methodCech);
    methodAlph = "α-shapes";      methodList->addItem(methodAlph);

    QObject::connect(methodList, &QComboBox::currentTextChanged,
        [&] (const QString& choice) {
        if(choice == methodViet)      t.set_mode(Triangulation::rips);
        //else if(choice == methodCech) t.set_mode(Triangulation::cech);
        else                          t.set_mode(Triangulation::alpha_shapes);
    });
    t.set_mode(Triangulation::rips);

    selectionGroupBox = new QGroupBox(QStringLiteral("Delta parameter"));

    deltaSpinner = new QDoubleSpinBox(widget);
    deltaSpinner->setMinimum(0.0);
    deltaSpinner->setDecimals(4);
    deltaSpinner->setSingleStep(0.05);
    deltaSpinner->setValue(1.0);

    QObject::connect(deltaSpinner, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [&] (double val) { t.set_distance(val); });

    samplePercentGroupBox = new QGroupBox(QStringLiteral("Sample percent"));

    samplePercentSlider = new QSlider(Qt::Horizontal, widget);
    samplePercentSlider->setMinimum(0);
    samplePercentSlider->setMaximum(100);
    samplePercentSlider->setTickInterval(1);
    samplePercentSlider->setEnabled(true);
    samplePercentSlider->setSliderPosition(100);

    QObject::connect(samplePercentSlider, &QSlider::sliderMoved,
                     [&] (int val) { t.set_point_filtration((float)val / 100); });

    transparencyRBttn = new QPushButton(widget);
    transparencyRBttn->setText(QStringLiteral("Transparent"));
    transparencyRBttn->setCheckable(true);

    linesBttn = new QPushButton(widget);
    linesBttn->setText(QStringLiteral("Show Lines"));
    linesBttn->setCheckable(true);

    confirmButton = new QPushButton(widget);
    confirmButton->setText(QStringLiteral("Confirm"));

    homologyGroupBox = new QGroupBox(QStringLiteral("Homology"));
    h0label = new QLabel("H0: 0");
    h1label = new QLabel("H1: 0");
    h2label = new QLabel("H2: 0");
    eulerLabel = new QLabel("Euler: 0");
    statusLabel = new QLabel("Idle");

    vLayout->addWidget(filesGroupBox);
    vLayout->addWidget(filesList);
    vLayout->addWidget(methodGroupBox);
    vLayout->addWidget(methodList);
    vLayout->addWidget(selectionGroupBox);
    vLayout->addWidget(deltaSpinner);
    vLayout->addWidget(samplePercentGroupBox);
    vLayout->addWidget(samplePercentSlider);
    vLayout->addWidget(transparencyRBttn);
    vLayout->addWidget(linesBttn);
    vLayout->addWidget(confirmButton);
    vLayout->addWidget(homologyGroupBox);
    vLayout->addWidget(h0label);
    vLayout->addWidget(h1label);
    vLayout->addWidget(h2label);
    vLayout->addWidget(eulerLabel);
    vLayout->addWidget(statusLabel);

    QObject::connect(confirmButton, &QPushButton::clicked, [&] {
        std::vector<QLabel*> labels = { h0label, h1label, h2label, eulerLabel, statusLabel };
        Compute* thr = new Compute(&t, labels, confirmButton);

        QObject::connect(thr, &Compute::drawTriangulation, this, &Main::drawTriangulation);
        QObject::connect(thr, SIGNAL(finished()), thr, SLOT(deleteLater()));
        thr->start();
    });

    QObject::connect(transparencyRBttn, &QRadioButton::toggled,
                     [=]{plot->toggleTransparency(transparencyRBttn->isChecked());});

    QObject::connect(linesBttn, &QRadioButton::toggled,
                     [=]{plot->toggleLines(linesBttn->isChecked());});

    widget->show();
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
}

void Main::drawTriangulation()
{
    plot->redraw(t.get_triangles(), t.get_lines(), t.get_points());
    plot->toggleLines(linesBttn->isChecked());
    plot->toggleTransparency(transparencyRBttn->isChecked());

    statusLabel->setText("Idle");
    confirmButton->setEnabled(true);
}

int main(int argc, char **argv)
{
    Main main(argc, argv);
    return main.exec();
}

