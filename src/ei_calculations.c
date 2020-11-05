/**
 *  @file   ei_calculations.c
 *  @brief  API for all kinds of rects/points operations.
 *
 */

#include "ei_calculations.h"
#include "ei_types.h"


ei_point_t get_point(ei_point_t center, int radius, float angle) {
    ei_point_t point;
    point.x = center.x + radius * cos(angle);
    point.y = center.y + radius * sin(angle);

    return point;
}


ei_linked_point_t *get_last_point(ei_linked_point_t *first){
    ei_linked_point_t *ret = first;

    while(ret->next != NULL)
        ret = ret->next;

    return ret;
}


void ei_free_linked_points_list(ei_linked_point_t *list) {
    ei_linked_point_t *current = list;
    ei_linked_point_t *next = NULL;
    while(current) {
        next = current->next;
        free(current);
        current = next;
    }
}


ei_rect_t get_ei_rect_intersection(ei_rect_t rect1, ei_rect_t rect2){
    /* the intersection between the two given rectangles */
    ei_rect_t intersection;

    /* sets the top left corner o the intersection */
    intersection.top_left.x = max(rect1.top_left.x, rect2.top_left.x);
    intersection.top_left.y = max(rect1.top_left.y, rect2.top_left.y);

    /* sets the width and the height of the intersection */
    intersection.size.width = min(rect1.top_left.x + rect1.size.width, rect2.top_left.x + rect2.size.width) - intersection.top_left.x;
    intersection.size.height = min(rect1.top_left.y + rect1.size.height, rect2.top_left.y + rect2.size.height) - intersection.top_left.y;

    return intersection;
}


void normalize_rect(ei_rect_t *rect, ei_size_t window_size) {
	rect->size.width += min(0, rect->top_left.x);
	rect->size.height += min(0, rect->top_left.y);
	rect->top_left.x = max(0, rect->top_left.x);
	rect->top_left.y = max(0, rect->top_left.y);

	if(rect->top_left.x+rect->size.width >= window_size.width)
		rect->size.width = window_size.width-rect->top_left.x;
	if(rect->top_left.y+rect->size.height >= window_size.height)
		rect->size.height = window_size.height-rect->top_left.y;
}


ei_rect_t extend_rect(ei_rect_t rect) {
    rect.top_left.x -= 2;
    rect.top_left.y -= 2;
    rect.size.width += 4;
    rect.size.height += 4;
    return rect;
}
