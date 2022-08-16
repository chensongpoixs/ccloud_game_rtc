// Attempt to identify a window by name or attribute.
// by Adam Pierce <adam@doctort.org>
 
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <iostream>
#include <list>
#include <stdlib.h>
 
using namespace std;
 
class WindowsMatchingPid
{
public:
    WindowsMatchingPid(Display *display, Window wRoot, unsigned long pid)
    	: _display(display)
    	, _pid(pid)
    {
    // Get the PID property atom.
    	_atomPID = XInternAtom(display, "_NET_WM_PID", True);
    	if(_atomPID == None)
    	{
    		cout << "No such atom" << endl;
    		return;
    	}
 
    	search(wRoot);
    }
 
    const list<Window> &result() const { return _result; }
 
private:
    unsigned long  _pid;
    Atom           _atomPID;
    Display       *_display;
    list<Window>   _result;
 
    void search(Window w)
    {
    // Get the PID for the current Window.
    	Atom           type;
    	int            format;
    	unsigned long  nItems;
    	unsigned long  bytesAfter;
    	unsigned char *propPID = 0;
    	if(Success == XGetWindowProperty(_display, w, _atomPID, 0, 1, False, XA_CARDINAL,
    	                                 &type, &format, &nItems, &bytesAfter, &propPID))
    	{
    		if(propPID != 0)
    		{
    		// If the PID matches, add this window to the result set.
    			if(_pid == *((unsigned long *)propPID))
    				_result.push_back(w);
 
    			XFree(propPID);
    		}
    	}
 
    // Recurse into child windows.
    	Window    wRoot;
    	Window    wParent;
    	Window   *wChild;
    	unsigned  nChildren;
    	if(0 != XQueryTree(_display, w, &wRoot, &wParent, &wChild, &nChildren))
    	{
    		for(unsigned i = 0; i < nChildren; i++)
    			search(wChild[i]);
    	}
    }
};
 #include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>

#include <iostream>
#include <list>
#include <vector>

/* Find X Window From PID
*
* Reference:
*  http://stackoverflow.com/questions/151407/how-to-get-an-x11-window-from-a-process-id
*/
class XWindowFinder {
public:
  XWindowFinder(unsigned long pid, Display *display=NULL): _display(display), _pid(pid) {
    if(_display==NULL) _display = XOpenDisplay(NULL);

    _atomPID = XInternAtom(_display, "_NET_WM_PID", True);
    if(_atomPID == None) {
      std::cout << "No such atom" << std::endl;
      return;
    }

    Window wRoot = XDefaultRootWindow(_display);
    this->search(wRoot);
  }

  const std::vector<Window> &result() const { return _result; }

  void print(){
    printf("[%s][%d]\n", __FUNCTION__, __LINE__);
    for(std::vector<Window>::const_iterator it=_result.begin(); it !=_result.end(); it++)
      std::cout << "Window #" << (unsigned long)(*it) << std::endl;
  }

private:
    unsigned long  _pid;
    Atom           _atomPID;
    Display       *_display;
    std::vector<Window>   _result;

    unsigned long getPid(Window w){
    Atom           type;
    int            format;
    unsigned long  nItems;
    unsigned long  bytesAfter;
    unsigned char *propPID = 0;

    if(Success == XGetWindowProperty(_display, w, _atomPID, 0, 1, False, XA_CARDINAL,
                                     &type, &format, &nItems, &bytesAfter, &propPID))
    {
        if(propPID != 0) {
              unsigned long pid = *((unsigned long *)propPID);
          XFree(propPID);
              return pid;
            }
        }
        return 0;
    }

    bool search(Window w) {
      bool found = false;

      unsigned long propPID = this->getPid(w);
      if(_pid == propPID){
        _result.push_back(w);
        return true;
      }

      // Recurse into child windows.
      Window    wRoot;
      Window    wParent;
      Window   *wChild;
      unsigned  nChildren;
      if(0 != XQueryTree(_display, w, &wRoot, &wParent, &wChild, &nChildren)) {
        for(unsigned i = 0; i < nChildren; i++){
          found = search(wChild[i]) || found;
          // found = found || search(wChild[i]);
          //if(found) break;
        }
      }
      return found;
    }
};

int main(int argc, char **argv){

  // if(argc>=2){
    int pid = atoi(argv[1]);
    XWindowFinder finder(pid);
    finder.print();
    return 0;
  // }
  return 0;
}
int test_ddmain(int argc, char **argv)
{
    if(argc < 2)
    	return 1;
 
    int pid = atoi(argv[1]);
    cout << "Searching for windows associated with PID " << pid << endl;
 
// Start with the root window.
    Display *display = XOpenDisplay(0);
 
    WindowsMatchingPid match(display, XDefaultRootWindow(display), pid);
 
// Print the result.
    const list<Window> &result = match.result();
    //for(list<Window>::const_iterator it = result.begin(); it != result.end(); it++)
    	//cout << "Window #" << (unsigned long)(*it) << endl;
    cout << "Window id: "<< (unsigned long)(*result.begin()) << endl;
 
    return 0;
}
 
//编译：g++ -o xx this.cpp -lX11