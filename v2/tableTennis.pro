TEMPLATE = app
TARGET = tableTennis

DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX NO_VECTORIAL_RENDER 
DEFINES += WIN32 CREATE_QGLVIEWER_DLL

QT += core gui xml opengl multimedia
CONFIG += qt debug console opengl warn_on #thread no_keywords
CONFIG -= flat


INCLUDEPATH += 	$(KINECTSDK20_DIR)inc \
        ./3rdParty/glew/include \
        ./3rdParty \
        ./src \
		./src/utils \
		./src/shader \
		./src/qt \
		./src/pingPong \
		./src/mesh \
		./src/mesh/reader \
		./src/paddle \
		./src/kinect \
		./QGLViewerSrc \
		./QGLViewerSrc/VRender
		

LIBS += -L$(KINECTSDK20_DIR)Lib/x86 \
	-L3rdParty/glew/lib/Release/Win32 \
   	-lKinect20 \
	-lglew32 

FORMS += ./src/qt/GLRender.ui \
         ./QGLViewerSrc/ImageInterface.ui 
	 
	

SOURCES += ./src/kinect/FakePaddle.cpp \
    ./src/kinect/GestureRecognizer.cpp \
    ./src/kinect/Helpers.cpp \
    ./src/kinect/KinectManager.cpp \
    ./src/kinect/OpponentPaddle.cpp \
    ./src/kinect/Paddle.cpp \
    ./src/kinect/Player.cpp \
    ./src/kinect/StatusLog.cpp \
    ./src/mesh/reader/MtlReader.cpp \
    ./src/mesh/reader/ObjReader.cpp \
    ./src/mesh/Material.cpp \
    ./src/mesh/Mesh.cpp \
    ./src/mesh/Texture3D.cpp \
    ./src/paddle/PaddleDrawer.cpp \
    ./src/paddle/VirtualPaddle.cpp \
    ./src/pingPong/Ball.cpp \
    ./src/pingPong/BallLauncher.cpp \
    ./src/pingPong/BallTrajectoryDrawer.cpp \
    ./src/pingPong/PingPongMenu.cpp \
    ./src/pingPong/Score.cpp \
    ./src/pingPong/ScoreBoard.cpp \
    ./src/pingPong/Table.cpp \
    ./src/pingPong/TableTennisSystem.cpp \
    ./src/qt/CCanvas.cpp \
    ./src/qt/GLRender.cpp \
    ./src/qt/SoundEngine.cpp \
    ./src/shader/FileShader.cpp \
    ./src/shader/Shader.cpp \
    ./src/utils/Configuration.cpp \
    ./src/utils/Defines.cpp \
    ./src/utils/Timer.cpp \
    ./src/main.cpp \
    ./QGLViewerSrc/qglviewer.cpp \
./QGLViewerSrc/camera.cpp \
./QGLViewerSrc/manipulatedFrame.cpp \
./QGLViewerSrc/manipulatedCameraFrame.cpp \
./QGLViewerSrc/frame.cpp \
./QGLViewerSrc/saveSnapshot.cpp \
./QGLViewerSrc/constraint.cpp \
./QGLViewerSrc/keyFrameInterpolator.cpp \
./QGLViewerSrc/mouseGrabber.cpp \
./QGLViewerSrc/quaternion.cpp \
./QGLViewerSrc/vec.cpp




HEADERS  += ./src/kinect/FakePaddle.h \
    ./src/kinect/GestureRecognizer.h \
    ./src/kinect/Helpers.h \
    ./src/kinect/KinectManager.h \
    ./src/kinect/OpponentPaddle.h \
    ./src/kinect/Paddle.h \
    ./src/kinect/Player.h \
    ./src/kinect/StatusLog.h \
    ./src/mesh/reader/MtlReader.h \
    ./src/mesh/reader/ObjReader.h \
    ./src/mesh/Material.h \
    ./src/mesh/Mesh.h \
    ./src/mesh/Texture3D.h \
    ./src/paddle/PaddleDrawer.h \
    ./src/paddle/VirtualPaddle.h \
    ./src/pingPong/Ball.h \
    ./src/pingPong/BallLauncher.h \
    ./src/pingPong/BallTrajectoryDrawer.h \
    ./src/pingPong/PingPongMenu.h \
    ./src/pingPong/Score.h \
    ./src/pingPong/ScoreBoard.h \
    ./src/pingPong/Table.h \
    ./src/pingPong/TableTennisSystem.h \
    ./src/qt/CCanvas.h \
    ./src/qt/GLRender.h \
    ./src/qt/SoundEngine.h \
    ./src/shader/FileShader.h \
    ./src/shader/glext.h \
    ./src/shader/Shader.h \
    ./src/utils/Base.h \
    ./src/utils/ColorDef.h \
    ./src/utils/Configuration.h \
    ./src/utils/Defines.h \
    ./src/utils/Point2.h \
    ./src/utils/Point3.h \
    ./src/utils/Timer.h \
    ./QGLViewerSrc/qglviewer.h \
./QGLViewerSrc/camera.h \
./QGLViewerSrc/manipulatedFrame.h \
./QGLViewerSrc/manipulatedCameraFrame.h \
./QGLViewerSrc/frame.h \
./QGLViewerSrc/constraint.h \
./QGLViewerSrc/keyFrameInterpolator.h \
./QGLViewerSrc/mouseGrabber.h \
./QGLViewerSrc/quaternion.h \
./QGLViewerSrc/vec.h \
./QGLViewerSrc/domUtils.h \
./QGLViewerSrc/config.h

