#ifndef __WINDOW_H
#define __WINDOW_H

#define MAXWINDOWS 100
#define MAXCONTROLS 100
#define MAXCLICKHANDLERS 100
#define MAXCOMBOBOXITEMS 50
#define MAXLISTBOXITEMS 1024
#define MAXMENUITEMS 15
#define MAXSUBMENUITEMS 15

class guiroot {
	public:
		void init();
		void DrawHorizLine(int x,
					   int y,
					   int width,
					   unsigned char r,
					   unsigned char g,
					   unsigned char b);
		void DrawDottedHorizLine(int x,
					   int y,
					   int width,
					   int x2,
					   int y2);
		void DrawVertiLine(int x,
					   int y,
					   int height,
					   unsigned char r,
					   unsigned char g,
					   unsigned char b);
					   
		void DrawDottedVertiLine(int x,
					   int y,
					   int height,
					   int x2,
					   int y2);
					   
        void Fillrect( int x,
						int y,
						int width,
						int height,
						unsigned char r,
						unsigned char g,
						unsigned char b);
					   					   
		void doUpdate();
		void drawWallpaper();
		void drawImage(int x, int y, int width, int height, unsigned char* buf);
};

class control {
	public:
		int x,y,width,height,height2,width2,heightoffset,widthoffset;
		virtual void Draw(int x, int y);
		virtual void onKeyPressed(char *s);		
		virtual void onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown);
		virtual void onMouseMove(int x, int y, int windowsx, int windowsy, int leftbuttondown);
		virtual void onRelease(int x, int y, int oldx, int oldy, int leftbuttondown);
		virtual void onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown);
		control();
		unsigned int handle;
		unsigned int windowHandle;
		unsigned char refresh;
};

class container:public control {
	public:
		virtual void Draw(int x, int y);
		virtual void onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown);
		virtual void onRelease(int x, int y, int oldx, int oldy, int leftbuttondown);
		void createcontainer(char *title1, int x1, int y1, int width1, int height1);
		char title[255];	
};

class button:public control {
	public:
		virtual void Draw(int x, int y);
		virtual void onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown);
		virtual void onRelease(int x, int y, int oldx, int oldy, int leftbuttondown);
		void createbutton(char *title1, int x1, int y1, int width1, int height1);
		char title[255];
		char buttondown;
};

class label:public control {
	public:
		virtual void Draw(int x, int y);
		void createlabel(char *title1, int x1, int y1);
		char message[8000];
};

class textbox:public control {
	public:
		virtual void Draw(int x, int y);
		virtual void onKeyPressed(char *s);
		void onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown);
		void createtextbox(char *title1, int x1, int y1, int width1, int height1);
		char text[8000];
		unsigned short drawOffset;
};

class combobox:public control {
	public:
		virtual void Draw(int x, int y);
		virtual void onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown);
		virtual void onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown);
		virtual void onMouseMove(int x, int y, int windowsx, int windowsy, int leftbuttondown);
		void createcombobox(char *item, int x1, int y1, int width1, int height1);
		char items[MAXCOMBOBOXITEMS][255];
		char topitem[255];
		unsigned short numItems;
		unsigned short selectedItem;
		unsigned short topItemID;
		char isOpened;
		void additem(char *item);
};

class listbox:public control {
	public:
		virtual void Draw(int x, int y);
		virtual void onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown);
		virtual void onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown);
		virtual void onMouseMove(int x, int y, int windowsx, int windowsy, int leftbuttondown);
		void createlistbox(char *item, int x1, int y1, int width1, int height1);
		void addItem(char *name);
		char items[MAXLISTBOXITEMS][255];
		unsigned short numItems;
		unsigned short selectedItem;
		unsigned int scrollbarSize;
		unsigned int topOffset;
		unsigned int firstItem;
};

class menu:public control {
	public:
		virtual void Draw(int x, int y);
		virtual void onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown);
		virtual void onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown);
		virtual void onMouseMove(int x1, int y1, int windowsx, int windowsy, int leftbuttondown);
		void createmenu(char *name, int x1, int y1, int width1, int height1);
		void addtopmenu();
		void additem(char *title, int i, int j);
		char items[MAXMENUITEMS][MAXSUBMENUITEMS][255];
		int itemsx[MAXMENUITEMS];
		unsigned short numItems1;
		unsigned short numItems2;
		unsigned char opened[MAXMENUITEMS];
		unsigned int mouseOverItem;
};

class taskbar:public control {
	public:
		virtual void Draw(int x, int y);
		virtual void onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown);
		virtual void onMouseOut(int x, int y, int oldx, int oldy, int leftbuttondown);
		virtual void onMouseMove(int x1, int y1, int windowsx, int windowsy, int leftbuttondown);
		void createtaskbar(char *name, int x1, int y1, int width1, int height1);
		unsigned char buttondown;
};

class window {
	public:
	void createwindow(char *title, int width, int height);
	void drawwindow();
	void onClick(int x, int y, int oldx, int oldy, int windowsx, int windowsy, int leftbuttondown);
	void onRelease(int x, int y, int oldx, int oldy, int leftbuttondown);
	void onMouseMove(int x1, int y1, int windowsx, int windowsy, int leftbuttondown);
	
	char title[255];
	int width, height, x, y, z;
	
	control *controls[MAXCONTROLS];
	int controlscount;
	char windowmoving;
	unsigned char invisible;	//$$$ for taskbar $$$
	int handle;
	unsigned char minimized;
};

class shell {
	public:
	void update();
	void updateWindow(int i);
	void updateAll();
	window *windows[MAXWINDOWS];
	int windowscount;
	unsigned int handleWithFocus;
	unsigned int windowWithFocus;
};

#endif
