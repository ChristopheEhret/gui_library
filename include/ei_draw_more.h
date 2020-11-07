/**
 * @file	ei_draw_more.h
 *
 * @brief 	API for all kinds of drawings.
 * 
 */


#ifndef EI_DRAW_MORE_H
#define EI_DRAW_MORE_H

#include "ei_draw.h"
#include "ei_widget.h"
#include <math.h>


/**
 * \brief Returns the first linked_point of a list of points making an arc,
 *       from \ref beg_angle to \ref end_angle, both must be between 0 and 2pi. 
 *
 * @param   center  center of the arc.
 * @param   radius  radius of the arc (in pixels).
 * @param   beg_angle   beginning angle of the arc, in radians.
 * @param   end_angle   end angle of the arc, in radians.
 * @param   clockwise   true if the arc should go from \ref beg to \ref end clockwise.
 * 
 * @return the arc as a list of points
*/
ei_linked_point_t* ei_arc_points(ei_point_t center, int radius, float beg_angle, float end_angle, ei_bool_t clockwise);


/**
 * \brief Returns the top-left side of a rounded frame
 *
 * @param   rect	The frame rectangle.
 * @param   radius  Radius of the arc (in pixels).
 * 
 * @return A list of points, using ei_arc_points for the rounded frame.
*/
ei_linked_point_t* ei_rounded_frame_high(ei_rect_t rect, int r);


/**
 * \brief Returns the bottom-right side of a rounded frame
 *
 * @param   rect	The frame rectangle.
 * @param   radius  Radius of the arc (in pixels).
 * 
 * @return A list of points, using ei_arc_points for the rounded frame.
*/
ei_linked_point_t* ei_rounded_frame_low(ei_rect_t rect, int r);


/**
 * \brief Returns the combination of ei_rounded_frame_low and ei_rounded_frame_high.
 *
 * @param   rect	The frame rectangle.
 * @param   radius  Radius of the arcs of the corners (in pixels).
 * 
 * @return A list of points representing the rounded frame.
*/
ei_linked_point_t* ei_rounded_frame_all(ei_rect_t rect, int r);


/**
 * \brief	A function that draws widgets of a class frame or button.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		Where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The surface used for picking (i.e. find the widget below the mouse pointer).
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 * @param	isFrame		If true, this function draws a frame (that does not have a corner-radius).
 *						If false, draws a button.
 */
void drawframebuttonclasses(struct ei_widget_t*     widget,
                      ei_surface_t      surface,
                      ei_surface_t      pick_surface,
                      ei_rect_t*        clipper,
                      ei_bool_t         isFrame);

/**
 * \brief Returns a darker color than the one in parameter.
 *
 * @param   color	The color to darken.
 * @return A darker color.
*/
ei_color_t darken_color(ei_color_t color);


/**
 * \brief Returns a lighter color than the one in parameter.
 *
 * @param   color	The color to lighten.
 * @return A lighter color.
*/
ei_color_t lighten_color(ei_color_t color);

#endif
