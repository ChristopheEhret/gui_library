/**
 *  @file	ei_placermanager.c
 *  @brief	Manages the positioning and sizing of widgets on the screen with placer rules.
 *
 */

#include "ei_placermanager.h"
#include "ei_calculations.h"

void runplacer(struct ei_widget_t* widget) {
    /* returns if the widget does not exist or if it is not managed by a placer */
    if(!widget || !(widget->geom_params) || strcmp(widget->geom_params->manager->name, "placer"))
        return;
    
    ei_rect_t old_screen_location = widget->screen_location;

    /* Reocurring variables */ 
    ei_rect_t parent_sl = *widget->parent->content_rect;
    ei_placer_param_t *params = (ei_placer_param_t *) widget->geom_params;

    /* Computes initial position, as if the widget was anchored in the parent's north-west */
    int x = parent_sl.size.width  * params->rel_x + params->x + parent_sl.top_left.x;
    int y = parent_sl.size.height * params->rel_y + params->y + parent_sl.top_left.y;

    /* Computes the size of the widget */ 
    int height = parent_sl.size.height * params->rel_height + params->height;
    if(height == 0)
        height = widget->requested_size.height;

    int width = parent_sl.size.width * params->rel_width + params->width;
    if(width == 0)
        width = widget->requested_size.width;

    /* sets the anchor : default is the top left corner */
    switch (params->anchor) {
        case ei_anc_center:
            x -= width/2;
            y -= height/2;
            break;
        case ei_anc_north:
            x -= width/2;
            break;
        case ei_anc_northeast:
            x -= width;
            break;
        case ei_anc_east:
            x -= width;
            y -= height/2; 
            break;
        case ei_anc_southeast:
            x -= width;
            y -= height; 
            break;
        case ei_anc_south:
            x -= width/2;
            y -= height; 
            break;
        case ei_anc_southwest:
            y -= height; 
            break;
        case ei_anc_west:
            y -= height/2;
            break;
        case ei_anc_northwest:
        default:
            break;
    }
    
    /* Updates widget's screen_location */
    widget->screen_location.top_left = (ei_point_t) {x, y};
    widget->screen_location.size =  (ei_size_t) {width, height};

    if( widget->screen_location.top_left.x != old_screen_location.top_left.x ||
        widget->screen_location.top_left.y != old_screen_location.top_left.y ||
        widget->screen_location.size.width != old_screen_location.size.width ||
        widget->screen_location.size.height != old_screen_location.size.height) {
            ei_rect_t inv_rect = extend_rect(old_screen_location);
            ei_app_invalidate_rect(&inv_rect);
            widget->wclass->geomnotifyfunc(widget);
    }
}
