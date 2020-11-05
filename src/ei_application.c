/**
 *  @file	ei_application.c
 *  @brief	Manages the main steps of a graphical application: initialization, main window,
 *		main loop, quitting, resource freeing.
 *
 */

#include "ei_application.h"
#include "ei_widgetclass.h"
#include "ei_frameclass.h"
#include "ei_buttonclass.h"
#include "ei_toplevelclass.h"
#include "ei_geometrymanager.h"
#include "ei_widget.h"
#include "ei_types.h"
#include "ei_utils.h"
#include "ei_draw_more.h"
#include "ei_event_more.h"
#include "ei_calculations.h"
#include <stdio.h>
#include <unistd.h>


/* root widget (usually the root window)*/
static ei_frame_t *root_widget = NULL;
static ei_surface_t root_surface;
ei_surface_t pick_surface;
/* list of all the widget classes */
extern ei_widgetclass_t *widclss_top;
/* list of all the geometry managers */
extern ei_geometrymanager_t *geommanager_top;
/* list of the invalidated rectangles  */
ei_linked_rect_t *invalidated_rects = NULL;
/* boolean to run the app loop */
static ei_bool_t run = EI_TRUE;
/* true if resizing */
static ei_bool_t resizing = EI_FALSE;


void ei_app_create(ei_size_t main_window_size, ei_bool_t fullscreen) {
	/* hardware initialisation */
	hw_init();

	/* root window creation */
	root_surface = hw_create_window(main_window_size, fullscreen);

	/* registration of all geometry managers */
	ei_register_placer_manager();

	/* registration of all classes of widget */
	ei_frame_register_class();
	ei_button_register_class();
	ei_toplevel_register_class();

	/* defines the root window as the root widget (of class frame) */
	root_widget = (ei_frame_t*)ei_widget_create("frame", NULL, NULL, NULL);

	/* builds default screen_location for the root widget */
	ei_rect_t root_screen_location;;

	root_screen_location.top_left = (ei_point_t) {0, 0};
	root_screen_location.size = main_window_size;

	/* initialisation of the root widget's attributes */
	ei_frame_configure((ei_widget_t*)root_widget, &main_window_size, &ei_default_background_color, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	root_widget->widget.screen_location = root_screen_location;

	pick_surface = hw_surface_create(root_surface, main_window_size, EI_FALSE);

	/* Binding all buttons (for their animations) */
	ei_bind(ei_ev_mouse_buttondown, NULL, "button", ei_handle_button_down, NULL);
	ei_bind(ei_ev_mouse_buttonup, NULL, "all", ei_handle_button_up, NULL);
	ei_bind(ei_ev_mouse_move, NULL, "all", ei_handle_button_move, NULL);

	/* Binding for resizing events */
	ei_bind(ei_ev_mouse_buttonup, NULL, "all", end_resize, NULL);
	ei_bind(ei_ev_mouse_move, NULL, "all", resize, NULL);

	/* Binding for moving events */
	ei_bind(ei_ev_mouse_buttonup, NULL, "all", end_move, NULL);
	ei_bind(ei_ev_mouse_move, NULL, "all", move, NULL);
}



void ei_draw_widget_children(ei_widget_t* widget, ei_rect_t* parent_clipper) {
	if(parent_clipper == NULL)
		return;

	if(parent_clipper->size.width < 0 || parent_clipper->size.height < 0)
		return;

	/* the first widget to draw is the head of all the children */
	ei_widget_t * current_draw = widget->children_head;

	/* stops to draw if all the children have been drawn, or if there is no child */
	while(current_draw) {
		/* Do not draw the widget if it does not have geometry parameters */
		if(current_draw->geom_params) {
			/* Run the widget's geometry manager */
			current_draw->geom_params->manager->runfunc(current_draw);

			/* calls the draw function of the widget to draw */
			current_draw->wclass->drawfunc(current_draw, ei_app_root_surface(), pick_surface, parent_clipper);

			/* Computes clipper for the current widget */
			ei_rect_t current_clipper = get_ei_rect_intersection(*parent_clipper, *current_draw->content_rect);
			
			/* recursive call : draws the children of the current widget */
			ei_draw_widget_children(current_draw, &current_clipper);
		}

		/* after having drawn a widget and its children, draws the next widget */
		current_draw = current_draw->next_sibling;		
	}
}


void initialDraw() {
	/* root widget */
	ei_widget_t* root = ei_app_root_widget();

	/* if there is no root widget (root = NULL), nothing is done */
	if(root) {
		/* root seen as frame (in case the root is not already a frame) */
		ei_frame_t* frame_root = (ei_frame_t *) root;

		/* surface of the root */
		ei_size_t size = hw_surface_get_size(ei_app_root_surface());

		/* Locks root's surface */
		hw_surface_lock(ei_app_root_surface());
		hw_surface_lock(pick_surface);

		/* draw the root and all its children */
		root->wclass->drawfunc(root, ei_app_root_surface(), pick_surface, frame_root->widget.content_rect);
		ei_draw_widget_children(root, frame_root->widget.content_rect);

		/* Unlocks root's surface */
		hw_surface_unlock(ei_app_root_surface());
		hw_surface_unlock(pick_surface);
		
		/* upadtes the rectangles to draw because of a bug (AUR machines do not seem to sync the code and the graphic card) */
		for(int i = 0; i < 10; i++) {
			usleep(5000);
			hw_surface_update_rects(ei_app_root_surface(), NULL);
		}
	}
}


ei_bool_t is_under_mouse(ei_widget_t* widget, ei_point_t mouse_pos, ei_surface_t pick_surface) {
	/**
	 *	Checks if 'widget' is under 'mouse_pos' according to 'pick_surface'
	 */

	/* Gets the pick_surface's pixel color under the mouse */
	uint32_t* pixel_ptr = (uint32_t *) hw_surface_get_buffer(pick_surface);
	ei_size_t pick_size = hw_surface_get_size(pick_surface); 
	pixel_ptr += mouse_pos.x + mouse_pos.y * pick_size.width;

	/* Returns true if the color is the same as the widget color */
	return (ei_bool_t) *pixel_ptr == ei_map_rgba(pick_surface, widget->pick_color);
}

ei_widget_t *find_picked_in_children(ei_widget_t *parent, uint32_t pick_color, ei_surface_t pick_surface) {
	/**
	 *	Returns the widget that is under 'mouse_pos' according to 'pick_surface' that is parent or one of it's children.
	 *	Returns NULL if no widget is found.
	 */
	
	/* Checks if the parent matches */
	if(pick_color == ei_map_rgba(pick_surface, parent->pick_color))
		return parent;

	/* Navigator through the parent's direct children */
	ei_widget_t* curr_test = parent->children_head;
	/* Go through the parent's direct children linked list */
	while(curr_test) {
		/* Try to find the matching widget */
		ei_widget_t* ret = find_picked_in_children(curr_test, pick_color, pick_surface);

		/* If it is found, return it */
		if(ret)
			return ret;

		/* Else continue */
		curr_test = curr_test->next_sibling;
	}

	/* If no widget was found, returns NULL */
	return NULL;
}

ei_widget_t *find_picked_tagged_widget(ei_point_t mouse_pos,ei_surface_t pick_surface, ei_tag_t tag) {
	/**
	 *	Returns the widget that is under 'mouse_pos' according to 'pick_surface' that matches 'tag' 
	 * 	Returns NULL if no widget is found.
	 */

	/* Gets the pick_surface's pixel colot under the mouse */
	uint32_t* pixel_ptr = (uint32_t *) hw_surface_get_buffer(pick_surface);
	ei_size_t pick_size = hw_surface_get_size(pick_surface); 
	pixel_ptr += mouse_pos.x + mouse_pos.y * pick_size.width;

	/* Get the widget under 'mouse_pos' */
	ei_widget_t *found = find_picked_in_children(ei_app_root_widget(), *pixel_ptr, pick_surface);

	/* Returns this widget if its tag correspond or if the requested tag is "all" */
	if(found && (!strcmp(tag, "all") || !strcmp(tag, found->wclass->name)))
		return found;

	return NULL;
}


ei_widget_t *find_unlocalised_tagged_widget(ei_widget_t *parent, ei_tag_t tag) {
	/* Checks if the parent's tag matches */
	if(!strcmp(tag, "all") || !strcmp(tag, parent->wclass->name))
		return parent;

	/* Navigator through the parent's children */
	ei_widget_t *curr_test = parent->children_head;
	/* Go through the parent's chidren */
	while(curr_test) {
		/* Try to find matching widget */
		ei_widget_t *ret = find_unlocalised_tagged_widget(curr_test, tag);

		/* if found, return it */
		if(ret)
			return ret;

		/* else, continue */
		curr_test = curr_test->next_sibling;
	}

	/* if no widget was found, return NULL */
	return NULL;
}

void ei_app_run() {
	/* Creates the offscreen pick surface */
	/* First draw : all the frame*/
	initialDraw();
	
	/* Frees the list of rects */
	ei_linked_rect_t *current_rect = invalidated_rects;
	ei_linked_rect_t *next_rect;
	while(current_rect) {
		next_rect = current_rect->next;
		free(current_rect);
		current_rect = next_rect;
	}
	invalidated_rects = NULL;

	/* Event variables */
	ei_event_t event;
	event.type = ei_ev_none;
	int t=0;
	while(run) {
		/* Waits until next event */
		hw_event_wait_next(&event);

		/* Navigator through the binded events linked list */
		ei_linked_binded_event *current_bind = get_top_event_bind();

		/* True if the current event has been processed and the loop should stop */
		ei_bool_t processed = EI_FALSE;

		/* Go through the binded events linked list */
		while(!processed && current_bind) {

			/* If we found a matching event type ...*/
			if(current_bind->eventtype == event.type) {

				/* ..., if the target is a widget ... */
				if(current_bind->widget) { 

					/* ..., then we run the event if it is not localised or if the widget is under the mouse. */ 
				    if(!current_bind->pickable || is_under_mouse(current_bind->widget, event.param.mouse.where, pick_surface))
						processed = current_bind->callback(current_bind->widget, &event, current_bind->user_param);
				/* ..., if the target is a tag ... */	
				} else if (current_bind->tag) {
					/* Find targeted widget */
					ei_widget_t *to_callback = NULL;
					if(current_bind->pickable) {
						to_callback = find_picked_tagged_widget(event.param.mouse.where, pick_surface, current_bind->tag);
					} else
						to_callback = find_unlocalised_tagged_widget(ei_app_root_widget(), current_bind->tag);

					/* If found, activate event */
					if(to_callback)
						processed = current_bind->callback(current_bind->widget, &event, current_bind->user_param);
				}
			}

			/* Next binded event */
			current_bind = current_bind->next;
		}

		/* Redraws invalidated_rects */
		ei_linked_rect_t * current_invalidated_rect = invalidated_rects;
		if(!current_invalidated_rect)
			continue;

		hw_surface_lock(ei_app_root_surface());
		hw_surface_lock(pick_surface);
		while(current_invalidated_rect) {
			ei_app_root_widget()->wclass->drawfunc(ei_app_root_widget(), ei_app_root_surface(), pick_surface, &current_invalidated_rect->rect);
			ei_draw_widget_children(ei_app_root_widget(), &current_invalidated_rect->rect);
			current_invalidated_rect = current_invalidated_rect->next;
		}

		hw_surface_unlock(ei_app_root_surface());
		hw_surface_unlock(pick_surface);

		hw_surface_update_rects(ei_app_root_surface(), invalidated_rects);

		/* Frees the list of rects */
		ei_linked_rect_t *current_rect = invalidated_rects;
		ei_linked_rect_t *next_rect;
		while(current_rect) {
			next_rect = current_rect->next;
			free(current_rect);
			current_rect = next_rect;
		}
		invalidated_rects = NULL;
	}
}

void ei_app_invalidate_rect(ei_rect_t* rect) {
	/* linked_rect to add to the list (a struct having a rect and a next rect)*/
	ei_linked_rect_t* rect_to_invalidate = malloc(sizeof(ei_linked_rect_t));

	/* as told in the header, the rect we add to the list is a copy of the rect given */
	normalize_rect(rect, ei_app_root_widget()->screen_location.size);
	rect_to_invalidate->rect = *rect;
	rect_to_invalidate->next = NULL;

	/* case 1 : this is the first rect to invalidate */
	if(!invalidated_rects) {
		/* initialise the rect list with the rect given */
		invalidated_rects = rect_to_invalidate;
	}/* case 2 : the list of rects to invalidate is not empty */
	else {
		/* linked_rect pointers */
		ei_linked_rect_t* current_linked_rect;
		ei_linked_rect_t* next_linked_rect;
		current_linked_rect = invalidated_rects;
		next_linked_rect = current_linked_rect->next;

		/* we stop on the last rect of the list of the rects to invalidate */
		while(next_linked_rect){
			current_linked_rect = next_linked_rect;
			next_linked_rect = next_linked_rect->next;
		}

		/* Optimizing with the last inserted rect (which is usually quit similar to the new rect) */
		const ei_rect_t last = current_linked_rect->rect;
		printf("temp\n");
		if(	last.top_left.x-rect->top_left.x<max(last.size.width, rect->size.width)/2 &&
			last.top_left.y-rect->top_left.y<max(last.size.height, rect->size.height)/2) {
			/* Computing the "union" of the two rects */
			ei_point_t new_topleft = {min(last.top_left.x, rect->top_left.x), min(last.top_left.y, rect->top_left.y)};
			ei_size_t new_size = {
				max(last.top_left.x+last.size.width, rect->top_left.x+rect->size.width) - new_topleft.x,
				max(last.top_left.y+last.size.height, rect->top_left.y+rect->size.height) - new_topleft.y
			};
			current_linked_rect->rect = (ei_rect_t){new_topleft, new_size};
			free(rect_to_invalidate);
		} else /* we add the given rect at the end of the list */
			current_linked_rect->next = rect_to_invalidate;
	}
}


void ei_app_free() {
	/* Frees widgets */
	ei_widget_t* root = ei_app_root_widget();
	if(root)
		ei_widget_destroy(root);

	/* Frees the list of widget classes */
	ei_widgetclass_t *current_class = widclss_top;
	ei_widgetclass_t *next_class;
	while(current_class) {
		next_class = current_class->next;
		free(current_class);
		current_class = next_class;
	}

	/* Frees the list of geometry managers */
	ei_geometrymanager_t *current_gm = geommanager_top;
	ei_geometrymanager_t *next_gm;
	while(current_gm) {
		next_gm = current_gm->next;
		free(current_gm);
		current_gm = next_gm;
	}

	/* Frees the list of rects */
	ei_linked_rect_t *current_rect = invalidated_rects;
	ei_linked_rect_t *next_rect;
	while(current_rect) {
		next_rect = current_rect->next;
		free(current_rect);
		current_rect = next_rect;
	}

	/* Unbinding all buttons (for their animations) */
	ei_unbind(ei_ev_mouse_buttondown, NULL, "button", ei_handle_button_down, NULL);
	ei_unbind(ei_ev_mouse_buttonup, NULL, "all", ei_handle_button_up, NULL);
	ei_unbind(ei_ev_mouse_move, NULL, "all", ei_handle_button_move, NULL);

	/* Unbinding for resizing events */
	ei_unbind(ei_ev_mouse_buttonup, NULL, "all", end_resize, NULL);
	ei_unbind(ei_ev_mouse_move, NULL, "all", resize, NULL);

	/* Unbinding for moving events */
	ei_unbind(ei_ev_mouse_buttonup, NULL, "all", end_move, NULL);
	ei_unbind(ei_ev_mouse_move, NULL, "all", move, NULL);

	ei_linked_binded_event *current_bind = get_top_event_bind();
	ei_linked_binded_event *next_bind;
	while(current_bind){
		next_bind = current_bind->next;
		free(current_bind);
		current_bind = next_bind;
	}

	hw_surface_free(ei_app_root_surface());
	hw_surface_free(pick_surface);

	/* hardware ending */
	hw_quit();
}


ei_surface_t ei_app_root_surface(void) {
	return root_surface;
}

ei_widget_t* ei_app_root_widget(void) {
	/*
	 *	Returns the root widget seen as a mere widget.
	 */
	return (ei_widget_t*)root_widget;
}

void ei_app_quit_request() {
	run = EI_FALSE;
}
