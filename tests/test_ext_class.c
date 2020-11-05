//
//  test_ext_class.c
//  projet_c_IG
//
//  Created by François Bérard on 1/6/16.
//  Copyright © 2016 LIG-IIHM. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "ext_testclass.h"

#include "ei_utils.h"
#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"


static int		g_margin		= 8;

// process_key --

ei_bool_t process_key(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
	SDL_Keycode	key_code = event->param.key.key_code;

	if (key_code == SDLK_ESCAPE) {
		ei_app_quit_request();
		return EI_TRUE;

	} else if ((key_code == SDLK_UP) || (key_code == SDLK_DOWN)) {

		// increases/decreases margin

		g_margin += (key_code == SDLK_UP) ? 1 : -1;
		if (g_margin < 1)
			g_margin = 1;

		ext_testclass_configure((ei_widget_t*)user_param, g_margin);
		return EI_TRUE;
	}
        return EI_FALSE;
}


// handle_mouse_margin --

ei_bool_t handle_mouse_margin	(ei_widget_t*		widget,
				 struct ei_event_t*	event,
				 void*			user_param)
{
	static ei_bool_t		dragging		= EI_FALSE;
	static int 			initial_y;
	static int 			initial_margin;

	if ((event->type == ei_ev_mouse_buttondown) && (event->param.mouse.button == ei_mouse_button_left)) {
		dragging		= EI_TRUE;
		initial_y		= event->param.mouse.where.y;
		initial_margin		= g_margin;

	} else if ((event->type == ei_ev_mouse_buttonup) && (event->param.mouse.button == ei_mouse_button_left))
		dragging		= EI_FALSE;

	else if ((event->type == ei_ev_mouse_move) && dragging) {
		g_margin		= initial_margin + event->param.mouse.where.y - initial_y;
		if (g_margin < 1)
			g_margin	= 1;
		ext_testclass_configure(widget, g_margin);
	}

	return EI_FALSE;
}



// Uncomment either one of the two following lines.
//	If true, the testclass drawfunc draws a widget *and its children*.
//	If false, the testclass drawfunc only draws the widget, not its children.

 static const ei_bool_t	k_drawfunc_draws_children	= EI_TRUE;
// static const ei_bool_t	k_drawfunc_draws_children	= EI_FALSE;



// main --

int main(int argc, char** argv)
{
	ei_size_t	screen_size		= {800, 800};
        ei_color_t	root_bgcol		= {0x52, 0x7f, 0xb4, 0xff};
	ei_anchor_t	anchor_c		= ei_anc_center;
	float		fl_1_0			= 1.0;
	float		fl_0_5			= 0.5;
	
        ei_widget_t*	top;
        ei_size_t	top_size		= {400,400};
        char*		top_title		= "Test nouvelle classe de widget";
        ei_color_t	top_color		= {0xA0,0xA0,0xA0, 0xff};
        int		top_border_width	= 2;
        ei_bool_t	top_closable		= EI_TRUE;
        ei_axis_set_t	top_resizable		= ei_axis_both;
        ei_point_t	top_position		= {30, 10};
        ei_size_t	top_min_size		= {50, 50};
        ei_size_t*	top_min_size_ptr	= &top_min_size;

        ei_widget_t*	test;
	ei_size_t	test_sz;

	ei_widget_t*	but;
	char*		but_text		= "Click me !";

	// Initialize the application, set the background color.

	ei_app_create(screen_size, EI_FALSE);
        ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);        

	// Register the new "testclass" widget class.

	ext_testclass_register_class(k_drawfunc_draws_children); // See k_drawfunc_draws_children comment above.

	// Create a toplevel, place it on the screen.

        top = ei_widget_create("toplevel", ei_app_root_widget(), NULL, NULL);
        ei_toplevel_configure(top, &top_size, &top_color, &top_border_width, &top_title,
								&top_closable, &top_resizable, &top_min_size_ptr);
        ei_place(top, NULL, &(top_position.x), &(top_position.y), NULL, NULL, NULL, NULL, NULL, NULL);

	// Create the testclass widget as a child of the toplevel, use relative placement.

	test = ei_widget_create("testclass", top, NULL, NULL);
	ext_testclass_configure(test, g_margin);
	test_sz		= ei_size(-40, -40);
	ei_place(test, &anchor_c, NULL, NULL, &(test_sz.width), &(test_sz.height), &fl_0_5, &fl_0_5, &fl_1_0, &fl_1_0);

	// Check that picking works: do a mouse binding to control the margin size.

	ei_bind(ei_ev_mouse_move, test, NULL, handle_mouse_margin, NULL);
	ei_bind(ei_ev_mouse_buttondown, test, NULL, handle_mouse_margin, NULL);
	ei_bind(ei_ev_mouse_buttonup, test, NULL, handle_mouse_margin, NULL);

	// Create a button as a child of the testclass widget, place it.

	but = ei_widget_create("button", test, NULL, NULL);
	ei_button_configure(but, NULL, NULL, NULL, NULL, NULL, &but_text, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	ei_place(but, &anchor_c, NULL, NULL, NULL, NULL, &fl_1_0, &fl_0_5, NULL, NULL);

	// Run the main loop, bind the "Esc" key to quitting the application.

	ei_bind(ei_ev_keydown, NULL, "all", process_key, test);
        ei_app_run();
	ei_unbind(ei_ev_keydown, NULL, "all", process_key, test);

	ei_app_free();
	return (EXIT_SUCCESS);
}


