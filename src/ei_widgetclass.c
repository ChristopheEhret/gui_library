/**
 *  @file	ei_widgetclass.c
 *  @brief	Definition and registration of widget classes.
 *
 */

#include "ei_widgetclass.h"
#include "ei_frameclass.h"
#include "ei_buttonclass.h"
#include "ei_toplevelclass.h"

ei_widgetclass_t *widclss_top = NULL;


void ei_widgetclass_register(ei_widgetclass_t* widgetclass) {
	/* frees the memory and returns silent if this widgetclass is already registered */
	if(ei_widgetclass_from_name(widgetclass->name)) {
		free(widgetclass);
		return;
	}

	/* otherwise, adds (at the top of the widgetclass list) this widgetclass */
	if(widclss_top)
		widgetclass->next = widclss_top;
	widclss_top = widgetclass;
}


ei_widgetclass_t* ei_widgetclass_from_name(ei_widgetclass_name_t name) {
	/* returns NULL if no widget class was found */
	if(!widclss_top)
		return NULL;

	/* looking for widgetclass named 'name' in the widgetclass list */
	ei_widgetclass_t *current_wc = widclss_top;
	/* stops when the end is reached (i.e. NULL) or when the two strings match (i.e. same names)*/
	while(current_wc && strcmp(ei_widgetclass_stringname(current_wc->name), ei_widgetclass_stringname(name)))
		current_wc = current_wc->next;

	return current_wc;
}


void ei_frame_register_class(void) {
	/* dynamically allocates memory for the widgetclass to add */
	ei_widgetclass_t *frame_class = calloc(1,sizeof(ei_widgetclass_t));

	/* defines all the attributes of the widgetclass 'frame' */
	strcpy((char*)frame_class->name, "frame");
	frame_class->allocfunc = allocframe;
	frame_class->releasefunc = releaseframe;
	frame_class->drawfunc = drawframe;
	frame_class->setdefaultsfunc = setdefaultsframe;
	frame_class->geomnotifyfunc = geomnotifyframe;

	/* registers the widgetclass that has been defined above */
	ei_widgetclass_register(frame_class);
}


void ei_button_register_class(void) {
	/* dynamically allocates memory for the widgetclass to add */
	ei_widgetclass_t *button_class = calloc(1,sizeof(ei_widgetclass_t));

	/* defines all the attributes of the widgetclass 'button' */
	strcpy((char*)button_class->name, "button");
	button_class->allocfunc = allocbutton;
	button_class->releasefunc = releasebutton;
	button_class->drawfunc = drawbutton;
	button_class->setdefaultsfunc = setdefaultsbutton;
	button_class->geomnotifyfunc = geomnotifybutton;

	/* registers the widgetclass that has been defined above */
	ei_widgetclass_register(button_class);
}


void ei_toplevel_register_class(void) {
	/* dynamically allocates memory for the widgetclass to add */
	ei_widgetclass_t *toplevel_class = calloc(1,sizeof(ei_widgetclass_t));

	/* defines all the attributes of the widgetclass 'button' */
	strcpy((char*)toplevel_class->name, "toplevel");
	toplevel_class->allocfunc = alloctoplevel;
	toplevel_class->releasefunc = releasetoplevel;
	toplevel_class->drawfunc = drawtoplevel;
	toplevel_class->setdefaultsfunc = setdefaultstoplevel;
	toplevel_class->geomnotifyfunc = geomnotifytoplevel;

	/* registers the widgetclass that has been defined above */
	ei_widgetclass_register(toplevel_class);
}
