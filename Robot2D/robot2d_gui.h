#ifndef ROBOT2D_GUI_H_INCLUDED
#define ROBOT2D_GUI_H_INCLUDED
#include <Robot2d/Robot2d.h>
#include <Robot2d/robot2d_world.h>

#include <Robot2d/event.h>
using namespace std;
/*
 * SDL OpenGL Tutorial.
 * (c) Michael Vance, 2000
 * briareos@lokigames.com
 *
 * Distributed under terms of the LGPL.
 */
static GLboolean should_rotate = GL_TRUE;

class c_robot2d_gui
{
public:
	/* Information about the current video settings. */
	const SDL_VideoInfo* info;
	c_robot2d_world world;
	/* Dimensions of our window. */
	int width ;
	int height ;
	c_robot2d_gui(int a_width=640,int a_height=480)
	{
				/*
		 * Set our width/height to 640/480 (you would
		 * of course let the user decide this in a normal
		 * app). We get the bpp we will request from
		 * the display. On X11, VidMode can't change
		 * resolution, so this is probably being overly
		 * safe. Under Win32, ChangeDisplaySettings
		 * can change the bpp.
		 */
		width = a_width;
		height = a_height;
	}
	void init_sdl()
	{
		/* Color depth in bits of our window. */
		int bpp = 0;
		/* Flags we will pass into SDL_SetVideoMode. */
		int flags = 0;

		/* First, initialize SDL's video subsystem. */
		if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
			/* Failed, exit. */
			fprintf( stderr, "Video initialization failed: %s\n",
				 SDL_GetError( ) );
			quit_tutorial( 1 );
		}

		/* Let's get some video information. */
		info = SDL_GetVideoInfo( );

		if( !info ) {
			/* This should probably never happen. */
			fprintf( stderr, "Video query failed: %s\n",
				 SDL_GetError( ) );
			quit_tutorial( 1 );
		}


		bpp = info->vfmt->BitsPerPixel;

		/*
		 * Now, we want to setup our requested
		 * window attributes for our OpenGL window.
		 * We want *at least* 5 bits of red, green
		 * and blue. We also want at least a 16-bit
		 * depth buffer.
		 *
		 * The last thing we do is request a double
		 * buffered window. '1' turns on double
		 * buffering, '0' turns it off.
		 *
		 * Note that we do not use SDL_DOUBLEBUF in
		 * the flags to SDL_SetVideoMode. That does
		 * not affect the GL attribute state, only
		 * the standard 2D blitting setup.
		 */
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

		/*
		 * We want to request that SDL provide us
		 * with an OpenGL window, in a fullscreen
		 * video mode.
		 *
		 * EXERCISE:
		 * Make starting windowed an option, and
		 * handle the resize events properly with
		 * glViewport.
		 */
		flags = SDL_OPENGL ;//| SDL_FULLSCREEN;

		/*
		 * Set the video mode
		 */
		if( SDL_SetVideoMode( width, height, bpp, flags ) == 0 ) {
			/*
			 * This could happen for a variety of reasons,
			 * including DISPLAY not being set, the specified
			 * resolution not being available, etc.
			 */
			fprintf( stderr, "Video mode set failed: %s\n",
				 SDL_GetError( ) );
			quit_tutorial( 1 );
		}

		/*
		 * At this point, we should have a properly setup
		 * double-buffered window for use with OpenGL.
		 */
		setup_opengl( width, height );

		world.create();
	}
	void loop()
	{
		/*
		 * Now we want to begin our normal app process--
		 * an event loop with a lot of redrawing.
		 */
		int tick_interval = 1000/60;
		next_time = SDL_GetTicks() + tick_interval;
		while( 1 ) {
			/* Process incoming events. */
			process_events( );
			world.step(tick_interval);
			/* Draw the screen. */
			draw_screen( );

			SDL_Delay(time_left());
			next_time += tick_interval;
		}

		/* Never reached. */
	}
	void run()
	{
		init_sdl();
		loop();
	}
	Uint32 next_time;
	Uint32 time_left(void)
	{
		Uint32 now;
		now = SDL_GetTicks();
		if(next_time <= now)
			return 0;
		else
			return next_time - now;
	}
	static void quit_tutorial( int code )
	{
		/*
		 * Quit SDL so we can release the fullscreen
		 * mode and restore the previous video settings,
		 * etc.
		 */
		SDL_Quit( );

		/* Exit program. */
		exit( code );
	}

	static void handle_key_down( const SDL_keysym* keysym )
	{

		/*
		 * We're only interested if 'Esc' has
		 * been presssed.
		 *
		 * EXERCISE:
		 * Handle the arrow keys and have that change the
		 * viewing position/angle.
		 */
		switch( keysym->sym ) {
		case SDLK_ESCAPE:
			quit_tutorial( 0 );
			break;
		case SDLK_SPACE:
			should_rotate = !should_rotate;
			break;
		default:
			break;
		}

	}

	void process_events( void )
	{
		/* Our SDL event placeholder. */
		SDL_Event event;
		class keydown:public c_event
		{
		public:
			Uint8 type()
			{
				return SDL_KEYDOWN;
			}
			void on_event(const SDL_Event &event)
			{
				c_robot2d_gui::handle_key_down( &event.key.keysym );
			}
		}kd;
		/* Grab all the events off the queue. */
		while( SDL_PollEvent( &event ) ) {
			c_event_contain::instance().on_event(event);
			switch( event.type ) {
			case SDL_QUIT:
				/* Handle quit requests (like Ctrl-c). */
				quit_tutorial( 0 );
				break;
			}

		}

	}

	void draw_screen( void )
	{

		/* Clear the color and depth buffers. */
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		/* We don't want to modify the projection matrix. */
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity( );

		/* Move down the z-axis. */
		glTranslatef( 0.0, 0.0, -200.0 );

		world.draw();
		/*
		 * Swap the buffers. This this tells the driver to
		 * render the next frame from the contents of the
		 * back-buffer, and to set all rendering operations
		 * to occur on what was the front-buffer.
		 *
		 * Double buffering prevents nasty visual tearing
		 * from the application drawing on areas of the
		 * screen that are being updated at the same time.
		 */
		SDL_GL_SwapBuffers( );
	}

	static void setup_opengl( int width, int height )
	{
		float ratio = (float) width / (float) height;

		/* Our shading model--Gouraud (smooth). */
		glShadeModel( GL_SMOOTH );

		/* Culling. */
		glCullFace( GL_BACK );
		glFrontFace( GL_CCW );
		glEnable( GL_CULL_FACE );

		/* Set the clear color. */
		glClearColor( 0, 0, 0, 0 );

		/* Setup our viewport. */
		glViewport( 0, 0, width, height );

		/*
		 * Change to the projection matrix and set
		 * our viewing volume.
		 */
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		/*
		 * EXERCISE:
		 * Replace this with a call to glFrustum.
		 */
		gluPerspective( 60.0, ratio, 1.0, 1024.0 );
	}
};

#endif // ROBOT2D_GUI_H_INCLUDED
