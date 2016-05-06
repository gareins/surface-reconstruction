#include "surfacegraph.h"

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

#include "triangulation.h"

int main(int argc, char **argv) {

    if(1)
    {
        Triangulation t;
        t.set_in_file("dataset/sphere_1000.obj");
        t.set_mode(Triangulation::alpha_shapes);
        t.calculate();

        qDebug("Found %d triangles!", t.get_triangles().size());
        exit(0);
    }

    QApplication app(argc, argv);
    Q3DSurface *graph = new Q3DSurface();
    QWidget *container = QWidget::createWindowContainer(graph);

    if (!graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return -1;
    }

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);
    vLayout->setAlignment(Qt::AlignTop);

    widget->setWindowTitle(QStringLiteral("Surface Reconstruction"));

    //non bold labels: new QLabel(QStringLiteral("Column range"))
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

    QGroupBox *dimensionGroupBox = new QGroupBox(QStringLiteral("Dimensions"));

    QSlider *dimensionSlider = new QSlider(Qt::Horizontal, widget);
    dimensionSlider->setMinimum(0);
    dimensionSlider->setMaximum(2);
    dimensionSlider->setTickPosition(QSlider::TicksBelow);
    dimensionSlider->setTickInterval(1);
    dimensionSlider->setEnabled(true);
    dimensionSlider->setSliderPosition(2);

    QPushButton *confirmButton = new QPushButton(widget);
    confirmButton->setText(QStringLiteral("Confirm"));

    vLayout->addWidget(methodGroupBox);
    vLayout->addWidget(methodList);
    vLayout->addWidget(selectionGroupBox);
    vLayout->addWidget(deltaSpinner);
    vLayout->addWidget(samplePercentGroupBox);
    vLayout->addWidget(samplePercentSlider);
    vLayout->addWidget(dimensionGroupBox);
    vLayout->addWidget(dimensionSlider);
    vLayout->addWidget(confirmButton);

    widget->show();

    SurfaceGraph *plot = new SurfaceGraph(graph);

    // TODO: pass params to update
    QObject::connect(confirmButton, &QPushButton::clicked,
                     [=]{plot->update();});

    return app.exec();
}
