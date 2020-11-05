/**
 *  @file	ei_buttonclass.c
 *  @brief	allocfunc, releasefunc, drawfunc, setdefaultsfunc, geomnotifyfunc of button class.
 *
 */

#include "ei_buttonclass.h"
#include "ei_geometrymanager.h"
#include "ei_application.h"
#include "ei_draw_more.h"
#include "ei_calculations.h"
#include "ei_widget.h"

/* Currently pressed button, or NULL if there is no button pressed. */
ei_button_t *button_pressed = NULL;
/* Is the mouse cursor in front of the currently pressed button */
ei_bool_t pressing_over = EI_FALSE;


void* allocbutton(void) {
	/* Dynamically allocates memory for a button */
	ei_button_t* widget_button = calloc(1, sizeof(ei_button_t));
	return widget_button;
}

void releasebutton(struct ei_widget_t* widget) {
	ei_button_t* widget_button = (ei_button_t*)widget;
	
	/* Frees the memory of the general attributes */
	/* The widget itself is *not* destroyed by this function */
	if(widget_button->border_width)
		free(widget_button->border_width);
	if(widget_button->corner_radius)
		free(widget_button->corner_radius);
	if(widget_button->relief)
		free(widget_button->relief);
	if(widget_button->color)
		free(widget_button->color);
	if(widget->content_rect && widget->content_rect != &widget->screen_location)
		free(widget->content_rect);

	/* Frees text related stuffs */
	if(widget_button->text)
		free(widget_button->text);
	if(widget_button->text_font)
		free(widget_button->text_font);
	if(widget_button->text_color)
		free(widget_button->text_color);
	if(widget_button->text_anchor)
		free(widget_button->text_anchor);

	/* Frees img related stuffs */
	if(widget_button->img){
		hw_surface_free(*(widget_button->img));
		free(widget_button->img);
	}
	if(widget_button->img_rect)
		free(widget_button->img_rect);
	if(widget_button->img_anchor)
		free(widget_button->img_anchor);
	if(widget_button->callback)
		free(widget_button->callback);
	if(widget_button->user_param)
		free(widget_button->user_param);

}

void drawbutton(struct ei_widget_t*		widget,
					  ei_surface_t		surface,
					  ei_surface_t		pick_surface,
					  ei_rect_t*		clipper) {
	drawframebuttonclasses(widget, surface, pick_surface, clipper, EI_FALSE);
}

void setdefaultsbutton(struct ei_widget_t* widget) {
	/* casting the widget (pointer) into a button (pointer) */
	ei_button_t * widget_button;
	widget_button = (ei_button_t *)widget;

	/* configures the default color, relief and border width */
	if(widget_button->border_width == NULL)
		widget_button->border_width = malloc(sizeof(int));
	*(widget_button->border_width) = k_default_button_border_width;
	widget->content_rect = &widget->screen_location;

	if(widget_button->corner_radius == NULL)
		widget_button->corner_radius = malloc(sizeof(int));
	*(widget_button->corner_radius) = k_default_button_corner_radius;

	if(widget_button->color == NULL)
		widget_button->color = malloc(sizeof(ei_color_t));
	*(widget_button->color) = ei_default_background_color;

	if(widget_button->relief == NULL)
		widget_button->relief = malloc(sizeof(ei_relief_t));
	*(widget_button->relief) = ei_relief_raised;

	/* configures the default text attributes */
	if(widget_button->text)
		free(widget_button->text);
	widget_button->text = NULL;
	
	if(widget_button->text_font == NULL)
		widget_button->text_font = malloc(sizeof(ei_font_t));
	*(widget_button->text_font) = ei_default_font;

	if(widget_button->text_color == NULL)
		widget_button->text_color = malloc(sizeof(ei_color_t));
	*(widget_button->text_color) = ei_font_default_color;

	if(widget_button->text_anchor == NULL)
		widget_button->text_anchor = malloc(sizeof(ei_anchor_t));
	*(widget_button->text_anchor) = ei_anc_center;

	/**  
	 * configures the default image attributes
	 * even if the default image pointer is NULL, the image anchor
	 * is initialised so that it does not have to be defined later 
	 */
	if(widget_button->img)
		free(widget_button->img);
	widget_button->img = NULL;
	if(widget_button->img_rect)
		free(widget_button->img_rect);
	widget_button->img_rect = NULL;

	if(widget_button->img_anchor == NULL)
		widget_button->img_anchor = malloc(sizeof(ei_anchor_t));
	*(widget_button->img_anchor) = ei_anc_center;

	/* special parameters */
	widget_button->no_clipping = EI_FALSE;
	widget_button->is_quit_button = EI_FALSE;
	widget_button->is_resize_button = EI_FALSE;
}

void geomnotifybutton(struct ei_widget_t* widget) {
	int *border_width = ((ei_button_t*)widget)->border_width;
	ei_rect_t old_rect = *widget->content_rect;
	if(border_width && *border_width) {
		if(widget->content_rect == &(widget->screen_location))
	        widget->content_rect = malloc(sizeof(ei_rect_t));
	    *widget->content_rect = (ei_rect_t){
	        {
	            widget->screen_location.top_left.x+(*border_width),
	            widget->screen_location.top_left.y+(*border_width)
	        },
	        {
	            widget->screen_location.size.width-2*(*border_width),
	            widget->screen_location.size.height-2*(*border_width)
	        }
	    };
	} else {
		widget->content_rect = &widget->screen_location;
	}

	ei_widget_t *sibling = widget->children_head;
	while(sibling) {
		if(sibling->geom_params)
			sibling->geom_params->manager->runfunc(sibling);
		sibling = sibling->next_sibling;
	}
	if(widget->content_rect)
		ei_app_invalidate_rect(widget->content_rect);
}


ei_bool_t ei_handle_button_down(ei_widget_t *widget, ei_event_t* event, void* user_param) {
	/* Precondition : there is a button behind the cursor */
	if(event->type != ei_ev_mouse_buttondown || event->param.mouse.button != ei_mouse_button_left)
		return EI_FALSE;
	ei_button_t *widget_button = (ei_button_t*)ei_widget_pick(&event->param.mouse.where);
	if(!widget_button)
		return EI_FALSE;

	pressing_over = EI_TRUE;
	button_pressed = widget_button;
	*widget_button->relief = ei_relief_sunken;
	ei_app_invalidate_rect(&widget_button->widget.screen_location);
	return EI_FALSE;
}

ei_bool_t ei_handle_button_up(ei_widget_t *widget, ei_event_t* event, void* user_param) {
	if(event->type != ei_ev_mouse_buttonup || event->param.mouse.button != ei_mouse_button_left)
		return EI_FALSE;
	ei_button_t *widget_button = (ei_button_t*)ei_widget_pick(&event->param.mouse.where);
	ei_button_t *old_button_pressed = button_pressed;
	button_pressed = NULL;
	if(!widget_button || old_button_pressed != widget_button)
		return EI_FALSE;

	pressing_over = EI_FALSE;
	*widget_button->relief = ei_relief_raised;
	ei_app_invalidate_rect(&widget_button->widget.screen_location);
	if(widget_button->callback) {
		ei_callback_t callback = *widget_button->callback;
		callback((ei_widget_t*)widget_button, event, widget_button->user_param ? *widget_button->user_param : NULL);
	}
	return EI_FALSE;
}

ei_bool_t ei_handle_button_move(ei_widget_t *widget, ei_event_t* event, void* user_param) {
	if(!button_pressed || event->type != ei_ev_mouse_move || !event)
		return EI_FALSE;
	ei_widget_t *widget_picked = ei_widget_pick(&event->param.mouse.where);

	if(widget_picked != (ei_widget_t*)button_pressed && *button_pressed->relief == ei_relief_sunken) {
		*button_pressed->relief = ei_relief_raised;
		pressing_over = EI_FALSE;
		ei_app_invalidate_rect(&button_pressed->widget.screen_location);
	} else if(widget_picked == (ei_widget_t*)button_pressed && *button_pressed->relief == ei_relief_raised) {
		*button_pressed->relief = ei_relief_sunken;
		pressing_over = EI_TRUE;
		ei_app_invalidate_rect(&button_pressed->widget.screen_location);
	}
	return EI_FALSE;
}
