# surface-reconstruction
First seminar for course Computational Topology in 2016

## Prebuild instructions:

### QtDataVisualisation

    git clone http://code.qt.io/cgit/qt/qtdatavis3d.git
    cd qtdatavis3d
    qmake
    make
    sudo make install

### Dionysus

    hg clone http://hg.mrzv.org/Dionysus/
    cd Dionysus
    hg up tip

    mkdir build
    cd build
    cmake ..
    make

TODO linking...
