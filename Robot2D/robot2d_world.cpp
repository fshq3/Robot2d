#include "robot2d_world.h"
#include <tinyxml/tinyxml.h>
#include <iostream>
using namespace std;
class c_xml2world
{
public:
	int newId()
	{
		static int cur_id=0;
		return ++cur_id;
	}
	b2CircleShape XmlShapeCircle(TiXmlNode * node)
	{
		b2CircleShape circle;
		TiXmlElement *e=node->ToElement();
		if(e)
		{
			double radius=0;
			e->Attribute("radius",&radius);
			circle.m_radius=radius;
		}
		return circle;
	}
	b2EdgeShape XmlShapeEdge(TiXmlNode *node)
	{
		b2EdgeShape edge;
		b2Vec2 p[2];
		TiXmlNode * pn = node->FirstChild("p");
		for(int i=0; i<2;i++)
		{
			if(pn)
			{
				p[i]=XmlVec2(pn);
				pn=pn->NextSibling("p");
			}
		}
		edge.Set(p[0],p[1]);
		return edge;
	}
	b2PolygonShape XmlShapeBox(TiXmlNode * node)
	{

		b2PolygonShape box;
		if(!node)
			return box;
		TiXmlElement *e=node->ToElement();
		if(e)
		{
			double width,height;
			e->Attribute("width",&width);
			e->Attribute("height",&height);
			box.SetAsBox(width, height);
		}
		return box;
	}
	b2PolygonShape XmlShapePolygon(TiXmlNode * node)
	{

		b2PolygonShape p;
		if(!node)
			return p;
		b2Vec2 vertices[b2_maxPolygonVertices];
		int vertexCount = 0;
		TiXmlNode *it=node->FirstChild("p");
		for(;it!=NULL && vertexCount<=b2_maxPolygonVertices;it=it->NextSibling("p"))
		{
			vertices[vertexCount++]=XmlVec2(it);
		}
		p.Set(vertices,vertexCount);
		return p;
	}
	b2Vec2 XmlVec2(TiXmlNode * node)
	{
		b2Vec2 vec;
		if(node)
		{
			TiXmlElement *e=node->ToElement();
			if(e)
			{
				double x=0,y=0;
				e->Attribute("x",&x);
				e->Attribute("y",&y);
				vec.Set(x,y);
				cout<<x<<","<<y<<endl;
			}
		}
		return vec;
	}
	b2BodyDef XmlBody(TiXmlNode * node)
	{
		b2BodyDef bd;

		string type=node->ToElement()->Attribute("type");
		if(type=="static"){
			bd.type = b2_staticBody;
		}else{
			bd.type = b2_dynamicBody;
		}
		bd.position=XmlVec2(node->FirstChild("position"));
		return bd;
	}
	b2FixtureDef XmlFixture(TiXmlNode *node)
	{
		b2FixtureDef fd;
		fd.shape=NULL;
		if(node)
		{
			TiXmlElement *e=node->ToElement();
			if(e)
			{
				double density=0,friction=0,restitution=0;
				e->Attribute("density",&(density));
				e->Attribute("friction",&(friction));
				e->Attribute("restitution",&(restitution));
				fd.density=density;
				fd.friction=friction;
				fd.restitution=restitution;
			}
		}
		return fd;
	}
	void XmlFixShape(TiXmlNode *shape,b2Body* body,void*userData)
	{
		b2FixtureDef fd;
		b2CircleShape circle;
		b2EdgeShape edge;
		b2PolygonShape polygon;
		if(shape)
		{
			fd=XmlFixture(shape);
			fd.userData=userData;
			TiXmlElement *e=shape->ToElement();
			if(e)
			{
				string type(e->Attribute("type"));
				if(type=="circle")
				{
					circle = XmlShapeCircle(shape);
					fd.shape=&circle;
				}else if(type=="polygon")
				{
					polygon = XmlShapePolygon(shape);
					fd.shape=&polygon;
				}else if(type=="edge")
				{
					edge = XmlShapeEdge(shape);
					fd.shape=&edge;
				}else if("box"==type)
				{
					polygon = XmlShapeBox(shape);
					fd.shape=&polygon;
				}
				body->CreateFixture(&fd);
			}
		}

	}
	b2Body* XmlBox2d(TiXmlNode * node,b2World * world)
	{
		if(!node)
			return NULL;
		b2Body* body = NULL;

		b2BodyDef bd;
		bd=XmlBody(node->FirstChild("body"));

		body = world->CreateBody(&bd);
		TiXmlNode *shape=node->FirstChild("shape");
		while(shape)
		{

			XmlFixShape(shape,body,(void*)newId());
			shape=shape->NextSibling("shape");
		}
		return body;
	}
	c_xml2world(TiXmlDocument & doc)
	{
		b2Vec2 gravity(0.0f, -10.0f);
		m_world = new b2World(gravity);
		bool loadOkay = doc.LoadFile();
		if(!loadOkay)
		{
			std::cout<<"error"<<std::endl;
			return;
		}
		TiXmlNode* node = 0;

		TiXmlElement* itemElement = 0;
		node = doc.FirstChild("world");
		if(node)
		{
			TiXmlElement* element = 0;
			node->ToElement();
			TiXmlNode* item=node->FirstChild("box2d");
			for(;item;item=item->NextSibling("box2d"))
			{
				int i=1;
				item->ToElement()->Attribute("number",&i);
				while(i--)
					XmlBox2d(item,m_world);
				TiXmlElement * e=item->ToElement();
				e->Print(stdout,0);
			}

		}


	}

	void Keyboard(unsigned char key)
	{

	}

	void Step()
	{

		b2Contact * contact=m_world->GetContactList();
		for(;contact!=NULL;contact=contact->GetNext()){
			//m_debugDraw.DrawString(5, m_textLine, "%d->%d",contact->GetFixtureA()->GetUserData(),contact->GetFixtureB()->GetUserData());
			//m_textLine += 15;
		}
		//settings->viewCenter.x = m_car->GetPosition().x;

	}

	b2World *m_world;

};

void c_robot2d_world::create()
{
	TiXmlDocument doc( "Robot2dTest.xml");
	c_xml2world xml_world(doc);
	m_world = xml_world.m_world;
}
void c_robot2d_world::step(float32 tick_interval)
{
	if(m_world==NULL)
		return;
	// Prepare for simulation. Typically we use a time step of 1/60 of a
	// second (60Hz) and 10 iterations. This provides a high quality simulation
	// in most game scenarios.
	float32 timeStep = 1.0f / tick_interval;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	// Instruct the world to perform a single step of simulation.
	// It is generally best to keep the time step and iterations fixed.
	m_world->Step(timeStep, velocityIterations, positionIterations);

}
void c_robot2d_world::draw()
{
	if(m_world==NULL)
		return;
	drawer.DrawWorld(m_world);
}
