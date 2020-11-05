/**
 *  @file	ei_toplevelclass.c
 *  @brief	allocfunc, releasefunc, drawfunc, setdefaultsfunc, geomnotifyfunc of toplevel class.
 *
 */

#include "ei_toplevelclass.h"
#include "ei_geometrymanager.h"
#include "ei_widget.h"
#include "ei_application.h"
#include "ei_draw_more.h"
#include "ei_calculations.h"


static ei_toplevel_t *resizing_toplevel = NULL;
static ei_toplevel_t *moving_toplevel = NULL;
static uint32_t offsetptr[2];


ei_bool_t quit_toplevel_event(ei_widget_t* w_quitbutton, ei_event_t* event, void* params) {
	/* the quit button's parent is the toplevel to quit */
	ei_toplevel_t* toplevel = (ei_toplevel_t *) w_quitbutton->parent;
	/* invalidates the surface the toplevel occupied */
	ei_app_invalidate_rect(toplevel->draw_rect);
	if(*toplevel->closable){
		ei_widget_destroy((ei_widget_t*) toplevel); 
		return EI_TRUE;
	}
	/* if the toplevel is not closable, we tell that the event did not occur */
	return EI_FALSE;
}


ei_bool_t resize(ei_widget_t* w_resizebutton, ei_event_t* event, void* params){
	if(!resizing_toplevel)
		return EI_FALSE;

	/* the parameters need to have two fields at least : x and y offsets */
	uint32_t offsetx = offsetptr[0];
	uint32_t offsety = offsetptr[1];
	
	/* the size of the toplevel (! the mouse is not exactly at the bottom right corner : needs offset !) */
	int n_width = event->param.mouse.where.x - resizing_toplevel->widget.screen_location.top_left.x + offsetx;
	int n_height = event->param.mouse.where.y - resizing_toplevel->widget.screen_location.top_left.y + offsety;

	ei_bool_t redraw = EI_FALSE;
	/* resizes according to the resizable axes */
	switch(*resizing_toplevel->resizable){
		case ei_axis_both:
		case ei_axis_x:
			if(n_width >= resizing_toplevel->min_size->width){
				/* sets the new width if superior to the minimal width */
				resizing_toplevel->widget.requested_size.width = n_width;
				redraw = EI_TRUE;
			} else
				n_width = resizing_toplevel->widget.requested_size.width;
			if(*resizing_toplevel->resizable == ei_axis_x)
				break;
		case ei_axis_y:
			if(n_height >= resizing_toplevel->min_size->height){
				/* sets the next height if superior to the minimal height */
				resizing_toplevel->widget.requested_size.height = n_height;
				redraw = EI_TRUE;
			} else
				n_height = resizing_toplevel->widget.requested_size.height;
		default:
			break;	
	}

	/* redraws if necessary */
	float nll = 0.0;
	if(redraw) /* updates the toplevel geometry parameters */
		ei_place((ei_widget_t*)resizing_toplevel, NULL, NULL, NULL, &n_width, &n_height, NULL, NULL, &nll, &nll);

	return EI_FALSE;
}


ei_bool_t end_resize(ei_widget_t* w_resizebutton, ei_event_t* event, void* params){
	resizing_toplevel = NULL;
	return EI_FALSE;
}


ei_bool_t beg_resize(ei_widget_t* w_resizebutton, ei_event_t* event, void* params){
	if(event->type != ei_ev_mouse_buttondown || event->param.mouse.button != ei_mouse_button_left)
		return EI_FALSE;

	/* the resize button's parent is the toplevel to resize */
	ei_toplevel_t *toplevel = (ei_toplevel_t *) w_resizebutton->parent;

	/* stops if the toplevel is not resizable */
	if(*toplevel->resizable == ei_axis_none)
		return EI_FALSE;

	resizing_toplevel = toplevel;

	/* the distance between the mouse and the right and bottom sides */
	offsetptr[0] = w_resizebutton->screen_location.top_left.x + w_resizebutton->screen_location.size.width - event->param.mouse.where.x - (*toplevel->border_width);
	offsetptr[1] = w_resizebutton->screen_location.top_left.y + w_resizebutton->screen_location.size.height - event->param.mouse.where.y - default_topbar_height;
	return EI_TRUE;
}


ei_bool_t move(ei_widget_t* w_toplevel, ei_event_t* event, void* params){
	if(!moving_toplevel)
		return EI_FALSE;

	/* the distance between the mouse and the right and bottom sides */
	uint32_t offset_x = offsetptr[0];
	uint32_t offset_y = offsetptr[1];

	/* the new position of the toplevel */
	uint32_t n_x = event->param.mouse.where.x - offset_x - moving_toplevel->widget.parent->content_rect->top_left.x;
	uint32_t n_y = event->param.mouse.where.y - offset_y - moving_toplevel->widget.parent->content_rect->top_left.y;
	float nll = 0.0;
	ei_anchor_t anc = ei_anc_northwest;

	/* updating... */
	ei_place((ei_widget_t*)moving_toplevel, &anc, &n_x, &n_y, NULL, NULL, &nll, &nll, NULL, NULL);
	return EI_FALSE;
}


ei_bool_t end_move(ei_widget_t* w_toplevel, ei_event_t* event, void* params) {
	if(!moving_toplevel)
		return EI_FALSE;
	moving_toplevel = NULL;
	return EI_FALSE;
}


ei_bool_t beg_move(ei_widget_t* w_toplevel, ei_event_t* event, void* params) {
	ei_toplevel_t *toplevel = (ei_toplevel_t *) w_toplevel;

	/* Checking whether the user clicked on the topbar or not */
	if(event->param.mouse.where.y > w_toplevel->screen_location.top_left.y + default_topbar_height)
		return EI_FALSE;

	/* the distance between the mouse and the right and bottom sides */
	offsetptr[0] = event->param.mouse.where.x - w_toplevel->screen_location.top_left.x;
	offsetptr[1] = event->param.mouse.where.y - w_toplevel->screen_location.top_left.y;
	moving_toplevel = toplevel;
	return EI_FALSE;
}



void* alloctoplevel(void) {
	/* Dynamically allocates memory for a toplevel */
	ei_toplevel_t* widget_toplevel = calloc(1, sizeof(ei_toplevel_t));
	return widget_toplevel;
}


void releasetoplevel(struct ei_widget_t* widget) {
	/* The widget itself is *not* destroyed by this function */
	ei_toplevel_t* widget_toplevel = (ei_toplevel_t*)widget;

    /* frees the title attributes */
	if(widget_toplevel->title)
        free(widget_toplevel->title);
    if(widget_toplevel->title_font)
        free(widget_toplevel->title_font);
    if(widget_toplevel->border_width)
        free(widget_toplevel->border_width);
	if(widget_toplevel->background_color)
        free(widget_toplevel->background_color);
    if(widget->content_rect && widget->content_rect != &widget->screen_location)
		free(widget->content_rect);

    /* frees the toplevel special attributes */
    if(widget_toplevel->closable)
        free(widget_toplevel->closable);
    if(widget_toplevel->resizable)
        free(widget_toplevel->resizable);
    if(widget_toplevel->min_size)
        free(widget_toplevel->min_size);
    if(widget_toplevel->draw_rect)
    	free(widget_toplevel->draw_rect);
}


void drawtoplevel(struct ei_widget_t*		widget,
					  ei_surface_t		surface,
					  ei_surface_t		pick_surface,
					  ei_rect_t*		clipper) {
	ei_toplevel_t * widget_toplevel = (ei_toplevel_t *) widget;

	int corner_radius = 0;
    int *border_width = widget_toplevel->border_width;
    ei_color_t *color = widget_toplevel->background_color;
    const char *text = *widget_toplevel->title;
    ei_font_t *text_font = widget_toplevel->title_font;


    ei_rect_t all_clipper = get_ei_rect_intersection(*clipper, *widget_toplevel->draw_rect);
    ei_rect_t content_clipper = get_ei_rect_intersection(*clipper, *widget->content_rect);

    /* Drawing... */
    ei_color_t border_color = {50, 50, 50, 255};
    ei_fill(surface, &border_color, &all_clipper);
	ei_fill(surface, widget_toplevel->background_color, &content_clipper);

    /* filling the pick_surface with the widget's pick_color */
    ei_fill(pick_surface, widget->pick_color, &all_clipper);

    if(text) {
        /* computing text width and text height */
        ei_point_t where = {widget_toplevel->draw_rect->top_left.x+25+*widget_toplevel->border_width, widget_toplevel->draw_rect->top_left.y};
        const ei_color_t white = {255, 255, 255, 255};
        ei_draw_text(surface, &where, text, NULL, white, &all_clipper);
    }
}


void setdefaultstoplevel(struct ei_widget_t* widget) {
	/* casting the widget (pointer) into a toplevel (pointer) */
	ei_toplevel_t * widget_toplevel = (ei_toplevel_t *)widget;

	widget->content_rect = &widget->screen_location;

	/* configures the default title attributes */
	if(widget_toplevel->title)
		free(widget_toplevel->title);
	widget_toplevel->title = malloc(sizeof(char[9]));
	*widget_toplevel->title = "Toplevel";
	

	if(widget_toplevel->title_font == NULL)
		widget_toplevel->title_font = malloc(sizeof(ei_font_t));
	*(widget_toplevel->title_font) = ei_default_font;

	if(widget_toplevel->draw_rect == NULL)
		widget_toplevel->draw_rect = malloc(sizeof(ei_rect_t));

	if(widget_toplevel->border_width == NULL)
		widget_toplevel->border_width = malloc(sizeof(int));
	*(widget_toplevel->border_width) = 4;

	if(widget_toplevel->background_color == NULL)
		widget_toplevel->background_color = malloc(sizeof(ei_color_t));
	*(widget_toplevel->background_color) = ei_default_background_color;


	/* configures the default toplevel special attributes */
	if(widget_toplevel->closable == NULL)
		widget_toplevel->closable = malloc(sizeof(ei_bool_t));
	*(widget_toplevel->closable) = EI_TRUE;

	if(widget_toplevel->resizable == NULL)
		widget_toplevel->resizable = malloc(sizeof(ei_axis_set_t));
	*(widget_toplevel->resizable) = ei_axis_both;

    if(widget_toplevel->min_size == NULL)
		widget_toplevel->min_size = malloc(sizeof(ei_size_t));
	*(widget_toplevel->min_size) = (ei_size_t){160,120};


	/* places the quit button, the foreground and the resize button */
	if(*widget_toplevel->resizable) {
		ei_button_t* resize_button = (ei_button_t *) ei_widget_create("button", widget, NULL, NULL);
		ei_size_t resize_button_size = {20,20};
		ei_color_t resize_button_color = ei_default_background_color;
		int resize_button_corner_radius = 0;
		ei_relief_t resize_button_relief = ei_relief_none;
		ei_button_configure((ei_widget_t *) resize_button, &resize_button_size, &resize_button_color,
				    NULL, &resize_button_corner_radius, &resize_button_relief, NULL, NULL, NULL, NULL,
				    NULL, NULL, NULL, NULL, NULL);
		resize_button->is_resize_button = EI_TRUE;

		ei_anchor_t resize_button_anchor = ei_anc_southeast;
		float rel_x = 1.0;
		float rel_y = 1.0;
		int x = 0;
		int y = 0;
		/* the resize button is at the bottom right corner */
		ei_place((ei_widget_t *) resize_button, &resize_button_anchor, &x, &y, NULL, NULL, &rel_x, &rel_y, NULL, NULL);
		
		ei_bind(ei_ev_mouse_buttondown, (ei_widget_t *) resize_button, NULL, beg_resize, NULL);
	}

	ei_bind(ei_ev_mouse_buttondown, widget, NULL, beg_move, NULL);

	/* configures the quit button */
	if(*widget_toplevel->closable) {
		ei_button_t* quit_button = (ei_button_t *) ei_widget_create("button", widget, NULL, NULL);
		ei_size_t quit_button_size = {10,10};
		ei_color_t quit_button_color = {0xFF,0,0,0xFF};
		int quit_button_corner_radius = 1;
		ei_relief_t quit_button_relief = ei_relief_raised;
		ei_button_configure((ei_widget_t *) quit_button, &quit_button_size, &quit_button_color,
				    NULL, &quit_button_corner_radius, &quit_button_relief, NULL, NULL, NULL, NULL,
				    NULL, NULL, NULL, NULL, NULL);
		quit_button->no_clipping = EI_TRUE;
		quit_button->is_quit_button = EI_TRUE;

		int quit_button_x = 6;
		int quit_button_y = -default_topbar_height+9;
		/* the quit button is near the top left corner of the toplevel */
		ei_place((ei_widget_t *) quit_button, NULL, &quit_button_x, &quit_button_y, NULL, NULL, NULL, NULL, NULL, NULL);
		
		ei_bind(ei_ev_mouse_buttondown, (ei_widget_t *) quit_button, NULL, quit_toplevel_event, NULL);
	}

	widget->content_rect = &widget->screen_location;
}


void geomnotifytoplevel(struct ei_widget_t* widget) {

	ei_toplevel_t * widget_toplevel = (ei_toplevel_t *) widget;
	int *border_width = ((ei_toplevel_t*)widget)->border_width;
	ei_rect_t old_rect = *widget->content_rect;
	if(!widget->content_rect || widget->content_rect == &widget->screen_location)
		widget->content_rect = malloc(sizeof(ei_rect_t));
    *widget->content_rect = (ei_rect_t){
    	{
    		widget->screen_location.top_left.x+(*border_width),
    		widget->screen_location.top_left.y+default_topbar_height
    	},
    	widget->screen_location.size
    };

		/* drawing the widget with relief */
    if(!widget_toplevel->draw_rect)
    	widget_toplevel->draw_rect = malloc(sizeof(ei_rect_t));
    *widget_toplevel->draw_rect = (ei_rect_t){widget->screen_location.top_left,
        {
            widget->screen_location.size.width+2*(*border_width),
            widget->screen_location.size.height+(*border_width)+default_topbar_height
        }
    };

	ei_widget_t *sibling = widget->children_head;
	while(sibling) {
		if(sibling->geom_params)
			sibling->geom_params->manager->runfunc(sibling);
		sibling = sibling->next_sibling;
	}
	ei_rect_t inv_rect = extend_rect(*((ei_toplevel_t*)widget)->draw_rect);
	ei_app_invalidate_rect(&inv_rect);
}
