/**
 * @file	ei_widget.c
 * @brief 	API for widgets management: creation, configuration, hierarchy, redisplay.
 * 
 */

#include "ei_widget.h"
#include "ei_frameclass.h"
#include "ei_buttonclass.h"
#include "ei_application.h"
#include "ei_toplevelclass.h"
#include "ei_event_more.h"
#include "ei_placermanager.h"
#include "ei_calculations.h"
#include <string.h>

static uint32_t wid_id = 0;
extern ei_surface_t pick_surface;


void ei_frame_configure(ei_widget_t *widget,
						ei_size_t *requested_size,
						const ei_color_t *color,
						int *border_width,
						ei_relief_t *relief,
						char **text,
						ei_font_t *text_font,
						ei_color_t *text_color,
						ei_anchor_t *text_anchor,
						ei_surface_t *img,
						ei_rect_t **img_rect,
						ei_anchor_t *img_anchor) {
	if (requested_size != NULL)
		widget->requested_size = *requested_size;
	else if(text && *text) {
		int w; int h;
		hw_text_compute_size(*text, text_font ? *text_font : ei_default_font, &w, &h);
		widget->requested_size = (ei_size_t){w+2*(border_width?*border_width:0), h+2*(border_width?*border_width:0)};
	} else if(img && *img) {
		widget->requested_size = hw_surface_get_size(*img);
		widget->requested_size.width += 2*(border_width?*border_width:0);
		widget->requested_size.height += 2*(border_width?*border_width:0);
	} else
		widget->requested_size = (ei_size_t){0, 0};

	/* casting the widget (pointer) into a frame (pointer) */
	ei_frame_t *widget_frame;
	widget_frame = (ei_frame_t *)widget;

	/* configures the color */
	if(!color) {
		if(!widget_frame->color) {
			widget_frame->color = malloc(sizeof(ei_color_t));
			*(widget_frame->color) = ei_default_background_color;
		}
	} else
		*(widget_frame->color) = *color;

	/* configures the relief */
	if(!relief) {
		if(!widget_frame->relief) {
			widget_frame->relief = malloc(sizeof(ei_relief_t));
			*(widget_frame->relief) = ei_relief_none;
		}
	} else
		*(widget_frame->relief) = *relief;

	/* configures the border width */
	if(!border_width) {
		if(!widget_frame->border_width) {
			widget_frame->border_width = malloc(sizeof(int));
			*(widget_frame->border_width) = 0;
			widget->content_rect = &widget->screen_location;
		}
	} else {
		*(widget_frame->border_width) = *border_width;
		if(widget->content_rect != &widget->screen_location)
			widget->content_rect = calloc(1, sizeof(ei_rect_t));
	}

	/* 
		configures the text or image :
		if the text parameter is not NULL, it configures the text.
		if the img parameter is not NULL, it configures the image,
		and its sub-rectangle
		'text' and 'img' cannot be non-NULL at the same time :
		either we configure the widget's text or its image

		text font, color and anchor and image anchor are handled
		even if 'text' and 'img' are NULL : if a frame has no text 
		it will however have a default text font, color and anchor 
		(and a frame without image will have a default image anchor),
		so that if a text or an image is set later it will already have
		default values.
	*/
	if(text && img) {
		perror("parameters 'char** text' and 'ei_surface_t* img' \
		cannot be both non-NULL : \
		if one of them is non-NULL, the other should be NULL");
		exit(EXIT_FAILURE);
	} else if(text != NULL && strlen((char *)text) < 40) {
		/* configures the text */
		if(*text) {
			if(widget_frame->text)
				free(widget_frame->text);
			widget_frame->text = calloc(1, sizeof(ei_widget_text_t));
			strcpy(*(widget_frame->text), *text);
		} else
			widget_frame->text = NULL;
		widget_frame->img = NULL;
	} else if(img) {
		/* configures the image */
		if(*img) {
			if (widget_frame->img == NULL)
				widget_frame->img = malloc(sizeof(ei_surface_t));
			*(widget_frame->img) = *img;

			/* configures the rect */
			if(img_rect) {
				if (widget_frame->img_rect == NULL)
					widget_frame->img_rect = malloc(sizeof(ei_rect_t));
				*(widget_frame->img_rect) = **img_rect;
			}
		} else
			widget_frame->img = NULL;
		widget_frame->text = NULL;
	}

	/* configures the text font */
	if(!text_font) {
		if(!widget_frame->text_font) {
			widget_frame->text_font = malloc(sizeof(ei_font_t));
			*(widget_frame->text_font) = ei_default_font;
		}
	} else
		*(widget_frame->text_font) = *text_font;

	/* configures the text color */
	if(!text_color) {
		if(!widget_frame->text_color) {
			widget_frame->text_color = malloc(sizeof(ei_color_t));
			*(widget_frame->text_color) = ei_font_default_color;
		}
	} else
		*(widget_frame->text_color) = *text_color;

	/* configures the text anchor */
	if(!text_anchor) {
		if(!widget_frame->text_anchor) {
			widget_frame->text_anchor = malloc(sizeof(ei_anchor_t));
			*(widget_frame->text_anchor) = ei_anc_center;
		}
	} else
		*(widget_frame->text_anchor) = *text_anchor;

	/* configures the image anchor */
	if(!img_anchor) {
		if (!widget_frame->img_anchor) {
			widget_frame->img_anchor = malloc(sizeof(ei_anchor_t));
			*(widget_frame->img_anchor) = ei_anc_center;
		}
	} else
		*(widget_frame->img_anchor) = *img_anchor;

	if(widget->geom_params) {
		widget->geom_params->manager->runfunc(widget);
		ei_rect_t inv_rect = extend_rect(widget->screen_location);
        ei_app_invalidate_rect(&inv_rect);
	}
}

void ei_button_configure	(ei_widget_t*		widget,
							 ei_size_t*		requested_size,
							 const ei_color_t*	color,
							 int*			border_width,
							 int*			corner_radius,
							 ei_relief_t*		relief,
							 char**			text,
							 ei_font_t*		text_font,
							 ei_color_t*		text_color,
							 ei_anchor_t*		text_anchor,
							 ei_surface_t*		img,
							 ei_rect_t**		img_rect,
							 ei_anchor_t*		img_anchor,
							 ei_callback_t*		callback,
							 void**			user_param) {
	if(requested_size != NULL)
		widget->requested_size = *requested_size;
	else if(text && *text) {
		int w; int h;
		hw_text_compute_size(*text, text_font ? *text_font : ei_default_font, &w, &h);
		widget->requested_size = (ei_size_t){w+2*(border_width?*border_width:0), h+2*(border_width?*border_width:0)};
	} else if(img && *img) {
		widget->requested_size = hw_surface_get_size(*img);
		widget->requested_size.width += 2*(border_width?*border_width:0);
		widget->requested_size.height += 2*(border_width?*border_width:0);
	} else
		widget->requested_size = (ei_size_t){0, 0};

	/* casting the widget (pointer) into a button (pointer) */
	ei_button_t *widget_button;
	widget_button = (ei_button_t*)widget;

	/* configures the border width */
	if(!border_width) {
		if(!widget_button->border_width) {
			widget_button->border_width = malloc(sizeof(int));
			*(widget_button->border_width) = k_default_button_border_width;
			widget->content_rect = &widget->screen_location;
		}
	} else {
		*(widget_button->border_width) = *border_width;
		if(widget->content_rect != &widget->screen_location)
			widget->content_rect = calloc(1, sizeof(ei_rect_t));
	}

	/* configures the corner radius */
	if(!corner_radius) {
		if(!widget_button->corner_radius) {
			widget_button->corner_radius = malloc(sizeof(int));
			*(widget_button->corner_radius) = k_default_button_corner_radius;
		}
	} else
		*(widget_button->corner_radius) = *corner_radius;

	/* configures the color */
	if(!color) {
		if(!widget_button->color) {
			widget_button->color = malloc(sizeof(ei_color_t));
			*(widget_button->color) = ei_default_background_color;
		}
	} else
		*(widget_button->color) = *color;

	/* configures the relief */
	if(!relief) {
		if(!widget_button->relief) {
			widget_button->relief = malloc(sizeof(ei_relief_t));
			*(widget_button->relief) = ei_relief_raised;
		}
	} else
		*(widget_button->relief) = *relief;

	/* 
		configures the text or image :
		if the text parameter is not NULL, it configures the text.
		if the img parameter is not NULL, it configures the image,
		and its sub-rectangle
		'text' and 'img' cannot be non-NULL at the same time :
		either we configure the widget's text or its image

		text font, color and anchor and image anchor are handled
		even if 'text' and 'img' are NULL : if a button has no text 
		it will however have a default text font, color and anchor 
		(and a button without image will have a default image anchor),
		so that if a text or an image is set later it will already have
		default values.
	*/
	if(text && img) {
		perror("parameters 'char** text' and 'ei_surface_t* img' \
		cannot be both non-NULL : \
		if one of them is non-NULL, the other should be NULL");
		exit(EXIT_FAILURE);
	} else if(text && strlen((char *)text) < 40) {
		/* configures the text */
		if(*text) {
			if(widget_button->text)
				free(widget_button->text);
			widget_button->text = calloc(1, sizeof(ei_widget_text_t));
			strcpy(*(widget_button->text), *text);
		} else
			widget_button->text = NULL;
		widget_button->img = NULL;
	} else if(img) {
		/* configures the image */
		if(*img) {
			/* Copy the image */
			ei_size_t img_size = hw_surface_get_size(*img);
			if(widget_button->img)
				free(widget_button->img);
			else 
				widget_button->img = malloc(sizeof(ei_surface_t));
			*(widget_button->img) = hw_surface_create(*img, img_size, EI_FALSE);
			ei_copy_surface(*(widget_button->img), NULL, *img, NULL, EI_FALSE);

			/* configures the rect */
			if(img_rect) {
				if (!widget_button->img_rect)
					widget_button->img_rect = malloc(sizeof(ei_rect_t));
				*(widget_button->img_rect) = **img_rect;
			}
		} else
			widget_button->img = NULL;
		widget_button->text = NULL;
	}

	/* configures the text font */
	if(!text_font) {
		if(!widget_button->text_font) {
			widget_button->text_font = malloc(sizeof(ei_font_t));
			*(widget_button->text_font) = ei_default_font;
		}
	} else
		*(widget_button->text_font) = *text_font;

	/* configures the text color */
	if(!text_color) {
		if(!widget_button->text_color) {
			widget_button->text_color = malloc(sizeof(ei_color_t));
			*(widget_button->text_color) = ei_font_default_color;
		}
	} else
		*(widget_button->text_color) = *text_color;

	/* configures the text anchor */
	if(!text_anchor) {
		if(!widget_button->text_anchor) {
			widget_button->text_anchor = malloc(sizeof(ei_anchor_t));
			*(widget_button->text_anchor) = ei_anc_center;
		}
	} else
		*(widget_button->text_anchor) = *text_anchor;

	/* configures the image anchor */
	if(!img_anchor) {
		if(!widget_button->img_anchor) {
			widget_button->img_anchor = malloc(sizeof(ei_anchor_t));
			*(widget_button->img_anchor) = ei_anc_center;
		}
	} else
		*(widget_button->img_anchor) = *img_anchor;

	/* Binds callback to mouse_down on the button */
	if(callback) {
		widget_button->callback = malloc(sizeof(ei_callback_t));
		*widget_button->callback = *callback;
	}
	if(user_param) {
		widget_button->user_param = malloc(sizeof(void*));
		*widget_button->user_param = *user_param;
	}
	

	/* special parameters : these must be changed AFTER a button_configure() */
	widget_button->no_clipping = EI_FALSE;

	widget_button->is_quit_button = EI_FALSE;
	widget_button->is_resize_button = EI_FALSE;
	
	if(widget->geom_params) {
		widget->geom_params->manager->runfunc(widget);
		ei_rect_t inv_rect = extend_rect(widget->screen_location);
        ei_app_invalidate_rect(&inv_rect);
	}
}

void ei_toplevel_configure(ei_widget_t *widget,
						   ei_size_t *requested_size,
						   ei_color_t *color,
						   int *border_width,
						   char **title,
						   ei_bool_t *closable,
						   ei_axis_set_t *resizable,
						   ei_size_t **min_size)
{
	/* configures requested size */
	if(requested_size)
		widget->requested_size = *requested_size;
	else
		widget->requested_size = (ei_size_t){320, 240};
	
	/* casting the widget (pointer) into a toplevel (pointer) */
	ei_toplevel_t *widget_toplevel = (ei_toplevel_t *)widget;

	/* configures the color */
	if(!color) {
		if(!widget_toplevel->background_color) {
			widget_toplevel->background_color = malloc(sizeof(ei_color_t));
			*(widget_toplevel->background_color) = ei_default_background_color;
		}
	} else
		*(widget_toplevel->background_color) = *color;

	/* configures the border width */
	if(!border_width) {
		if(!widget_toplevel->border_width) {
			widget_toplevel->border_width = malloc(sizeof(int));
			*(widget_toplevel->border_width) = 4;
		}
	} else
		*(widget_toplevel->border_width) = *border_width;

	/* configures the title */
	if(!title) {
		if(!widget_toplevel->title) {
			widget_toplevel->title = malloc(sizeof(char[9]));
			*(widget_toplevel->title) = "Toplevel";
		}
	} else {
		free(widget_toplevel->title);
		widget_toplevel->title = malloc(sizeof(*title));
		*(widget_toplevel->title) = *title;
	}

	if(!widget_toplevel->title_font) {
		widget_toplevel->title_font = malloc(sizeof(ei_font_t));
		*(widget_toplevel->title_font) = ei_default_font;
	}

	/* defines if the toplevel is closable or not */
	if(!closable) {
		if(!widget_toplevel->closable) {
			widget_toplevel->closable = malloc(sizeof(ei_bool_t));
			*(widget_toplevel->closable) = EI_TRUE;
		}
	} else {
		*(widget_toplevel->closable) = *closable;

		/* if the toplevel is not closable, withdraw the quit button */
		if(*closable == EI_FALSE) {
			ei_widget_t * curr = widget->children_head;

			/* if the quit button is at the head (it must be the case) */
			if(!strcmp(curr->wclass->name,"button") && ((ei_button_t*)curr)->is_quit_button) {
				/* changes the head */
				widget->children_head = curr->next_sibling;
				/* if the quit button was at the head and the tail (i.e. no other siblings) */
				if(widget->children_tail == curr)
					widget->children_tail = NULL;
				
				ei_widget_destroy(curr);
			} else { /* the quit button is not at the head : we finds it */
				while(curr && curr->next_sibling && (strcmp(curr->next_sibling->wclass->name,"button") || 
				(!strcmp(curr->next_sibling->wclass->name,"button") && !((ei_button_t*)curr->next_sibling)->is_quit_button)) )
					curr = curr->next_sibling;

				/* checks that it is a quit button : the while loop can end without having 
				found a quit button if the toplevel already has no quit button */
				if(curr && curr->next_sibling && !strcmp(curr->next_sibling->wclass->name,"button") 
				&& ((ei_button_t*)curr->next_sibling)->is_quit_button) {
					ei_widget_t* quit_button = curr->next_sibling;
					/* withdraws the quit button from the children list */
					/* if the quit button is not at the end */
					if(quit_button->next_sibling) {
						curr->next_sibling = quit_button->next_sibling;
					} else { /* if it is at the end */
						widget->children_tail = curr;
					}
					ei_widget_destroy(quit_button);
				}
			}
		}
	}

	/* defines if and where the toplevel is sizeable or not */
	if(!resizable) {
		if(!widget_toplevel->resizable) {
			widget_toplevel->resizable = malloc(sizeof(ei_axis_set_t));
			*(widget_toplevel->resizable) = ei_axis_both;
		}
	} else {
		*(widget_toplevel->resizable) = *resizable;

		/* if the toplevel is not resizable, withdraw the resize button */
		if(*resizable == EI_FALSE) {
			ei_widget_t * curr = widget->children_head;

			/* if the resize button is at the head (it must not be the case) */
			if(!strcmp(curr->wclass->name,"button") && ((ei_button_t*)curr)->is_resize_button) {
				/* changes the head */
				widget->children_head = curr->next_sibling;
				/* if the resize button was at the head and the tail (i.e. no other siblings) */
				if(widget->children_tail == curr)
					widget->children_tail = NULL;
				ei_widget_destroy(curr);
			} else { /* the resize button is not at the head : we finds it (it must be at the tail) */
				while(curr && curr->next_sibling && (strcmp(curr->next_sibling->wclass->name,"button") || 
				(!strcmp(curr->next_sibling->wclass->name,"button") && !((ei_button_t*)curr->next_sibling)->is_resize_button)) )
					curr = curr->next_sibling;

				/* checks that it is a resize button : the while loop can end without having 
				found a resize button if the toplevel already has no resize button */
				if(curr && curr->next_sibling && !strcmp(curr->next_sibling->wclass->name,"button") 
				&& ((ei_button_t*)curr->next_sibling)->is_resize_button) {
					ei_widget_t* resize_button = curr->next_sibling;
					/* withdraws the resize button from the children list */
					/* if the resize button is not at the end */
					if(resize_button->next_sibling) {
						curr->next_sibling = resize_button->next_sibling;
					} else { /* if it is at the end (it must be the case)*/
						widget->children_tail = curr;
					}
					ei_widget_destroy(resize_button);
				}
			}
		}
	}

	/* configures the minimum size */
	if(!min_size) {
		if(!widget_toplevel->min_size) {
			widget_toplevel->min_size = malloc(sizeof(ei_size_t));
			*(widget_toplevel->min_size) = (ei_size_t){160, 120};
		}
	} else
		*(widget_toplevel->min_size) = **min_size;

	if(widget->geom_params) {
		widget->geom_params->manager->runfunc(widget);
		ei_rect_t inv_rect = extend_rect(widget->screen_location);
        ei_app_invalidate_rect(&inv_rect);
	}
}

ei_widget_t *ei_widget_create(ei_widgetclass_name_t class_name,
							  ei_widget_t *parent,
							  void *user_data,
							  ei_widget_destructor_t destructor)
{

	/* a pointer to the given widget class */
	ei_widgetclass_t *wclass = ei_widgetclass_from_name(class_name);
	/* allocates memory depending on the widget class */
	ei_widget_t *wid = wclass->allocfunc();

	/*Copy of wid_id to not alter the orginal */
	/* sets the widgetclass attributes */
	wid->wclass = wclass;
	wid->pick_id = wid_id++;
	wid->user_data = user_data;
	wid->destructor = destructor;
	wid->parent = parent;

	/* sets widget's pick color */
	uint32_t temp_wid_id = wid->pick_id;

	wid->pick_color = calloc(1, sizeof(ei_color_t));
	wid->pick_color->red = temp_wid_id & 255;
	temp_wid_id >>= 8;
	if (temp_wid_id > 0) {
		wid->pick_color->green = temp_wid_id & 255;
		temp_wid_id >>= 8;
		if (temp_wid_id > 0) {
			wid->pick_color->blue = temp_wid_id & 255;
			temp_wid_id >>= 8;
		}
	}
	wid->pick_color->alpha = 255;

	/* manages the widgets (children) if the created widget has a parent (i.e. is not the root) */
	if(parent) {
		/* if the parent has children, we add the widget to the children list */
		ei_bool_t place_at_tail = EI_TRUE;
		if(!strcmp(parent->wclass->name,"toplevel") && *((ei_toplevel_t*)parent)->resizable && parent->children_head ){
			ei_widget_t* penultimate = parent->children_head;
			ei_widget_t* ultimate = penultimate->next_sibling;

			/* if there is only child */
			if(!ultimate) {
				parent->children_head = wid;
				wid->next_sibling = penultimate;
				place_at_tail = EI_FALSE;
			} else {
				/* finds the first resize button of the children, must be the last one */
				while(strcmp(ultimate->wclass->name,"button") ||
					(!strcmp(ultimate->wclass->name,"button") && !((ei_button_t*)ultimate)->is_resize_button)){
					penultimate = penultimate->next_sibling;
					ultimate = penultimate->next_sibling;
				}

				/* checks that we are at the tail and it is the resize button */
				if(ultimate->next_sibling == NULL && parent->children_tail == ultimate 
				&& !strcmp(ultimate->wclass->name,"button") && ((ei_button_t*)ultimate)->is_resize_button) {
					penultimate->next_sibling = wid;
					wid->next_sibling = parent->children_tail;
					place_at_tail = EI_FALSE;
				}
			}
		}

		if(place_at_tail) {
			if(parent->children_tail){
				parent->children_tail->next_sibling = wid;
			}
			parent->children_tail = wid;
			wid->next_sibling = NULL;

			if(!parent->children_head)
				parent->children_head = wid;
		}
	}
	/* sets the default attributes of the widget */
	wclass->setdefaultsfunc(wid);
	return wid;
}

void ei_widget_destroy_recurs(ei_widget_t *widget)
{
	/* Removes the widget from the screen if it is currently managed by a geometry manager */
	if (widget->destructor)
		widget->destructor(widget);

	/* destroys of the children */
	ei_widget_t *current_free = widget;
	ei_widget_t *next;
	while (current_free)
	{
		next = current_free->next_sibling;

		/* destroys children of the children if they have so */
		if (current_free->children_head)
			ei_widget_destroy_recurs(current_free->children_head);

		/* calls the release function depending on the widget class */
		ei_geometrymanager_unmap(current_free);
		current_free->wclass->releasefunc(current_free);
		free(current_free->pick_color);
		free(current_free);
		current_free = next;
	}
}

void ei_widget_destroy(ei_widget_t *widget)
{
	/* TODO : mathis zizi */
	ei_app_invalidate_rect(&widget->screen_location);
	destroy_event_binded_to_widget(widget);

	/* Removes the widget from the screen if it is currently managed by a geometry manager */
	if (widget->destructor)
		widget->destructor(widget);

	/* destroys of the children (note that we do not check if the widget has children : 
	   we must have checked before)*/
	if (widget->children_head)
		ei_widget_destroy_recurs(widget->children_head);

	if (widget->parent) {
		ei_widget_t *last_child = NULL;
		ei_widget_t *curr_child = widget->parent->children_head;
		while (curr_child && curr_child != widget) {
			last_child = curr_child;
			curr_child = curr_child->next_sibling;
		}

		if (!last_child && widget->parent->children_tail == curr_child) {
			widget->parent->children_tail = NULL;
			widget->parent->children_head = NULL;
		} else if (!last_child) {
			widget->parent->children_head = widget->next_sibling;
		} else if (curr_child == widget->parent->children_tail) {
			last_child->next_sibling = NULL;
			widget->parent->children_tail = last_child;
		} else
			last_child->next_sibling = curr_child->next_sibling;
	}

	/* calls the release function depending on the widget class */
	ei_geometrymanager_unmap(widget);
	widget->wclass->releasefunc(widget);
	free(widget->pick_color);
	free(widget);
}

ei_widget_t *ei_widget_pick_rec(ei_widget_t *parent, uint32_t pick_color, ei_surface_t pick_surface)
{
	/* Checks if the parent matches */
	if (pick_color == ei_map_rgba(pick_surface, parent->pick_color))
		return parent;

	/* Navigator through the parent's direct children */
	ei_widget_t *curr_test = parent->children_head;
	/* Go through the parent's direct children linked list */
	while (curr_test) {
		/* Try to find the matching widget */
		ei_widget_t *ret = ei_widget_pick_rec(curr_test, pick_color, pick_surface);

		/* If it is found, return it */
		if (ret)
			return ret;

		/* Else continue */
		curr_test = curr_test->next_sibling;
	}

	/* If no widget was found, returns NULL */
	return NULL;
}

ei_widget_t *ei_widget_pick(ei_point_t *where)
{
	/* Gets the pick_surface's pixel colot under the mouse */
	uint32_t *pixel_ptr = (uint32_t *)hw_surface_get_buffer(pick_surface);
	ei_size_t pick_size = hw_surface_get_size(pick_surface);
	pixel_ptr += where->x + where->y * pick_size.width;

	/* Returns found widget */
	return ei_widget_pick_rec(ei_app_root_widget(), *pixel_ptr, pick_surface);
}
