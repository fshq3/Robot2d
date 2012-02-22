#include <Robot2d/robot2d_gui.h>
#include <box2d/box2d.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>


int main( int argc, char* argv[] )
{
	c_robot2d_gui gui;
	gui.run();
    return 0;
}
#ifdef __WIN32__
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	main( 0, 0 );
    return 0;
}
#endif
