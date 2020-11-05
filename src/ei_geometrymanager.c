/**
 *  @file	ei_geometrymanager.c
 *  @brief	Manages the positioning and sizing of widgets on the screen.
 *
 */

#include "ei_geometrymanager.h"
#include "ei_placermanager.h"
#include "ei_application.h"
#include "ei_utils.h"
#include "ei_widget.h"

/* top of the list of the geomtry managers */
ei_geometrymanager_t *geommanager_top = NULL;


void ei_geometrymanager_register(ei_geometrymanager_t* geometrymanager) {
	/* frees the memory and returns silent if this geometry manager is already registered */
	if(ei_geometrymanager_from_name(geometrymanager->name)) {
		free(geometrymanager);
		return;
	}

	/* otherwise, adds (at the top of the geometry manager list) this geometry manager */
	if(geommanager_top)
		geometrymanager->next = geommanager_top;
	geommanager_top = geometrymanager;
}


ei_geometrymanager_t* ei_geometrymanager_from_name(ei_geometrymanager_name_t name) {
	/* returns NULL if no geometry manager was found */
	if(!geommanager_top)
		return NULL;

	/* looking for geometry manager named 'name' in the geometry manager list */
	ei_geometrymanager_t *current_gm = geommanager_top;
	/* stops when the end is reached (i.e. NULL) or when the two strings match (i.e. same names)*/
	while(current_gm && strcmp((char*)current_gm->name, (char*)name))
		current_gm = current_gm->next;
	return current_gm;
}

void ei_geometrymanager_unmap(ei_widget_t* widget) {
	/* returns silent if the widget has no geometrical parameters */
	if(!widget->geom_params)
		return;

	/* releasefunc on the widget to remove */
	if(widget->geom_params->manager->releasefunc)
		widget->geom_params->manager->releasefunc(widget);

	/* frees its geometrical parameters and set them to NULL */
	free(widget->geom_params);
	widget->geom_params = NULL;


	/**
	 * the screen needs to be updated when the widget is removed,
	 * so the rectangle that the widget occupies is invalidated
	 */
	ei_app_invalidate_rect(&widget->screen_location);
	
	/* sets a default location */
	widget->screen_location = ei_rect_zero();
}

void ei_register_placer_manager(void) {
	/* dynamically allocates memory for the geometry manager to add */
	ei_geometrymanager_t *placer_gm = calloc(1, sizeof(ei_geometrymanager_t));

	/* defines all the attributes of the geometry manager 'placer' */
	strcpy((char*)placer_gm->name, "placer");
	placer_gm->runfunc = runplacer;
	placer_gm->releasefunc = NULL;

	/* registers the geometry manager that has been defined above */
	ei_geometrymanager_register(placer_gm);
}

void ei_place(ei_widget_t*		widget,
			  ei_anchor_t*		anchor,
			  int*				x,
			  int*				y,
			  int*				width,
			  int*				height,
			  float*			rel_x,
			  float*			rel_y,
			  float*			rel_width,
			  float*			rel_height) {
	/* if the geometrical parameters are already set with the placer, unmaps the placer */
	if(widget->geom_params && strcmp((char*)widget->geom_params->manager->name, "placer"))
		ei_geometrymanager_unmap(widget);
	
	/* if the widget has no geometry manager yet (or the latter was removed just above) : memory allocation... */
	ei_bool_t first_use = widget->geom_params ? EI_FALSE : EI_TRUE;
	if(!widget->geom_params)
		widget->geom_params = calloc(1, sizeof(ei_placer_param_t));

	/* ... and sets the placer as the widget's geomtry manager*/
	widget->geom_params->manager = ei_geometrymanager_from_name("placer");

	/* sets all the geometrical parameters */
	if(anchor)
		((ei_placer_param_t*)widget->geom_params)->anchor = *anchor;
	else if(first_use)
		((ei_placer_param_t*)widget->geom_params)->anchor = ei_anc_northwest;

	if(x)
		((ei_placer_param_t*)widget->geom_params)->x = *x;
	else if(first_use)
	 	((ei_placer_param_t*)widget->geom_params)->x = 0;

	if(y)
		((ei_placer_param_t*)widget->geom_params)->y = *y;
	else if(first_use)
		((ei_placer_param_t*)widget->geom_params)->y = 0;

	if(width)
		((ei_placer_param_t*)widget->geom_params)->width = *width;
	else if(rel_width && first_use)
		((ei_placer_param_t*)widget->geom_params)->width = 0;
	else if(first_use)
		((ei_placer_param_t*)widget->geom_params)->width = widget->requested_size.width;

	if(height)
		((ei_placer_param_t*)widget->geom_params)->height = *height;
	else if(rel_height && first_use)
		((ei_placer_param_t*)widget->geom_params)->height = 0;
	else if(first_use)
		((ei_placer_param_t*)widget->geom_params)->height = widget->requested_size.height;

	if(rel_x)
		((ei_placer_param_t*)widget->geom_params)->rel_x = *rel_x;
	else if(first_use)
		((ei_placer_param_t*)widget->geom_params)->rel_x = 0.0;

	if(rel_y)
		((ei_placer_param_t*)widget->geom_params)->rel_y = *rel_y;
	else if(first_use)
		((ei_placer_param_t*)widget->geom_params)->rel_y = 0.0;

	if(rel_width)
		((ei_placer_param_t*)widget->geom_params)->rel_width = *rel_width;
	else if(first_use)
		((ei_placer_param_t*)widget->geom_params)->rel_width = 0.0;

	if(rel_height)
		((ei_placer_param_t*)widget->geom_params)->rel_height = *rel_height;
	else if(first_use)
		((ei_placer_param_t*)widget->geom_params)->rel_height = 0.0;


	if(widget->geom_params)
		widget->geom_params->manager->runfunc(widget);
}
