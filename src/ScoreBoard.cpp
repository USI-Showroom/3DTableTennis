#include "ScoreBoard.h"

#include <sstream>

ScoreBoard::ScoreBoard()
	:mesh(Mesh::meshFromOBJ(Path::meshFolder()+"\\scoreBoard.obj")),
	textures(),
	time(0),
	score1(-1), score2(-1),
	currentD1(-1), currentU1(-1),
	currentD2(-1), currentU2(-1)
{

	std::vector<Material> &mats=mesh.getMaterials();

	for(int i=0;i<mats.size();++i)
	{
		Material *m=&mats[i];

		if(m->getName()=="M_0"){
			d1=m;
			m->clearTexture();
			continue;
		}
		else if(m->getName()=="M_1")
		{
			u1=m;
			m->clearTexture();
			continue;
		}
		else if(m->getName()=="M_2")
		{
			d2=m;
			m->clearTexture();
			continue;
		}
		else if(m->getName()=="M_3")
		{
			u2=m;
			m->clearTexture();
			continue;
		}
	}

	for(int i=0;i<10;++i)
	{
		std::ostringstream s;
		s<<Path::textureFolder()<<"\\scoreBoard\\M_"<<i<<".png";

		textures.push_back(new Texture3D(s.str()));
	}

	setScore1(0);
	setScore2(0);
}

void ScoreBoard::update(const double tau)
{
	time+=tau;

	//setScore1((int) time);
	//setScore2((int) time);
}
	
void ScoreBoard::draw(unsigned int shaderId)
{
	mesh.draw(shaderId);
}

void ScoreBoard::setScore1(int v)
{
	if(score1==v)
		return;

	score1=v;

	if(score1/10!=currentU1)
	{
		currentU1=score1/10;
		u1->setTexture(textures[currentU1]);
	}

	if(score1%10!=currentD1)
	{
		currentD1=score1%10;
		d1->setTexture(textures[currentD1]);
	}

}

void ScoreBoard::setScore2(int v)
{
	if(score2==v)
		return;

	score2=v;

	if(score2/10!=currentU2)
	{
		currentU2=score2/10;
		u2->setTexture(textures[currentU2]);
	}

	if(score2%10!=currentD2)
	{
		currentD2=score2%10;
		d2->setTexture(textures[currentD2]);
	}
}

void ScoreBoard::increaseScore1()
{
	setScore1(score1+1);
}

void ScoreBoard::increaseScore2()
{
	setScore1(score2+1);
}