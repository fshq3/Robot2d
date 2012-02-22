#ifndef __C_TEST_H
#define __C_TEST_H
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <time.h>
namespace htest_lib
{
using namespace std;
#define ctest_for(index,begin,end)  for(unsigned int index=(unsigned int)begin;index<(unsigned int)end;index++)
#define ctest_foreach(type,value,index) for(type::iterator index=value.begin(),end=value.end();index!=end; index++)
//#define test_for(index,end) test_for(index,0,end)
	#define ctest_assert_basic(_cond,_type,_descript,_message)\
		if(!(_cond))\
		{\
			c_message assert;\
			assert.type=_type;\
			assert.file_name=__FILE__;\
			assert.line=__LINE__;\
			assert.descript=_descript;\
			assert.message=_message;\
			throw assert;\
		};
	#define ctest_assert_message(cond,msg) ctest_assert_basic(cond,"assert",#cond,msg)
	#define ctest_assert(cond) ctest_assert_basic(cond,"assert",#cond,"")
	#define ctest_fail(message) ctest_assert_basic(false,"assert","fail",message)
	#define ctest_assert_operator(_expected,_operator,_actual) \
	{\
		stringstream cond,msg;\
		cond<<#_expected<<#_operator<<#_actual;msg<<_expected<<#_operator<<_actual;\
		ctest_assert_basic(((_expected)_operator(_actual)),"assert",cond.str(),msg.str())\
	}

	#define ctest_assert_equal(_expected,_actual) ctest_assert_operator(_expected,==,_actual)
	#define ctest_case_add(function) add(#function,&function);
/*
CPPUNIT_ASSERT(condition)：判断condition的值是否为真，如果为假则生成错误信息。

CPPUNIT_ASSERT_MESSAGE(message, condition)：与CPPUNIT_ASSERT类似，但结果为假时报告messsage信息。

CPPUNIT_FAIL(message)：直接报告messsage错误信息。

CPPUNIT_ASSERT_EQUAL(expected, actual)：判断expected和actual的值是否相等，如果不等输出错误信息。

CPPUNIT_ASSERT_EQUAL_MESSAGE(message, expected, actual)：与CPPUNIT_ASSERT_EQUAL类似，但断言失败时输出message信息。

CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, actual, delta)：判断expected与actual的偏差是否小于delta，用于浮点数比较。

CPPUNIT_ASSERT_THROW(expression, ExceptionType)：判断执行表达式expression后是否抛出ExceptionType异常。

CPPUNIT_ASSERT_NO_THROW(expression)：断言执行表达式expression后无异常抛出。*/
	//const char * msg_types[]={"error", "exception", "assert", "ok"};
#define MT_ERROR		"error"
#define MT_EXCEPTION	"exception"
#define MT_ASSERT		"assert"
#define MT_OK			"ok"
#define MT_UNKNOW		"unknow"



	class c_message
	{
	public:
		string type;//error exception assert ok
		string file_name;
		int line;
		string descript;
		string message;
		string pack_name;
		string case_name;
		c_message(){}
		c_message(string type, string file_name, int line, string descript, string pack_name, string case_name)
			:type(type), file_name(file_name), line(line), descript(descript), pack_name(pack_name), case_name(case_name)
		{
		}
		std::string str()
		{
			stringstream res;
			res<<type<<":"<<descript<<"\n"
				<<file_name<<": "<<dec<<line<<"\n"
				<<message<<"\n"
				<<"("
				<<pack_name<<","<<case_name<<")\n\n";
			return res.str();
		}
	};



	class i_case
	{
	public:
		virtual void call(int index)=0;//运行下标为index的用例
		virtual void init()=0;
		virtual int size()=0;//用例名称
		virtual void test_start()=0;
		virtual void test_end()=0;
		virtual std::string test_regist()=0;
		virtual std::string name()=0;//本测试包名称
		virtual std::string case_name(int index)=0;//测试用例名称

	};
	class i_output
	{
	public:
		virtual void start(int count=0)=0;
		virtual void end()=0;
		virtual void start_case(i_case*,int index)=0;
		virtual void end_case(i_case*,int index)=0;
		virtual void out(c_message msg)=0;

	};
	class i_suit
	{
	public:
		virtual i_suit * add(i_case *c)=0;
		virtual i_suit * add(i_output *r)=0;
		virtual void run()=0;
	};

	class c_case:public i_case
	{

		typedef void (c_case::*fun_t) ();
		typedef std::pair<std::string,fun_t> case_t;
		std::string _name;
		std::vector<case_t> case_list;
	public:
		template<typename T>
		inline void add(std::string name,void(T::*fun)())
		{
			case_list.push_back(case_t(name,(fun_t)fun));
		}
		inline std::string operator[](int index)
		{
			return case_list[index].first;
		}
		inline void call(int index)
		{
			test_start();
			(this->*(case_list[index]).second)();
			test_end();
		}
		inline void init()
		{
			case_list.clear();
			_name=test_regist();
		}
		//本测试包名称
		virtual std::string name()
		{
			return _name;
		}
		//测试用例名称
		virtual std::string case_name(int index)
		{
			return case_list[index].first;
		}
		inline int size()
		{
			return case_list.size();
		}
		inline virtual void test_start()
		{
		}
		inline virtual void test_end()
		{
		}
		virtual std::string test_regist()=0;
	};
	class c_output_composite:public i_output
	{
		std::vector<i_output*> output_list;
	public:
		inline void add(i_output *r)
		{
			output_list.push_back(r);
		}
		inline void out(c_message msg)
		{
			ctest_for(i,0,output_list.size())
			{
				output_list[i]->out(msg);
			}
		}
		inline void end()
		{
			ctest_for(i,0,output_list.size())
			{
				output_list[i]->end();
			}
		}

		inline void start(int total_case)
		{
			ctest_for(i,0,output_list.size())
			{
				output_list[i]->start(total_case);
			}
		}
		inline void start_case(i_case *pcase,int index)
		{
			ctest_for(i,0,output_list.size())
			{
				output_list[i]->start_case(pcase,index);
			}
		}
		inline void end_case(i_case *pcase,int index)
		{
			ctest_for(i,0,output_list.size())
			{
				output_list[i]->end_case(pcase,index);
			}
		}
	};
	class c_test:i_suit
	{
	public:
		inline i_suit * add(i_case *c)
		{
			case_list.push_back(c);
			return this;
		}
		virtual i_suit * add(i_output *o)
		{
			output.add(o);
			return this;
		}
		template<typename T>
		inline c_test & add(T & cs)
		{
			add((i_case*)&cs);
			return *this;
		}
		inline void run_case(i_case * pc,int index)
		{
			output.start_case(pc,index);
			pc->test_start();
			try
			{
				pc->call(index);
				c_message msg(MT_OK,"-",0,"",pc->name(),pc->case_name(index));
				output.out(msg);
			}
			catch(c_message error)
			{
				error.pack_name=pc->name();
				error.case_name=pc->case_name(index);
				output.out(error);
			}
			catch(std::exception *e)
			{
				c_message error(MT_EXCEPTION,e->what(),0,"",pc->name(),pc->case_name(index));
				output.out(error);
			}
			catch(...)
			{
				c_message error(MT_EXCEPTION,"unknow",0,"",pc->name(),pc->case_name(index));
				//error.output(cout);
				output.out(error);
			}
			pc->test_end();
			output.end_case(pc,index);

		}
		inline void run_packet(int index)
		{
			i_case * pc = case_list[index];
			pc->init();
			ctest_for(j,0,pc->size())
			{
				run_case(pc,j);
			}
		}
		inline int get_total_case()
		{
			int res=0;
			ctest_for(i,0,case_list.size())
			{
				case_list[i]->init();
				res+=case_list[i]->size();
				cout<<case_list[i]->size()<<endl;
			}
			return res;
		}
		inline void run()
		{
			output.start(get_total_case());
			ctest_for(i,0,case_list.size())
			{
				run_packet(i);
			}
			output.end();
		}
		inline int size()
		{
			return case_list.size();
		}

	private:
		std::vector<i_case*> case_list;
		c_output_composite output;
	};


	class c_std_output:public i_output
	{
	protected:
		map<std::string,bool> accept;
		map<std::string,int> counter;
		clock_t start_ms;
		int case_count;
		int total_case;
		ostream & sout;
	public:
		c_std_output():sout(cout)
		{
			ok(false);

		}
		c_std_output(ostream & _out):sout(_out)
		{
			ok(false);
		}
		inline c_std_output & exception(bool condition)
		{
			accept[MT_EXCEPTION]=condition;
			return *this;
		}
		inline c_std_output & assert(bool condition)
		{
			accept[MT_ASSERT]=condition;
			return *this;
		}
		inline c_std_output & ok(bool condition)
		{
			accept[MT_OK]=condition;
			return *this;
		}
		inline c_std_output & error(bool condition)
		{
			accept[MT_ERROR]=condition;
			return *this;
		}
		inline bool exception()
		{
			return is_accept(MT_EXCEPTION);
		}
		bool assert()
		{
			return is_accept(MT_ASSERT);
		}
		bool ok()
		{
			return is_accept(MT_OK);
		}
		bool error()
		{
			return is_accept(MT_ERROR);
		}

		inline c_std_output& set_accept(std::string type,bool condition)
		{
			 accept[type]=condition;
			 return *this;
		}
		inline bool is_accept(std::string type)
		{
			if(accept.find(type)==accept.end())
				return true;
			return accept[type];
		}
		virtual void out(c_message msg)
		{
			counter[msg.type]++;
			if(is_accept(msg.type))
				sout<<"\n"<<msg.str()<<".\n";
		}
		bool is_ok()
		{
			map<std::string,int>::iterator it,end;
			for(it=counter.begin(),end=counter.end(); it!=end; it++)
			{
				if(MT_OK!=it->first && it->second>0)
					return false;
			}
			return true;
		}
		double get_percent()
		{
			return case_count/double(total_case)*100;
		}
		virtual void start_case(i_case*pc,int index)
		{
			case_count++;
			sout<<"\r"<<get_percent()<<"%"<<case_count<<":"<<pc->name()<<"->"<<pc->case_name(index);
		}
		virtual void end_case(i_case*,int index)
		{
			sout<<"\r"<<".\t\t\t\t\t\t\t\t\t.";
		}
		virtual void start(int count)
		{
			sout<<"=======================test start========================"<<count<<endl;
			counter.clear();
			case_count=0;
			total_case=count;
			start_ms=clock();
		}
		virtual void end()
		{
			sout<<"\nrun time:"<<double(clock()-start_ms)/CLOCKS_PER_SEC<<"s"<<endl;
			sout<<"======================================="<<endl;
			if(is_ok())
			{
					sout<<"ok"<<"("<<counter[MT_OK]<<")"<<endl;
			}else
			{
				map<std::string,int>::iterator it,end;
				for(it=counter.begin(),end=counter.end(); it!=end; it++)
				{
					sout<<it->first<<":"<<it->second<<endl;
				}
				sout<<"total:"<<case_count<<endl;
			}

		}
	};



	//facade
	class c_std_test
	{
	public:
		c_test test;
		c_std_output out;
		inline c_std_test()
		{
			test.add(&out);
		}
		inline c_std_test(ostream &_out):out(_out)
		{
			test.add(&out);
		}
		inline c_std_test add(c_case & c)
		{
			test.add(c);
			return *this;
		}
		inline void run()
		{
			test.run();
		}
	};
	/*
	测试的门面接口,继承c_std_case,并实现c_case的函数，并实例化一个对象，然后调用c_std_run::run();运行并输出结果如:
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
			c_std_run::run();
			return 0;
		}

	*/
	class c_std_run
	{
	public:
		inline static c_std_test & std_test()
		{
			static c_std_test  test;
			return test;
		}
		inline static void run()
		{
			std_test().run();
		}
	};
	class c_std_case:public c_case
	{
	public:
		inline static void run()
		{
			c_std_run::std_test().run();
		}
		inline c_std_case()
		{
			c_std_run::std_test().add(*this);
		}
	};

}
#endif
