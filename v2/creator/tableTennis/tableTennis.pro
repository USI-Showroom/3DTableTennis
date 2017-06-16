#-------------------------------------------------
#
# Project created by QtCreator 2014-08-12T14:37:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tableTennis
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../src/kinect/FakePaddle.cpp \
    ../../src/kinect/GestureRecognizer.cpp \
    ../../src/kinect/Helpers.cpp \
    ../../src/kinect/KinectManager.cpp \
    ../../src/kinect/OpponentPaddle.cpp \
    ../../src/kinect/Paddle.cpp \
    ../../src/kinect/Player.cpp \
    ../../src/kinect/StatusLog.cpp \
    ../../src/mesh/reader/MtlReader.cpp \
    ../../src/mesh/reader/ObjReader.cpp \
    ../../src/mesh/Material.cpp \
    ../../src/mesh/Mesh.cpp \
    ../../src/mesh/Texture3D.cpp \
    ../../src/paddle/PaddleDrawer.cpp \
    ../../src/paddle/VirtualPaddle.cpp \
    ../../src/pingPong/Ball.cpp \
    ../../src/pingPong/BallLauncher.cpp \
    ../../src/pingPong/BallTrajectoryDrawer.cpp \
    ../../src/pingPong/PingPongMenu.cpp \
    ../../src/pingPong/Score.cpp \
    ../../src/pingPong/ScoreBoard.cpp \
    ../../src/pingPong/Table.cpp \
    ../../src/pingPong/TableTennisSystem.cpp \
    ../../src/qt/CCanvas.cpp \
    ../../src/qt/GLRender.cpp \
    ../../src/qt/SoundEngine.cpp \
    ../../src/shader/FileShader.cpp \
    ../../src/shader/Shader.cpp \
    ../../src/utils/Configuration.cpp \
    ../../src/utils/Defines.cpp \
    ../../src/utils/Timer.cpp \
    ../../src/main.cpp

HEADERS  += mainwindow.h \
    ../../src/kinect/FakePaddle.h \
    ../../src/kinect/GestureRecognizer.h \
    ../../src/kinect/Helpers.h \
    ../../src/kinect/KinectManager.h \
    ../../src/kinect/OpponentPaddle.h \
    ../../src/kinect/Paddle.h \
    ../../src/kinect/Player.h \
    ../../src/kinect/StatusLog.h \
    ../../src/mesh/reader/MtlReader.h \
    ../../src/mesh/reader/ObjReader.h \
    ../../src/mesh/Material.h \
    ../../src/mesh/Mesh.h \
    ../../src/mesh/Texture3D.h \
    ../../src/paddle/PaddleDrawer.h \
    ../../src/paddle/VirtualPaddle.h \
    ../../src/pingPong/Ball.h \
    ../../src/pingPong/BallLauncher.h \
    ../../src/pingPong/BallTrajectoryDrawer.h \
    ../../src/pingPong/PingPongMenu.h \
    ../../src/pingPong/Score.h \
    ../../src/pingPong/ScoreBoard.h \
    ../../src/pingPong/Table.h \
    ../../src/pingPong/TableTennisSystem.h \
    ../../src/qt/CCanvas.h \
    ../../src/qt/GLRender.h \
    ../../src/qt/SoundEngine.h \
    ../../src/shader/FileShader.h \
    ../../src/shader/glext.h \
    ../../src/shader/Shader.h \
    ../../src/utils/Base.h \
    ../../src/utils/ColorDef.h \
    ../../src/utils/Configuration.h \
    ../../src/utils/Defines.h \
    ../../src/utils/Point2.h \
    ../../src/utils/Point3.h \
    ../../src/utils/Timer.h

FORMS    += mainwindow.ui
