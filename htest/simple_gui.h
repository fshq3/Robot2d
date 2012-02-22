#ifndef SIMPLE_GUI_H_INCLUDED
#define SIMPLE_GUI_H_INCLUDED
#include <windows.h>
#include "commctrl.h"
#include "richedit.h"
#include <map>
#include <vector>
using namespace std;
HINSTANCE hInst;

class gui_i_message
{
public:
    virtual LRESULT on(UINT message, WPARAM wParam, LPARAM lParam)=0;
};
template<typename T>
class gui_t_message:public gui_i_message
{
public:
    typedef LRESULT (T::*fun_t)(UINT message, WPARAM wParam, LPARAM lParam);
    fun_t m_fun;
    T * m_point;
    gui_t_message(T *a_point,fun_t a_fun)
    {
        m_point = a_point;
        m_fun 	= a_fun;
    }
    LRESULT on(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if(m_fun && m_point)
        {
            return (m_point->*m_fun)(message,wParam,lParam);
        }
        return 0;
    }
};
class guim_event_contain
{
public:
    map<UINT,vector<gui_i_message*> > m_event;
    template<typename T>
    void add_event(UINT message,T * a_point,typename gui_t_message<T>::fun_t a_fun)
    {
        m_event[message].push_back(new gui_t_message<T>(a_point,a_fun));
    }
    LRESULT onEvent(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
    {
        if(m_event.find(message)!=m_event.end())
        {
            int res;
            vector<gui_i_message*>::iterator it=m_event[message].begin(),end=m_event[message].end();
            for(; it!=end; it++)
            {
                res=(*it)->on(message,wParam,lParam);
            }
            return res;
        }
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
};
class gui_wnd
{
public:
	HWND hwnd;
	guim_event_contain m_event;
	gui_wnd()
	{
		hwnd=NULL;
	}
	static map<HWND,gui_wnd*> & getDialogMap()
    {
        static map<HWND,gui_wnd*> dialog_map;
        return dialog_map;
    }
    LRESULT onEvent(UINT message, WPARAM wParam, LPARAM lParam)
    {
        return m_event.onEvent(hwnd,message,wParam,lParam);
    }

	void set_text(const char * text)
    {
		SendMessage(// returns LRESULT in lResult
								(HWND) hwnd,   // handle to destination control
								(UINT) WM_SETTEXT,      // message ID
								0,      // not used
								(LPARAM) text      //  Pointer to a null-terminated string that is the window text
							);
    }
    void get_text(char * text,int len)
    {
		SendMessage(// returns LRESULT in lResult
								(HWND) hwnd,   // handle to destination control
								(UINT) WM_GETTEXT,      // message ID
								len,      // not used
								(LPARAM) text      //  Pointer to a null-terminated string that is the window text
							);
    }
    static const char * register_simple()
    {
        static const char *szClassName="simple_gui";
        static bool is_register=false;
        if(!is_register)
        {
            WNDCLASSEX wincl;        /* Data structure for the windowclass */

            /* The Window structure */
            wincl.hInstance = hInst;
            wincl.lpszClassName = (LPCSTR)szClassName;
            wincl.lpfnWndProc = DialogProc;      /* This function is called by windows */
            wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
            wincl.cbSize = sizeof (WNDCLASSEX);

            /* Use default icon and mouse-pointer */
            wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
            wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
            wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
            wincl.lpszMenuName = NULL;                 /* No menu */
            wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
            wincl.cbWndExtra = 0;                      /* structure or the window instance */
            /* Use Windows's default colour as the background of the window */
            wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

            /* Register the window class, and if it fails quit the program */
            if (!RegisterClassEx (&wincl))
                return NULL;
			is_register=true;
        }
        return szClassName;

    }

    static LRESULT  CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        gui_wnd * wnd;
        wnd=getDialogMap()[hwnd];
        if(message==WM_CREATE)
        {
            LPCREATESTRUCT pC=(LPCREATESTRUCT)lParam;
            wnd=(gui_wnd*)pC->lpCreateParams;
            if(wnd)
                wnd->hwnd=hwnd;
        }
        if(wnd)
        {
            return wnd->onEvent(message,wParam,lParam);
        }
        else
        {
            return DefWindowProc (hwnd, message, wParam, lParam) ;
        }
    }
};
class gui_dialog:public gui_wnd
{
public:
    MSG    msg ;
    bool is_closed;
    gui_dialog()
    {
        m_event.add_event(WM_CREATE,this,&gui_dialog::onCreate);
        m_event.add_event(WM_PAINT,this,&gui_dialog::onPaint);
        m_event.add_event(WM_DESTROY,this,&gui_dialog::onDestroy);
    }

    virtual LRESULT onCreate(UINT message, WPARAM wParam, LPARAM lParam)
    {
        /*MessageBox (  NULL, TEXT ("OnCreate"),

                      "on", MB_ICONERROR) ;*/
        return 0;
    }
    LRESULT onDestroy(UINT message, WPARAM wParam, LPARAM lParam)
    {
        PostQuitMessage (0) ;
        is_closed=true;
        return 0;
    }
    virtual LRESULT onPaint(UINT message, WPARAM wParam, LPARAM lParam)
    {
        HDC	hdc ;

        PAINTSTRUCT ps ;

        RECT          rect ;

        hdc = BeginPaint (hwnd, &ps) ;

        GetClientRect (hwnd, &rect) ;

        DrawText (hdc, TEXT ("Hello, Windows onPaint!"), -1, &rect,

                  DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

        EndPaint (hwnd, &ps);
        return 0;
    }

    void create(HINSTANCE hInst, int nCmdShow,
                int x= CW_USEDEFAULT,
                int y= CW_USEDEFAULT,
                int nWidth= CW_USEDEFAULT,
                int nHeight= CW_USEDEFAULT)
    {
		const char * szClassName = register_simple();
		if(szClassName==NULL)
			return;

        /* The class is registered, let's create the program*/
        hwnd = CreateWindowEx (
                   0,                   /* Extended possibilites for variation */
                   (LPCTSTR)szClassName,         /* Classname */
                   TEXT("Code::Blocks Template Windows App"),       /* Title Text */
                   WS_OVERLAPPEDWINDOW, /* default window */
                   x,       /* Windows decides the position */
                   y,       /* where the window ends up on the screen */
                   nWidth,                 /* The programs width */
                   nHeight,                 /* and height in pixels */
                   HWND_DESKTOP,        /* The window is a child-window to desktop */
                   NULL,                /* No menu */
                   hInst,       /* Program Instance handler */
                   (void*)(gui_wnd*)this                 /* No Window Creation data */
               );

        /* Make the window visible on the screen */
        ShowWindow (hwnd, nCmdShow);

        getDialogMap()[hwnd]=this;

        UpdateWindow (hwnd) ;
		is_closed=false;

    }
    bool step(int ms=33)
    {//GetMessage (&msg, NULL, 0, 0)

        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
			//DialogProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
            return true;
        }
        Sleep(ms);
        return false;
    }
    int run()
    {
        while (GetMessage (&msg, NULL, 0, 0))
        {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
        }
        return msg.wParam;
    }
};

class gui_edit:public gui_wnd
{
public:
    void create(gui_wnd &parent,HINSTANCE hInst,
                int x,
                int y,
                int nWidth,
                int nHeight)
    {
        hwnd = CreateWindowEx(NULL,
                               TEXT("edit"),
                               NULL,
                               WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |

							   WS_BORDER | ES_LEFT | ES_MULTILINE |

							   ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                               x,
                               y,
                               nWidth,
                               nHeight,
                               parent.hwnd,
                               (HMENU)this,
                               hInst,
                               (void*)(gui_wnd*)this);
		parent.m_event.add_event(WM_COMMAND,this,&gui_edit::onCommand);
    }

    LRESULT onCommand(UINT message, WPARAM wParam, LPARAM lParam)
    {
		return 0;
    }

    void SetBkColor(unsigned char r,unsigned char g,unsigned char b)
    {
    	SendMessage(      // returns LRESULT in lResult
						(HWND) hwnd,      // handle to destination control
						(UINT) EM_SETBKGNDCOLOR,      // message ID
						(WPARAM) 0,      // = (WPARAM) () wParam;
						(LPARAM) RGB(r, g, b)      // = (LPARAM) () lParam;
						);
    }
};
class gui_report:public gui_wnd
{
public:
    void create(HWND hParent,HINSTANCE hInst,
                int x,
                int y,
                int nWidth,
                int nHeight)
    {
        hwnd = CreateWindowEx(NULL,
                               TEXT("SysListView32"),
                               NULL,
                               LVS_REPORT | WS_CHILD | WS_VISIBLE,
                               x,
                               y,
                               nWidth,
                               nHeight,
                               hParent,
                               NULL,
                               hInst,
                               NULL);
    }
    int InsertColumn(int nCol,const char* title,int nWidth,int nFormat=LVCFMT_LEFT)
    {
        LV_COLUMN lvc;
        ZeroMemory(&lvc, sizeof(LV_COLUMN));
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT ;
        lvc.pszText = (LPTSTR)title;
        lvc.cx = nWidth;
        lvc.fmt = nFormat;
        return SendMessage(hwnd, LVM_INSERTCOLUMN, nCol, (long)&lvc);
    }
    void SetTextColor(unsigned char r,unsigned char g,unsigned char b)
    {
		//SendMessage(hwnd,WM_SETREDRAW,TRUE,0);

        SendMessage(hwnd, LVM_SETTEXTCOLOR, 0, RGB(r, g, b));
    }
    int InsertItem(int iItem,const char * value)
    {
        LV_ITEM item;       // 项
        ZeroMemory(&item, sizeof(LV_ITEM));
        // 添加一些行项
        item.mask = LVIF_TEXT;       // 文字
        item.cchTextMax = MAX_PATH;       // 文字长度
        item.iItem = iItem;

        item.iSubItem = 0;
        item.pszText = (LPTSTR)value;
        return SendMessage(hwnd, LVM_INSERTITEM, 0, (LPARAM)&item);
    }
    bool SetItemText(int iItem,int iSubItem,char * value)
    {
        LV_ITEM item;       // 项
        ZeroMemory(&item, sizeof(LV_ITEM));
        // 设置子项
        item.mask = LVIF_TEXT;       // 文字
        item.cchTextMax = MAX_PATH;       // 文字长度
        item.iItem = iItem;

        item.iSubItem = iSubItem;
        item.pszText = value;
        return SendMessage(hwnd, LVM_SETITEMTEXT, 0, (LPARAM)&item)==TRUE;
    }
    void SetBkColor(unsigned char r,unsigned char g,unsigned char b)
    {
    	SendMessage(hwnd, LVM_SETBKCOLOR, 0, RGB(r, g, b));
    }
    //SendMessage(hList, LVM_SETTEXTBKCOLOR, 0, RGB(0, 0, 0));
};

#endif // SIMPLE_GUI_H_INCLUDED
