#include <stdio.h>
#include <stdlib.h>

#include "ei_application.h"
#include "ei_event.h"
#include "hw_interface.h"
#include "ei_widget.h"
#include "ei_geometrymanager.h"
#include "ei_draw_more.h"

#include "unistd.h"

ei_bool_t salut_func(ei_widget_t* widget, ei_event_t* event, void* params){
	printf("salut\n");
	return EI_FALSE;
}


ei_bool_t process_key(ei_widget_t* widget, ei_event_t* event, void* user_param)
{
	if (event->param.key.key_code == SDLK_ESCAPE) {
		ei_app_quit_request();
		return EI_TRUE;
	}
	
	return EI_FALSE;
}

/*
 * main --
 *
 *	Main function of the application.
 */
int main(int argc, char** argv)
{
	ei_size_t	screen_size		= {800, 800};
	ei_color_t	root_bgcol		= {0, 0, 0xFF, 0xFF};

	ei_widget_t*	frame;
	int		frame_x			= 100;
	int		frame_y			= 200;
	ei_size_t	frame_size		= {200,300};
	ei_color_t	frame_color		= {0, 255, 0, 255};

	ei_widget_t* 	frame2;
	int		frame_x2		= 200;
	int		frame_y2		= 200;
	ei_size_t	frame_size2		= {400,100};
	ei_color_t	frame_color2		= {255, 0, 0, 255};

	ei_widget_t*	frame3;
	float		frame_relx		= 0.5;
	float		frame_rely		= 0.5;
	float		frame_relw		= 0.2;
	float		frame_relh		= 0.2;
	
	
	ei_relief_t	frame_relief		= ei_relief_raised;
	int		frame_border_width	= 8;

	/* Create the application and change the color of the background. */
	ei_app_create(screen_size, EI_FALSE);
	ei_frame_configure(ei_app_root_widget(), NULL, &root_bgcol, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	/* Create, configure and place the frame on screen. */
	frame3 = ei_widget_create("frame", ei_app_root_widget(), NULL, NULL);
	frame2 = ei_widget_create("frame", ei_app_root_widget(), NULL, NULL);
	frame = ei_widget_create("button", ei_app_root_widget(), NULL, NULL);

	//ei_surface_t image = hw_image_load("misc/klimt.jpg", ei_app_root_surface());
	char *text = calloc(40, 1);
	strcpy(text, "Yes les djeunz et les bogos !");
	ei_button_configure(frame, &frame_size, &frame_color,
			    &frame_border_width, NULL, &frame_relief, &text, NULL, NULL, NULL,
			    NULL, NULL, NULL, NULL, NULL);
	ei_frame_configure(frame2, &frame_size2, &frame_color2,
			    &frame_border_width, &frame_relief, NULL, NULL, NULL, NULL,
			    NULL, NULL, NULL);
	ei_frame_configure(frame3, NULL, &frame_color2,
			    &frame_border_width, &frame_relief, NULL, NULL, NULL, NULL,
			    NULL, NULL, NULL);
	ei_place(frame, NULL, &frame_x, &frame_y, NULL, NULL, NULL, NULL, NULL, NULL );
	ei_place(frame2, NULL, &frame_x2, &frame_y2, NULL, NULL, NULL, NULL, NULL, NULL );
	ei_place(frame3, NULL, NULL, NULL, NULL, NULL, &frame_relx, &frame_rely, &frame_relw, &frame_relh);

	ei_bind(ei_ev_mouse_buttondown, NULL, "button", salut_func, NULL);
	ei_bind(ei_ev_keydown,		NULL, "all", process_key, NULL);

	/* Run the application's main loop. */
	ei_app_run();

	free(text);
	ei_unbind(ei_ev_mouse_buttondown, NULL, "all", salut_func, NULL);

	/* We just exited from the main loop. Terminate the application (cleanup). */
	ei_app_free();

	return (EXIT_SUCCESS);
}
