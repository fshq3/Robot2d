#ifndef HTEST_GUI_H_INCLUDED
#define HTEST_GUI_H_INCLUDED
#include "htest.h"
#include "simple_gui.h"
using namespace htest_lib;
class c_gui_output:public c_std_output
{
public:
		gui_dialog dialog;
		gui_report report;
		gui_edit edit;
		stringstream sout;
		c_gui_output():c_std_output(sout)
		{
			dialog.m_event.add_event(WM_SIZE,this,&c_gui_output::onSize);
		}
		LRESULT onSize(UINT message, WPARAM wParam, LPARAM lParam)
		{
			MoveWindow (report.hwnd, 0, 0, LOWORD (lParam), HIWORD (lParam), TRUE) ;
			edit.set_text("onSize");
			return 0;
		}
		void start(int count)
		{
			dialog.create(NULL,10,0,0,800,700);
			report.create(dialog.hwnd,NULL,0,0,800,600);
			edit.create(dialog,NULL,0,600,800,60);
			report.InsertColumn(0,"type",200);
			report.InsertColumn(1,"descript",160);
			report.InsertColumn(2,"message",200);
			report.InsertColumn(3,"packet",100);
			report.InsertColumn(4,"case",100);
			report.InsertColumn(5,"file",200);
			report.InsertColumn(6,"line",40);
			edit.SetBkColor(0,200,0);
			step();
			c_std_output::start(count);
		}
		void end()
		{
			stringstream sout;
			if(is_ok())
			{
					sout<<"ok"<<"("<<counter[MT_OK]<<")"<<endl;
			}else
			{
				map<std::string,int>::iterator it,end;
				for(it=counter.begin(),end=counter.end(); it!=end; it++)
				{
					sout<<"("+it->first<<":"<<it->second<<")";
				}
				sout<<"(total:"<<case_count<<")";
			}
			dialog.set_text(sout.str().c_str());
			edit.set_text(sout.str().c_str());
			dialog.run();
			c_std_output::end();
		}
		void step()
		{
			while(dialog.step())
			;
		}
		virtual void start_case(i_case*pc,int index)
		{
			stringstream text;
			text<<get_percent()<<"%"<<pc->name()<<":"<<pc->case_name(index);
			dialog.set_text(text.str().c_str());
			edit.set_text(text.str().c_str());
			step();
			c_std_output::start_case(pc,index);
		}
		virtual void end_case(i_case*pc,int index)
		{
			step();
			c_std_output::end_case(pc,index);
		}
		virtual void out(c_message msg)
		{
			c_std_output::out(msg);
			step();
			if(!is_accept(msg.type))
			{
				return;
			}
			edit.SetBkColor(200,0,0);
			int n=report.InsertItem(0,(char*)msg.type.c_str());
			report.SetItemText(n,1,(char*)msg.descript.c_str());
			report.SetItemText(n,2,(char*)msg.message.c_str());
			report.SetItemText(n,3,(char*)msg.pack_name.c_str());
			report.SetItemText(n,4,(char*)msg.case_name.c_str());
			report.SetItemText(n,5,(char*)msg.file_name.c_str());
			stringstream ss;
			ss<<msg.line;
			report.SetItemText(n,6,(char*)ss.str().c_str());
		}
};
class c_gui_run
{
	inline static void init_output()
	{
		static c_gui_output out;
		static bool is_init=false;
		if(!is_init)
		{
			c_std_run::std_test().test.add(&out);
			is_init=true;
		}
	}
public:
	inline static void run()
	{
		init_output();
		c_std_run::std_test().run();
	}
};
	/*
	测试的门面接口,继承c_std_case,并实现c_case的函数，并实例化一个对象，然后调用c_gui_run::run();运行并输出结果如:
		class testFile:public c_std_case
		{
		public:
			string test_regist()
			{
				add("test block",&testFile::test_block);
				return "testFile";
			}
			void test_block()
			{
				ctest_assert(false);
			}

		}_testFile;
		int main()
		{
			c_gui_run::run();
			return 0;
		}

	*/
#endif // HTEST_GUI_H_INCLUDED
