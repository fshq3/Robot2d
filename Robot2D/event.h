#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED
#include <vector>
using namespace std;
class i_event
{
public:
	virtual Uint8 type()=0;
	virtual void on_event(const SDL_Event &e)=0;
};
class c_event_contain:public i_event
{
private:
	vector<i_event *> event_list;
	c_event_contain(){}
public:
	static c_event_contain & instance()
	{
		static c_event_contain contain;
		return contain;
	}
	void add(i_event *pevent)
	{
		event_list.push_back(pevent);
	}
	void del(i_event *pevent)
	{
		vector<i_event *>::iterator it=find(event_list.begin(),event_list.end(),pevent);
		if(it!=event_list.end())
			event_list.erase(it);
	}
	Uint8 type()
	{
		return SDL_NOEVENT;
	}
	void on_event(const SDL_Event &e)
	{
		vector<i_event *>::iterator it=event_list.begin()
			,end=event_list.end();
		for(;it!=end; it++){
			if((*it)->type()==e.type || (*it)->type()==SDL_NOEVENT ){
				(*it)->on_event(e);
			}
		}

	}
};
class c_event:public i_event
{
public:
	c_event()
	{
		c_event_contain::instance().add(this);
	}
	~c_event()
	{
		c_event_contain::instance().del(this);
	}
	//type() and on_event need to imp
};

#endif // EVENT_H_INCLUDED
