/**
 *  @file	ei_frameclass.c
 *  @brief	allocfunc, releasefunc, drawfunc, setdefaultsfunc, geomnotifyfunc of frame class.
 *
 */

#include "ei_frameclass.h"
#include "ei_geometrymanager.h"
#include "ei_widget.h"
#include "ei_draw_more.h"
#include "ei_application.h"
#include "ei_calculations.h"


void* allocframe(void) {
	/* Dynamically allocates memory for a frame */
	ei_frame_t* widget_frame = calloc(1, sizeof(ei_frame_t));
	return widget_frame;
}

void releaseframe(struct ei_widget_t* widget) {
	ei_frame_t* widget_frame = (ei_frame_t *)widget;
	
	/* Frees the memory of the general attributes */
	/* The widget itself is *not* destroyed by this function */
	if(widget_frame->border_width)
		free(widget_frame->border_width);
	if(widget_frame->relief)
		free(widget_frame->relief);
	if(widget_frame->relief)
		free(widget_frame->color);
	if(widget->content_rect && widget->content_rect != &widget->screen_location)
		free(widget->content_rect);

	/* Frees text related stuffs */
	if(widget_frame->text)
		free(widget_frame->text);
	if(widget_frame->text_font)
		free(widget_frame->text_font);
	if(widget_frame->text_color)
		free(widget_frame->text_color);
	if(widget_frame->text_anchor)
		free(widget_frame->text_anchor);

	/* Frees img related stuffs */
	if(widget_frame->img)
		free(widget_frame->img);
	if(widget_frame->img_rect)
		free(widget_frame->img_rect);
	if(widget_frame->img_anchor)
		free(widget_frame->img_anchor);
}

void drawframe(struct ei_widget_t*		widget,
					  ei_surface_t		surface,
					  ei_surface_t		pick_surface,
					  ei_rect_t*		clipper) {
	drawframebuttonclasses(widget, surface, pick_surface, clipper, EI_TRUE);	
}

void setdefaultsframe(struct ei_widget_t* widget) {
	/* casting the widget (pointer) into a frame (pointer) */
	ei_frame_t * widget_frame;
	widget_frame = (ei_frame_t *)widget;

	/* configures the default color, relief and border width */
	if(widget_frame->color == NULL)
		widget_frame->color = malloc(sizeof(ei_color_t));
	*(widget_frame->color) = ei_default_background_color;

	if(widget_frame->relief == NULL)
		widget_frame->relief = malloc(sizeof(ei_relief_t));
	*(widget_frame->relief) = ei_relief_none;

	if(widget_frame->border_width == NULL)
		widget_frame->border_width = malloc(sizeof(int));
	*(widget_frame->border_width) = 0;
	widget->content_rect = &widget->screen_location;

	/**  
	 * configures the default text attributes
	 * even if the default text pointer is NULL, the text font, color and anchor
	 * are initialised so that they do not have to be defined later
	 */
	widget_frame->text = NULL;

	if(widget_frame->text_font == NULL)
		widget_frame->text_font = malloc(sizeof(ei_font_t));
	*(widget_frame->text_font) = ei_default_font;

	if(widget_frame->text_color == NULL)
		widget_frame->text_color = malloc(sizeof(ei_color_t));
	*(widget_frame->text_color) = ei_font_default_color;

	if(widget_frame->text_anchor == NULL)
		widget_frame->text_anchor = malloc(sizeof(ei_anchor_t));
	*(widget_frame->text_anchor) = ei_anc_center;

	/**  
	 * configures the default image attributes
	 * even if the default image pointer is NULL, the image anchor
	 * is initialised so that it does not have to be defined later 
	 */
	if(widget_frame->img)
		free(widget_frame->img);
	widget_frame->img = NULL;
	if(widget_frame->img_rect)
		free(widget_frame->img_rect);
	widget_frame->img_rect = NULL;

	if(widget_frame->img_anchor == NULL)
		widget_frame->img_anchor = malloc(sizeof(ei_anchor_t));
	*(widget_frame->img_anchor) = ei_anc_center;
}

void geomnotifyframe(struct ei_widget_t* widget) {
	int *border_width = ((ei_frame_t*)widget)->border_width;
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
	}
	ei_widget_t *sibling = widget->children_head;
	while(sibling) {
		if(sibling->geom_params)
			sibling->geom_params->manager->runfunc(sibling);
		sibling = sibling->next_sibling;
	}
	ei_app_invalidate_rect(&widget->screen_location);
}
