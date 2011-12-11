#include <Windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <iostream>

#define MAXSIZEX 640
#define MAXSIZEY 480
HGLRC		hRC=	NULL;
HDC			hDC=	NULL;
HWND		hWnd=	NULL;
HINSTANCE	hInstance;
int MIDDLEX=MAXSIZEX>>1,
	MIDDLEY=MAXSIZEY>>1;
int			MouseWheel=0;

bool	keys[256];								// ������̰���������
bool	active=TRUE;								// ���ڵĻ��־��ȱʡΪTRUE
bool	fullscreen=TRUE;							// ȫ����־ȱʡ��ȱʡ�趨��ȫ��ģʽ

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);				// WndProc�Ķ���

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)				// ����OpenGL���ڴ�С
{
	if (height==0)								// ��ֹ�����
	{
		height=1;							// ��Height��Ϊ1
	}

	glViewport(0, 0, width, height);					// ���õ�ǰ���ӿ�
	glMatrixMode(GL_PROJECTION);						// ѡ��ͶӰ����
	glLoadIdentity();							// ����ͶӰ����

	// �����ӿڵĴ�С
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);						// ѡ��ģ�͹۲����
	glLoadIdentity();							// ����ģ�͹۲����
}



int InitGL(GLvoid)								// �˴���ʼ��OpenGL������������
{

	glClearColor(0.5f,0.7f,0.0f,0.5f);			// ���ñ�������ɫΪ��������ɫ

 	glClearDepth(1.0f);							// ������Ȼ���

 	glEnable(GL_DEPTH_TEST);						// ������Ȳ���
 	glDepthFunc(GL_LEQUAL);							// ������Ȳ��Ե�����

	return TRUE;
}

int DrawGLScene(GLvoid)								// �����￪ʼ�������еĻ���
{
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// �����Ļ����Ȼ���

	glLoadIdentity();							// ���õ�ǰ��ģ�͹۲����

	return TRUE;								//  һ�� OK
}

GLvoid KillGLWindow(GLvoid)							// �������ٴ���
{
	if(fullscreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}
	if(hRC)
	{
		if(!wglMakeCurrent(NULL,NULL))
			MessageBox(NULL,"�ͷ�DC��RCʧ��",NULL,MB_OK);
		if(!wglDeleteContext(hRC))
			MessageBox(NULL,"�ͷ�RCʧ��",NULL,MB_OK);
		hRC=NULL;
	}
	if (hDC && !ReleaseDC(hWnd,hDC))					// �����ܷ��ͷ� DC?
	{
		MessageBox(NULL,"�ͷ�DCʧ�ܡ�",NULL,MB_OK | MB_ICONINFORMATION);
		hDC=NULL;							// �� DC ��Ϊ NULL
	}
	if(hWnd && !DestroyWindow(hWnd))
	{
		MessageBox(NULL,"�ͷŴ��ھ䱭ʧ��",NULL,MB_OK);
		hWnd=NULL;
	}
	if(!UnregisterClass("OpenG",hInstance))
	{
		MessageBox(NULL,"����ע��������",NULL,MB_OK);
		hInstance=NULL;
	}
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint	PixelFormat;
	WNDCLASS		wc;
	DWORD			dwExStyle;
	DWORD			dwStyle;
	RECT WindowRect;							// ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
	WindowRect.left=(long)0;						// ��Left   ��Ϊ 0
	WindowRect.right=(long)width;						// ��Right  ��ΪҪ��Ŀ��
	WindowRect.top=(long)0;							// ��Top    ��Ϊ 0
	WindowRect.bottom=(long)height;						// ��Bottom ��ΪҪ��ĸ߶�

	fullscreen=fullscreenflag;						// ����ȫ��ȫ����־

	hInstance		= GetModuleHandle(NULL);			// ȡ�����Ǵ��ڵ�ʵ��
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
	wc.lpfnWndProc		= (WNDPROC) WndProc;				// WndProc������Ϣ
	wc.cbClsExtra		= 0;						// �޶��ⴰ������
	wc.cbWndExtra		= 0;						// �޶��ⴰ������
	wc.hInstance		= hInstance;					// ����ʵ��
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);			// װ��ȱʡͼ��
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);			// װ�����ָ��
	wc.hbrBackground	= NULL;						// GL����Ҫ����
	wc.lpszMenuName		= NULL;						// ����Ҫ�˵�
	wc.lpszClassName	= "OpenG";					// �趨������

	if (!RegisterClass(&wc))						// ����ע�ᴰ����
	{
		MessageBox(NULL,"ע�ᴰ��ʧ��","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// �˳�������FALSE
	}

	if(fullscreen)
	{
		DEVMODE dmScreenSettings;						// �豸ģʽ
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// ȷ���ڴ����Ϊ��
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// Devmode �ṹ�Ĵ�С
		dmScreenSettings.dmPelsWidth	= width;				// ��ѡ��Ļ���
		dmScreenSettings.dmPelsHeight	= height;				// ��ѡ��Ļ�߶�
		dmScreenSettings.dmBitsPerPel	= bits;					// ÿ������ѡ��ɫ�����
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL,"ȫ��ģʽ�ڵ�ǰ�Կ�������ʧ�ܣ�\nʹ�ô���ģʽ��","NeHe G",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				fullscreen=FALSE;
			else
			{
				MessageBox(NULL,"���򽫱��ر�","����",MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	if(fullscreen)
	{
		dwExStyle=WS_EX_APPWINDOW;					// ��չ������
		dwStyle=WS_POPUP;						// ������
		ShowCursor(FALSE);						// �������ָ��
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// ��չ������
		dwStyle=WS_OVERLAPPEDWINDOW;					//  ������
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// �������ڴﵽ����Ҫ��Ĵ�С

	if (!(hWnd=CreateWindowEx(	dwExStyle,				// ��չ������
		"OpenG",				// ������
		title,					// ���ڱ���
		WS_CLIPSIBLINGS |			// ����Ĵ���������
		WS_CLIPCHILDREN |			// ����Ĵ���������
		dwStyle,				// ѡ��Ĵ�������
		0, 0,					// ����λ��
		WindowRect.right-WindowRect.left,	// ��������õĴ��ڿ��
		WindowRect.bottom-WindowRect.top,	// ��������õĴ��ڸ߶�
		NULL,					// �޸�����
		NULL,					// �޲˵�
		hInstance,				// ʵ��
		NULL)))					// ����WM_CREATE�����κζ���
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"���ܴ���һ�������豸������","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	static	PIXELFORMATDESCRIPTOR pfd=					// /pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// ������ʽ�������Ĵ�С
		1,								// �汾��
		PFD_DRAW_TO_WINDOW |						// ��ʽ֧�ִ���
		PFD_SUPPORT_OPENGL |						// ��ʽ����֧��OpenGL
		PFD_DOUBLEBUFFER,						// ����֧��˫����
		PFD_TYPE_RGBA,							// ���� RGBA ��ʽ
		bits,								// ѡ��ɫ�����
		0, 0, 0, 0, 0, 0,						// ���Ե�ɫ��λ
		0,								// ��Alpha����
		0,								// ����Shift Bit
		0,								// ���ۼӻ���
		0, 0, 0, 0,							// ���Ծۼ�λ
		16,								// 16λ Z-���� (��Ȼ���)
		0,								// ���ɰ建��
		0,								// �޸�������
		PFD_MAIN_PLANE,							// ����ͼ��
		0,								// Reserved
		0, 0, 0								// ���Բ�����
	};

	if (!(hDC=GetDC(hWnd)))							// ȡ���豸��������ô?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"���ܴ���һ����ƥ������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))				// Windows �ҵ���Ӧ�����ظ�ʽ����?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))				// �ܹ��������ظ�ʽô?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	if (!(hRC=wglCreateContext(hDC)))					// �ܷ�ȡ����ɫ������?
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"���ܴ���OpenGL��Ⱦ������","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	if(!wglMakeCurrent(hDC,hRC))						// ���Լ�����ɫ������
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"���ܼ��ǰ��OpenGL��Ȼ������","����",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	ShowWindow(hWnd,SW_SHOW);						// ��ʾ����
	SetForegroundWindow(hWnd);						// ����������ȼ�
	SetFocus(hWnd);								// ���ü��̵Ľ������˴���
	ReSizeGLScene(width, height);						// ����͸�� GL ��Ļ
	if (!InitGL())								// ��ʼ���½���GL����
	{
		KillGLWindow();							// ������ʾ��
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// ���� FALSE
	}
	return TRUE;	
}

LRESULT CALLBACK WndProc(	HWND	hWnd,					// ���ڵľ��
						 UINT	uMsg,					// ���ڵ���Ϣ
						 WPARAM	wParam,					// ���ӵ���Ϣ����
						 LPARAM	lParam)					// ���ӵ���Ϣ����
{

	switch(uMsg)
	{
	case WM_ACTIVATE:
		if (!HIWORD(wParam))					// �����С��״̬
		{
			active=TRUE;					// �����ڼ���״̬
		}
		else
		{
			active=FALSE;					// �����ټ���
		}
		return 0;
	case WM_SYSCOMMAND:						// ϵͳ�ж�����
		{
			switch (wParam)						// ���ϵͳ����
			{
			case SC_SCREENSAVE:				// ����Ҫ����?
			case SC_MONITORPOWER:				// ��ʾ��Ҫ����ڵ�ģʽ?
				return 0;					// ��ֹ����
			}
			break;							// �˳�
		}
	case WM_CLOSE:							// �յ�Close��Ϣ?
		{
			PostQuitMessage(0);					// �����˳���Ϣ
			return 0;						// ����
		}
	case WM_KEYDOWN:						// �м�����ô?
		{
			keys[wParam] = TRUE;					// ����ǣ���ΪTRUE
			return 0;						// ����
		}
	case WM_KEYUP:							// �м��ſ�ô?
		{
			keys[wParam] = FALSE;					// ����ǣ���ΪFALSE
			return 0;						// ����
		}
	case WM_SIZE:							// ����OpenGL���ڴ�С
		{
			MIDDLEX=(LOWORD(lParam))>>1;
			MIDDLEY=(HIWORD(lParam))>>1;
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));		// LoWord=Width,HiWord=Height
			
			return 0;						// ����
		}	
	case WM_MOUSEMOVE:
		{
			//x=LOWORD(lParam);
			//y=HIWORD(lParam);
			
		}
	case WM_MOUSEWHEEL:
		{
			MouseWheel=(short)HIWORD(wParam);
			//if(MouseWheel<0 && z>-45)
				//z-=1.0f;
			//else if(MouseWheel>0 && z< -3)
				//z+=1.0f;
		}
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,				// ��ǰ����ʵ��
				   HINSTANCE	hPrevInstance,				// ǰһ������ʵ��
				   LPSTR		lpCmdLine,				// �����в���
				   int		nCmdShow)				// ������ʾ״̬
{
	MSG	msg;								// Windowsx��Ϣ�ṹ
	BOOL	done=FALSE;							// �����˳�ѭ����Bool ����
	if (MessageBox(NULL,"������ȫ��ģʽ������ô��", "����ȫ��ģʽ",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;						// FALSEΪ����ģʽ
	}
	if (!CreateGLWindow("OpenGL������",MAXSIZEX,MAXSIZEY,16,fullscreen))
	{
		return 0;							// ʧ���˳�
	}
	while(!done)								// ����ѭ��ֱ�� done=TRUE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))			// ����Ϣ�ڵȴ���?
		{
			if (msg.message==WM_QUIT)				// �յ��˳���Ϣ?
			{
				done=TRUE;					// �ǣ���done=TRUE
			}
			else							// ���ǣ���������Ϣ
			{
				TranslateMessage(&msg);				// ������Ϣ
				DispatchMessage(&msg);				// ������Ϣ
			}
		}
		else								// ���û����Ϣ
		{
			// ���Ƴ���������ESC��������DrawGLScene()���˳���Ϣ
			if (active)						// ���򼤻��ô?
			{
				if (keys[VK_ESCAPE])				// ESC ������ô?
				{
					done=TRUE;				// ESC �����˳��ź�
				}
				else						// �����˳���ʱ��ˢ����Ļ
				{
					DrawGLScene();				// ���Ƴ���
					SwapBuffers(hDC);			// �������� (˫����)

				}
			}
			if (keys[VK_F1])					// F1��������ô?
			{
				keys[VK_F1]=FALSE;				// ���ǣ�ʹ��Ӧ��Key�����е�ֵΪ FALSE
				KillGLWindow();					// ���ٵ�ǰ�Ĵ���
				fullscreen=!fullscreen;				// �л� ȫ�� / ���� ģʽ
				// �ؽ� OpenGL ����
				if (!CreateGLWindow("Angluca's test",MAXSIZEX,MAXSIZEY,16,fullscreen))
				{
					return 0;				// �������δ�ܴ����������˳�
				}
			}
		}
	}
	// �رճ���
	KillGLWindow();								// ���ٴ���
	return (msg.wParam);							// �˳�����
}



