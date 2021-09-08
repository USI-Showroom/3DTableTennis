#include "Score.h"
#include "CCanvas.h"
#include <sstream>

Score::Score(CCanvas *canvas,Shader *textShader,Shader *shader)
	:canvas(canvas),textShader(textShader),shader(shader),score(0),launches(0),timer(0)
{
}

void Score::onLaunch()
{
	++launches;
}

void Score::update()
{
	if(finish())
		++timer;

	if(timer > 600)
	{
		canvas->replay();
		score = 0;
		launches = 0;
		timer = 0;
	}
}

void Score::onBallHit(int numHits)
{
	score += 30*numHits;
}

void Score::onBallOnTable()
{
	score += 10;
}

void Score::onWinMatch()
{
	score += 100;
}

void Score::draw()
{
	shader->unbind();
	textShader->bind();
	int r = glGetUniformLocationARB(textShader->id(), "r");
	int g = glGetUniformLocationARB(textShader->id(), "g");
	int b = glGetUniformLocationARB(textShader->id(), "b");
	glUniform1iARB(r, 0.1);
	glUniform1iARB(g, 0.1);
	glUniform1iARB(b, 0.1);
	std::stringstream str;
	if(finish())
	{
		str << "Game Over - Score: " << score;
		QString msg(str.str().c_str());
		canvas->renderText(-2.2,3,-7.4,msg,Fonts::logFont());
	}
	else
	{
		str << "Score: " << score << "	 Balls: " << MAXB-launches;
		QString msg(str.str().c_str());
		canvas->renderText(-2.2,3,-7.4,msg,Fonts::logFont());
	}
	
	textShader->unbind();

	glPushMatrix();
	glTranslatef(0, 3.2, -8.0);
	glScalef(3,0.5,0.5);
	glBegin(GL_QUADS);
	if(finish())
		glColor3f(1.0f, 0.1f, 0.1f);
	else
		glColor3f(0.8f, 0.8f, 0.8f);

		// Top face (y = 1.0f)
		glVertex3f( 1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f,  1.0f);
		glVertex3f( 1.0f, 1.0f,  1.0f);
 
		// Bottom face (y = -1.0f)
		glVertex3f( 1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f( 1.0f, -1.0f, -1.0f);
 
		// Front face  (z = 1.0f)
		glVertex3f( 1.0f,  1.0f, 1.0f);
		glVertex3f(-1.0f,  1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f( 1.0f, -1.0f, 1.0f);
 
		// Back face (z = -1.0f)
		glVertex3f( 1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f);
		glVertex3f( 1.0f,  1.0f, -1.0f);
 
		// Left face (x = -1.0f)
		glVertex3f(-1.0f,  1.0f,  1.0f);
		glVertex3f(-1.0f,  1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f,  1.0f);
 
		// Right face (x = 1.0f)
		glVertex3f(1.0f,  1.0f, -1.0f);
		glVertex3f(1.0f,  1.0f,  1.0f);
		glVertex3f(1.0f, -1.0f,  1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();
	glPopMatrix();

	shader->bind();
}