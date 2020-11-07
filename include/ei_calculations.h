/**
 * @file	ei_calculations.h
 *
 * @brief 	API for all kinds of rects/points operations.
 * 
 */


#ifndef EI_CALCULATIONS_H
#define EI_CALCULATIONS_H

#define min(x, y) (x < y? x : y)
#define max(x, y) (x < y? y : x)
#include "ei_types.h"


/**
 * \brief Gets the point corresponding to the parameter.
 * 
 * @param rect The center point.
 * @param radius The radius (distance between the point and the center).
 * @param angle The angle.
 * @return The corresponding point.
 */
ei_point_t get_point(ei_point_t center, int radius, float angle);


/**
 * \brief Return the last point of a linked_point list
 * 
 * @param list The list.
 * @return The last point.
 */
ei_linked_point_t *get_last_point(ei_linked_point_t *first);


/**
 * \brief Frees a list of linked points.
 * 
 * @param list The list of linked points to free.
 */
void ei_free_linked_points_list(ei_linked_point_t *list);


/**
 * \brief Computes the intersection between 2 ei_rect_t.
 * 
 * @param rect1 The first rect to intersect.
 * @param rect2 The other rect.
 * @return The intersection rect.
 */
ei_rect_t get_ei_rect_intersection(ei_rect_t rect1, ei_rect_t rect2);


/**
 * \brief Makes a ei_rect_t keep its boundaries into the screen
 * 
 * @param rect The rect to update
 * @param window_size The window size
 */
void normalize_rect(ei_rect_t *rect, ei_size_t window_size);


/**
 * \brief Extends a rect with 2 pixels.
 * 
 * @param rect The rect to extend.
 * @return The extended rect.
 */
ei_rect_t extend_rect(ei_rect_t rect);


#endif
