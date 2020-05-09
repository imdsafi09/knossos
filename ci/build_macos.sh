#!/bin/bash
set -euxo pipefail

# Fix Python headers
# https://bugs.python.org/review/10910/diff2/2561:8559/Include/pyport.h
cd ..
mkdir python2.7
time cp /usr/include/python2.7/* python2.7
ed -s python2.7/pyport.h << EOF
/#ifdef _PY_PORT_CTYPE_UTF8_ISSUE/
a
#ifndef __cplusplus
.
+16
a
#endif
.
w
EOF

# Download and install PythonQt
time curl -JLO https://github.com/knossos-project/knossos/releases/download/nightly-dev/macOS-PythonQt.zip
time unzip -d / macOS-PythonQt.zip

# Fix QuaZip include directory name
cd $TRAVIS_BUILD_DIR && cd ..
mkdir -p quazip/quazip5
QUAZIP_VERSION=`brew list --versions quazip | cut -d " " -f2`
time cp -R /usr/local/Cellar/quazip/${QUAZIP_VERSION}/include/quazip/* quazip/quazip5/

# Build KNOSSOS
mkdir knossos-build && cd knossos-build
time cmake -G Ninja ../knossos -DCMAKE_PREFIX_PATH=/usr/local/opt/qt -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DCMAKE_CXX_FLAGS=-isystem\ ${TRAVIS_BUILD_DIR}/../quazip
time ninja

# OS X housekeeping
mv knossos.app KNOSSOS.app
time /usr/local/opt/qt/bin/macdeployqt KNOSSOS.app
cp /usr/local/Cellar/quazip/${QUAZIP_VERSION}/lib/libquazip.1.0.0.dylib KNOSSOS.app/Contents/Frameworks/libquazip.1.dylib
install_name_tool KNOSSOS.app/Contents/Frameworks/libquazip.1.dylib -change /usr/local/opt/qt/lib/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore

# Deployment
time zip -r ${TRAVIS_BUILD_DIR}/macos.KNOSSOS.nightly.app.zip KNOSSOS.app
time zip -r ${TRAVIS_BUILD_DIR}/macos.${TRAVIS_BRANCH}-KNOSSOS.nightly.app.zip KNOSSOS.app