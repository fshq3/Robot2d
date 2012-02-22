#ifndef ROBOT2D_GUI_H_INCLUDED
#define ROBOT2D_GUI_H_INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <box2d/box2d.h>
#include <Robot2d/Render.h>
using namespace std;
/*
 * SDL OpenGL Tutorial.
 * (c) Michael Vance, 2000
 * briareos@lokigames.com
 *
 * Distributed under terms of the LGPL.
 */
static GLboolean should_rotate = GL_TRUE;
class robot2d_world
{
public:
	b2World *m_world;
	c_b2draw drawer;
	void create()
	{
		// Define the gravity vector.
		b2Vec2 gravity(0.0f, -10.0f);

		// Construct a world object, which will hold and simulate the rigid bodies.
		m_world = new b2World(gravity);

		// Define the ground body.
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0.0f, -10.0f);

		// Call the body factory which allocates memory for the ground body
		// from a pool and creates the ground box shape (also from a pool).
		// The body is also added to the world.
		b2Body* groundBody = m_world->CreateBody(&groundBodyDef);

		// Define the ground box shape.
		b2PolygonShape groundBox;

		// The extents are the half-widths of the box.
		groundBox.SetAsBox(50.0f, 10.0f);

		// Add the ground fixture to the ground body.
		groundBody->CreateFixture(&groundBox, 0.0f);

		// Define the dynamic body. We set its position and call the body factory.
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(0.0f, 4.0f);
		b2Body* body = m_world->CreateBody(&bodyDef);

		// Define another box shape for our dynamic body.
		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(1.0f, 1.0f);

		// Define the dynamic body fixture.
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;

		// Set the box density to be non-zero, so it will be dynamic.
		fixtureDef.density = 1.0f;

		// Override the default friction.
		fixtureDef.friction = 0.3f;

		// Add the shape to the body.
		body->CreateFixture(&fixtureDef);

	}
	void step()
	{
		if(m_world==NULL)
			return;
		// Prepare for simulation. Typically we use a time step of 1/60 of a
		// second (60Hz) and 10 iterations. This provides a high quality simulation
		// in most game scenarios.
		float32 timeStep = 1.0f / 60.0f;
		int32 velocityIterations = 6;
		int32 positionIterations = 2;

		// Instruct the world to perform a single step of simulation.
		// It is generally best to keep the time step and iterations fixed.
		m_world->Step(timeStep, velocityIterations, positionIterations);

	}
	void draw()
	{
		if(m_world==NULL)
			return;
		drawer.DrawWorld(m_world);
	}

};

class c_robot2d_gui
{
public:
	/* Information about the current video settings. */
	const SDL_VideoInfo* info;
	robot2d_world world;
	void run()
	{
		/* Dimensions of our window. */
		int width = 0;
		int height = 0;
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

		/*
		 * Set our width/height to 640/480 (you would
		 * of course let the user decide this in a normal
		 * app). We get the bpp we will request from
		 * the display. On X11, VidMode can't change
		 * resolution, so this is probably being overly
		 * safe. Under Win32, ChangeDisplaySettings
		 * can change the bpp.
		 */
		width = 640;
		height = 480;
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
		/*
		 * Now we want to begin our normal app process--
		 * an event loop with a lot of redrawing.
		 */
		while( 1 ) {
			/* Process incoming events. */
			process_events( );
			world.step();
			/* Draw the screen. */
			draw_screen( );
		}

		/*
		 * EXERCISE:
		 * Record timings using SDL_GetTicks() and
		 * and print out frames per second at program
		 * end.
		 */

		/* Never reached. */
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

	static void handle_key_down( SDL_keysym* keysym )
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

	static void process_events( void )
	{
		/* Our SDL event placeholder. */
		SDL_Event event;

		/* Grab all the events off the queue. */
		while( SDL_PollEvent( &event ) ) {

			switch( event.type ) {
			case SDL_KEYDOWN:
				/* Handle key presses. */
				handle_key_down( &event.key.keysym );
				break;
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
