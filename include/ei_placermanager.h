/**
 *  @file	ei_placermanager.h
 *  @brief	Manages the positioning and sizing of widgets on the screen with placer rules.
 *
 */

#ifndef EI_PLACERMANAGER_H
#define EI_PLACERMANAGER_H

#include "ei_widget.h"
#include "ei_geometrymanager.h"
#include "ei_application.h"


/**
 * \brief	A function that runs the geometry computation for this widget. This may trigger
 *		geometry computation for this widget's parent and the other children of the parent.
 *
 * @param	widget		The widget instance for which to compute geometry.
 */
void	runplacer		(struct ei_widget_t*	widget);


/**
 * \brief 	A structure that stores information about the geometry manager managing a widget,
 *		and the widget's geometry management parameters.
 */
typedef struct ei_placer_param_t {
	ei_geometrymanager_t*	manager;	/**< Points to the geometry manager's structure 	*/
	ei_anchor_t				anchor;
	int						x;
	int						y;
	int						width;
	int						height;
	float					rel_x;
	float					rel_y;
	float					rel_width;
	float					rel_height;
} ei_placer_param_t;


#endif
