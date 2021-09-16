#include "CCanvas.h"
#include "Player.h"

#ifdef SHOW_BEHIND
static const bool	WINDOW_FULLSCREEN = false;
#else
static const bool	WINDOW_FULLSCREEN = true;
#endif
using namespace std;

CCanvas::CCanvas(QWidget *parent) :
QGLViewer(parent)
{       
	kinectMngr = new KinectManager();
#ifdef KINECT
	kinectMngr->initialize();
#endif

	conf = Configuration::getInstance();
	gestureRecog = GestureRecognizer::getInstance();

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer->start(5);

	QTimer *timer2 = new QTimer(this);
	connect(timer2, SIGNAL(timeout()), this, SLOT(updateKinect()));
	timer2->start(40);

	currentState = MENU; //PLAYING; //MENU;

	angle = 0;
	distance = 3;

}

void CCanvas::init()
{
	setFocusPolicy(Qt::ClickFocus);

	GLenum err = glewInit(); 
	if (GLEW_OK != err) { 
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl; 
		_ASSERT(GLEW_OK != err);
	}

	string path(Path::projectRoot());
	path=path+"\\ShaderFile\\shader";

	lightShader=new FileShader(path+".vert",path+".frag");

	string txtS(Path::projectRoot());
	txtS=txtS+"\\ShaderFile\\textShader";

	textShader=new FileShader(txtS+".vert",txtS+".frag");

	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);	

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDepthMask(GL_TRUE);

	// anti aliasing stuff
	glEnable(GL_MULTISAMPLE);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

#ifndef DEFAUL_STEUP
	setCamera(new CustomCamera());
#endif

#ifndef NDEBUG
	if (WINDOW_FULLSCREEN)
		window()->showFullScreen();
#endif
	if(WINDOW_FULLSCREEN)
		window()->showFullScreen();

#ifdef STEREO_PING_PONG
	setStereoDisplay(true);

	static const double DISTANCE = conf->get("DISTANCE");
	static const double SCREEN_WIDTH = conf->get("SCREEN_WIDTH");

#ifndef DEFAUL_STEUP
	camera()->setFocusDistance(DISTANCE);
	camera()->setPhysicalScreenWidth(SCREEN_WIDTH);
#endif
#endif

	cout << glGetString(GL_VERSION) << endl;

	ttSystem = new TableTennisSystem(this,textShader,lightShader);
//	poser = new CalibrationPoser();
	trajectory = new BallTrajectoryDrawer();
	menu = new PingPongMenu(this);
}

//-----------------------------------------------------------------------------

void CCanvas::updateKinect()
{
#ifdef KINECT
	kinectMngr->update();
#endif
}


//-----------------------------------------------------------------------------

void CCanvas::resizeGL(int width, int height)
{
	QGLViewer::resizeGL(width, height);

	// set up the window-to-viewport transformation
	glViewport( 0,0, width,height );

	static const double FIELD_OF_VIEW = conf->get("FIELD_OF_VIEW");
	const double SCENE_RADIUS = conf->get("SCENE_RADIUS");
	static const double TABLE_LENGTH = conf->get("TABLE_LENGTH");

#ifndef DEFAUL_STEUP
	camera()->setFieldOfView(FIELD_OF_VIEW/180.0 * M_PI);
	camera()->setSceneRadius(SCENE_RADIUS);
	camera()->setSceneCenter(qglviewer::Vec(0,0,TABLE_LENGTH/2.0));
	camera()->setType(qglviewer::Camera::PERSPECTIVE);
	camera()->setScreenWidthAndHeight(width, height);
	//camera()->showEntireScene();
#endif
}

//-----------------------------------------------------------------------------

void CCanvas::draw()
{
	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat position[] = {0.0f, 3.0f, -1.37f, 1.0f };
	glMatrixMode(GL_MODELVIEW);

	// set up a light source
	GLfloat ambient [] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuse [] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv( GL_LIGHT0, GL_AMBIENT,  ambient  );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuse  );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
	glLightfv( GL_LIGHT0, GL_POSITION, position );

	// turn on light
	glEnable( GL_LIGHT0 );

	/*if(!TTPDLMK::isPlayerCalibrated()){
		placeDefaultCamera();
		textShader->bind();
		int r = glGetUniformLocationARB(textShader->id(), "r");
		int g = glGetUniformLocationARB(textShader->id(), "g");
		int b = glGetUniformLocationARB(textShader->id(), "b");
		glUniform1iARB(r, 1);
		glUniform1iARB(g, 0);
		glUniform1iARB(b, 0);

		QString msg(TTPDLMK::getStatusLog()->getCurrentStatus(SKELETON_LOG).c_str());
		renderText(-1,1,-2.5,msg,Fonts::logFont());
		textShader->unbind();

		lightShader->bind();
		poser->draw(lightShader->id());
		poser->update(1.0);
		lightShader->unbind();
		return;
	}*/

	lightShader->bind();

	switch(currentState)
	{
	case MENU:
		drawMenu();
		break;
	case PLAYING:
		drawGame();
		break; 
	case REPLAY:
		drawTrajectory();
		break;
	}

	lightShader->unbind();
}

void CCanvas::placeDefaultCamera()
{
	static const double DISTANCE_OFFSET = conf->get("DISTANCE_OFFSET");
	static const double PLAYER_HEIGHT = conf->get("PLAYER_HEIGHT");
	static const double LOOK_HEIGHT = conf->get("LOOK_HEIGHT");
	static const double LOOK_DISTANCE = conf->get("LOOK_DISTANCE");
	static const double CENTER_HEIGHT = conf->get("CENTER_HEIGHT");
	static const double CENTER_DISTANCE = conf->get("CENTER_DISTANCE");
	static const double TABLE_LENGTH = conf->get("TABLE_LENGTH");
	static const double SCENE_RADIUS = conf->get("SCENE_RADIUS");
	static const double z = DISTANCE_OFFSET;
	static const double x = 0.0;

	camera()->setPosition(qglviewer::Vec(x, PLAYER_HEIGHT, z));
	camera()->lookAt(qglviewer::Vec(x, LOOK_HEIGHT, z-LOOK_DISTANCE));
	camera()->setUpVector(qglviewer::Vec(0, 1, 0));
	//camera()->setIODistance(0.15);
	camera()->setFocusDistance(0);
	camera()->setSceneCenter(qglviewer::Vec(0, CENTER_HEIGHT, TABLE_LENGTH/2.0/*z-CENTER_DISTANCE*/));
	//camera()->showEntireScene();

	camera()->setSceneRadius(SCENE_RADIUS);
}


void CCanvas::drawTrajectory()
{
	static const double DISTANCE_OFFSET = conf->get("DISTANCE_OFFSET");
	static const double PLAYER_HEIGHT = conf->get("PLAYER_HEIGHT");
	static const double LOOK_HEIGHT = conf->get("LOOK_HEIGHT");
	static const double LOOK_DISTANCE = conf->get("LOOK_DISTANCE");
	static const double CENTER_HEIGHT = conf->get("CENTER_HEIGHT");
	static const double CENTER_DISTANCE = conf->get("CENTER_DISTANCE");
	static const double TABLE_LENGTH = conf->get("TABLE_LENGTH");
	static const double SCENE_RADIUS = conf->get("SCENE_RADIUS");

	trajectory->update(1.0/100.0);

	// Gestures recognition
	angle += gestureRecog->recognizeSwipe();
	distance += gestureRecog->recognizeZoom();
	distance = distance > 7? 7 : distance;
	distance = distance < 0? 0 : distance;
	if(gestureRecog->recognizeCross())
		goMenu();

	camera()->setPosition(qglviewer::Vec(cos(angle)*distance, PLAYER_HEIGHT, sin(angle)*distance -  TABLE_LENGTH / 2.0));
	camera()->lookAt(qglviewer::Vec(0, LOOK_HEIGHT, -TABLE_LENGTH / 2.0));
	camera()->setUpVector(qglviewer::Vec(0, 1, 0));
	camera()->setFocusDistance(3.);
	camera()->setSceneCenter(qglviewer::Vec(0, CENTER_HEIGHT, -TABLE_LENGTH/2.0/*z-CENTER_DISTANCE*/));

	camera()->setSceneRadius(SCENE_RADIUS);

	glTranslatef(0,0,0);
	trajectory->draw(lightShader->id());
}

void CCanvas::drawMenu()
{
#ifdef KINECT
	placeDefaultCamera();
	menu->update(1.0/100.0);
	menu->draw(lightShader->id());
#endif
}

void CCanvas::drawGame()
{
	static const double DISTANCE_OFFSET = conf->get("DISTANCE_OFFSET");
	static const double PLAYER_HEIGHT = conf->get("PLAYER_HEIGHT");
	static const double LOOK_HEIGHT = conf->get("LOOK_HEIGHT");
	static const double LOOK_DISTANCE = conf->get("LOOK_DISTANCE");
	static const double CENTER_HEIGHT = conf->get("CENTER_HEIGHT");
	static const double CENTER_DISTANCE = conf->get("CENTER_DISTANCE");
	static const double TABLE_LENGTH = conf->get("TABLE_LENGTH");
	static const double SCENE_RADIUS = conf->get("SCENE_RADIUS");

#ifdef KINECT 
	Point3d head = PingPongPlayer::getInstance()->getHead();
	
	ttSystem->update(1.0/50.0);

#else
	ttSystem->update(1.0/200.0);
#endif

#ifdef KINECT
	double hz = DISTANCE_OFFSET + head.z() - CENTER_DISTANCE;
	double hx = head.x();
	double hy = head.y() + CENTER_HEIGHT;
#ifndef DEFAUL_STEUP
	camera()->setPosition(qglviewer::Vec(hx, hy, hz));
	camera()->lookAt(qglviewer::Vec(hx, LOOK_HEIGHT, hz-LOOK_DISTANCE));
	camera()->setUpVector(qglviewer::Vec(0, 1, 0));
	//camera()->setIODistance(0.15);
	camera()->setFocusDistance(head.z());
	camera()->setSceneCenter(qglviewer::Vec(0, CENTER_HEIGHT, TABLE_LENGTH/2.0/*z-CENTER_DISTANCE*/));
	//camera()->showEntireScene();

	camera()->setSceneRadius(SCENE_RADIUS);
#endif

#else
#ifndef DEFAUL_STEUP
	glLoadIdentity();

	gluLookAt(	0.0, PLAYER_HEIGHT, 3, // camera position
		0.0, LOOK_HEIGHT,3-LOOK_DISTANCE,  // "look at" point
		0.0, 1.0, 0.0 );     // view-up vector
#endif

#endif

#ifdef KINECT
	ttSystem->draw(lightShader->id());
#else
	glTranslatef(0,0,0);
	ttSystem->draw(lightShader->id());
#endif
}

void CCanvas::play()
{
	currentState = PLAYING;
	trajectory->close();
}

void CCanvas::replay()
{
	currentState = REPLAY;
	trajectory->loadFiles();
}

void CCanvas::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()){
	case Qt::Key_A :
		conf->set("SCENE_RADIUS", conf->get("SCENE_RADIUS") + 0.1);
		break;
	case Qt::Key_Y :
		conf->set("SCENE_RADIUS", conf->get("SCENE_RADIUS") - 0.1);
		break;
	case Qt::Key_M :
		goMenu();
		break;
	case Qt::Key_K :
		play();
		break;
	case Qt::Key_L :
		replay();
		break;
	case Qt::Key_Left :
		angle += 0.2;
		break;
	case Qt::Key_Right :
		angle -= 0.2;
		break;
	case Qt::Key_Up :
		distance -= 0.2;
		break;
	case Qt::Key_Down :
		distance += 0.2;
		break;
	case Qt::Key_Escape:
		exit(0);
	}

}
