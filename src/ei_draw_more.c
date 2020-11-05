/**
 * @file    ei_draw_more.c
 *
 * @brief   API for all kinds of drawings.
 * 
 */

#define __USE_MISC 1

#include "ei_draw_more.h"
#include "ei_widget.h"
#include "ei_frameclass.h"
#include "ei_buttonclass.h"
#include "ei_toplevelclass.h"
#include "ei_calculations.h"

extern ei_button_t *button_pressed;
extern ei_bool_t pressing_over;


ei_linked_point_t *ei_arc_points(ei_point_t center, int radius, float beg_angle, float end_angle, ei_bool_t clockwise){
    /* Check if the angles are right, depending on the rotation direction */
    if(clockwise && end_angle <= beg_angle)
        return NULL;
    if(!clockwise && end_angle >= beg_angle)
        return NULL;
    
    /* Computes the arc_length */
    float arc_length;
    if(clockwise)
        arc_length = end_angle - beg_angle;
    else 
        arc_length = beg_angle - end_angle;

    /* Computes the number of points for this arc */
    const int point_radius_ratio = 4;
    float arc_ratio = arc_length * M_1_PI / 2;
    int point_nb = point_radius_ratio * radius * arc_ratio;

    /* Sets the pointers */
    ei_linked_point_t *curr_point = calloc(1, sizeof(ei_linked_point_t));
    ei_linked_point_t *first_point = curr_point;

    /* Angle increase in ine step */
    float angle_step = arc_length/(point_nb - 1);
    /* Angle of the current point */
    float angle = beg_angle;

    /* Computes the first point */
    first_point->point = get_point(center, radius, angle);

    /* Computes every other points */
    for(int i = 1; i < point_nb; i++){
        /* Increase angle by one step */
        if(clockwise)
            angle += angle_step;
        else
            angle -= angle_step;

        /* Create and link the next point */
        curr_point->next = calloc(1, sizeof(ei_linked_point_t));
        curr_point = curr_point->next;

        /* Computes current point */
        curr_point->point = get_point(center, radius, angle);
    }

    return first_point;
}


ei_linked_point_t *ei_rounded_frame_high(ei_rect_t rect, int r) {
    /* Checks if the rounded frame can be made with the parameters */ 
    int h = min(rect.size.height, rect.size.width) / 2;
    if(h <= r)
        return NULL;

    /* Moving point used to compute the shape */
    ei_point_t curr_pos = rect.top_left;
    
    /* first point : top left */
    ei_linked_point_t *first = malloc(sizeof(ei_linked_point_t));
    curr_pos.y += r;
    first->point = curr_pos;

    /* second point : bottom left */
    ei_linked_point_t *sec = malloc(sizeof(ei_linked_point_t));
    curr_pos.y += rect.size.height - 2*r;
    sec->point = curr_pos;
    first->next = sec;

    /* Bottom left arc */
    curr_pos.x += r;
    ei_linked_point_t *bottom_left_arc = ei_arc_points(curr_pos, r, M_PI, M_PI_4 * 3, EI_FALSE);
    sec->next = bottom_left_arc;

    /* third point : middle bottom left */
    ei_linked_point_t *third = malloc(sizeof(ei_linked_point_t));
    curr_pos.x += h - r;
    curr_pos.y -= h - r;
    third->point = curr_pos;
    get_last_point(bottom_left_arc)->next = third;

    /* fourth point : middle top right */
    ei_linked_point_t *fourth = malloc(sizeof(ei_linked_point_t));
    curr_pos.x = rect.top_left.x + rect.size.width - h;
    curr_pos.y = rect.top_left.y + h;
    fourth->point = curr_pos;
    third->next = fourth;

    /* Top right arc */
    curr_pos.x += h - r;
    curr_pos.y -= h - r;
    ei_linked_point_t *top_right_arc = ei_arc_points(curr_pos, r, M_PI_4 * 7, M_PI_2 *3, EI_FALSE);
    fourth->next = top_right_arc;

    /* fifth point : top right */
    ei_linked_point_t *fifth = malloc(sizeof(ei_linked_point_t));
    curr_pos.y = rect.top_left.y; 
    fifth->point = curr_pos;
    get_last_point(top_right_arc)->next = fifth;

    /* sixth point : top left */
    ei_linked_point_t *sixth = malloc(sizeof(ei_linked_point_t));
    curr_pos.x = rect.top_left.x + r;
    sixth->point = curr_pos;
    fifth->next = sixth;

    /* Top left arc */
    curr_pos.y += r;
    ei_linked_point_t *top_left_arc = ei_arc_points(curr_pos, r, M_PI_2 * 3, M_PI, EI_FALSE);
    sixth->next = top_left_arc;

    return first;
}

ei_linked_point_t *ei_rounded_frame_low(ei_rect_t rect, int r) {
    /* Checks if the rounded frame can be made with the parameters */ 
    int h = min(rect.size.height, rect.size.width) / 2;
    if(h <= r)
        return NULL;

    /* Moving point used to compute the shape */
    ei_point_t curr_pos = rect.top_left;
    curr_pos.x += rect.size.width;
    curr_pos.y += rect.size.height;
    
    /* first point : bottom right */
    ei_linked_point_t *first = malloc(sizeof(ei_linked_point_t));
    curr_pos.y -= r;
    first->point = curr_pos;

    /* second point : top right */
    ei_linked_point_t *sec = malloc(sizeof(ei_linked_point_t));
    curr_pos.y -= rect.size.height - 2*r;
    sec->point = curr_pos;
    first->next = sec;

    /* Top right arc */ 
    curr_pos.x -= r;
    ei_linked_point_t *top_right_arc = ei_arc_points(curr_pos, r, 2 * M_PI, M_PI_4 * 7, EI_FALSE);
    sec->next = top_right_arc;

    /* third point : middle top right */
    ei_linked_point_t *third = malloc(sizeof(ei_linked_point_t));
    curr_pos.x -= h - r;
    curr_pos.y += h - r;
    third->point = curr_pos;
    get_last_point(top_right_arc)->next = third;

    /* fourth point : middle bottom left */
    ei_linked_point_t *fourth = malloc(sizeof(ei_linked_point_t));
    curr_pos.x = rect.top_left.x + h;
    curr_pos.y = rect.top_left.y + rect.size.height - h;
    fourth->point = curr_pos;
    third->next = fourth;

    /* Bottom left arc */
    curr_pos.x -= h - r;
    curr_pos.y += h - r;
    ei_linked_point_t *bottom_left_arc = ei_arc_points(curr_pos, r, M_PI_4 * 3, M_PI_2, EI_FALSE);
    fourth->next = bottom_left_arc;

    /* fifth point : bottom left */
    ei_linked_point_t *fifth = malloc(sizeof(ei_linked_point_t));
    curr_pos.y += r; 
    fifth->point = curr_pos;
    get_last_point(bottom_left_arc)->next = fifth;

    /* sixth point : bottom right */
    ei_linked_point_t *sixth = malloc(sizeof(ei_linked_point_t));
    curr_pos.x = rect.top_left.x + rect.size.width - r;
    sixth->point = curr_pos;
    fifth->next = sixth;

    /* Bottom right arc */
    curr_pos.y -= r;
    ei_linked_point_t *bottom_right_arc = ei_arc_points(curr_pos, r, M_PI_2, 0, EI_FALSE);
    sixth->next = bottom_right_arc;

    return first;
}

ei_linked_point_t* ei_rounded_frame_all(ei_rect_t rect, int r){
    /* Checks if the rounded frame can be made with the parameters */ 
    int h = min(rect.size.height, rect.size.width) / 2;
    if(h <= r)
        return NULL;

    /* Moving point used to compute the shape */
    ei_point_t curr_pos = rect.top_left;

    /* Top left arc */
    curr_pos.x += r;
    curr_pos.y += r;
    ei_linked_point_t *top_left_arc = ei_arc_points(curr_pos, r, M_PI, M_PI_2 * 3, EI_TRUE);

    /* Top right arc */
    curr_pos.x += rect.size.width - 2*r;
    ei_linked_point_t *top_right_arc = ei_arc_points(curr_pos, r, M_PI_2 * 3, M_PI * 2, EI_TRUE);
    get_last_point(top_left_arc)->next = top_right_arc;

    /* Bottom right arc */
    curr_pos.y += rect.size.height - 2*r;
    ei_linked_point_t *bottom_right_arc = ei_arc_points(curr_pos, r, 0, M_PI_2, EI_TRUE);
    get_last_point(top_right_arc)->next = bottom_right_arc;

    /* Bottom left arc */
    curr_pos.x -= rect.size.width - 2*r;
    ei_linked_point_t *bottom_left_arc = ei_arc_points(curr_pos, r, M_PI_2, M_PI, EI_TRUE);
    get_last_point(bottom_right_arc)->next = bottom_left_arc;

    return top_left_arc;
}

void drawframebuttonclasses(struct ei_widget_t*     widget,
                      ei_surface_t      surface,
                      ei_surface_t      pick_surface,
                      ei_rect_t*        clipper,
                      ei_bool_t         isFrame) {
    /* importing the common variables of the frame or button */
    ei_frame_t *widget_frame = (ei_frame_t*)widget;
    ei_button_t *widget_button = (ei_button_t*)widget;
    int *border_width;
    ei_color_t *color;
    ei_relief_t *relief;
    ei_widget_text_t *text;
    ei_font_t *text_font;
    ei_color_t *text_color;
    ei_anchor_t *text_anchor;
    ei_surface_t *img;
    ei_rect_t *img_rect;
    ei_anchor_t *img_anchor;
    int corner_radius = 0;
    ei_bool_t no_clipping = EI_FALSE;
    if(isFrame) { /* Case 1: it is a frame */
        border_width = widget_frame->border_width;
        color = widget_frame->color;
        relief = widget_frame->relief;
        text = widget_frame->text;
        text_font = widget_frame->text_font;
        text_color = widget_frame->text_color;
        text_anchor = widget_frame->text_anchor;
        img = widget_frame->img;
        img_rect = widget_frame->img_rect;
        img_anchor = widget_frame->img_anchor;
    } else { /* Case 2: it is a button */
        border_width = widget_button->border_width;
        color = widget_button->color;
        relief = widget_button->relief;
        text = widget_button->text;
        text_font = widget_button->text_font;
        text_anchor = widget_button->text_anchor;
        text_color = widget_button->text_color;
        img = widget_button->img;
        img_rect = widget_button->img_rect;
        img_anchor = widget_button->img_anchor;
        if(widget_button->corner_radius)
            corner_radius = *widget_button->corner_radius;
        else
            corner_radius = k_default_button_corner_radius;
        no_clipping = widget_button->no_clipping;
    }
    ei_rect_t final_clipper = widget->screen_location;
    ei_rect_t *border_clipper;
    /* The close buttons of the toplevels don't want to be clipped by the parent's content_rect */
    if(no_clipping)
        border_clipper = widget->parent->parent->content_rect;
    else
        border_clipper = clipper ? clipper : &widget->screen_location;

    /* drawing the widget with relief */
    if(border_width && *border_width && relief) {
        final_clipper = get_ei_rect_intersection(*clipper, *widget->content_rect);

        ei_color_t top_color;
        ei_color_t bottom_color;
        switch(*relief) {
            case ei_relief_none:
                top_color = darken_color(*color);
                bottom_color = darken_color(*color);
                break;
            case ei_relief_raised:
                top_color = lighten_color(*color);
                bottom_color = darken_color(*color);
                break;
            case ei_relief_sunken:
                top_color = darken_color(*color);
                bottom_color = lighten_color(*color);
                break;
        }
        /* Drawing the very widget */
        ei_linked_point_t *high = ei_rounded_frame_high(widget->screen_location, corner_radius);
        ei_linked_point_t *low = ei_rounded_frame_low(widget->screen_location, corner_radius);
        ei_linked_point_t *all = ei_rounded_frame_all(final_clipper, corner_radius);
        ei_draw_polygon(surface, high, top_color, border_clipper);
        ei_draw_polygon(surface, low, bottom_color, border_clipper);
        ei_draw_polygon(surface, all, *color, &final_clipper);
        /* Freeing the points allocated by the rounded_frame functions (for memory's sake) */
        ei_free_linked_points_list(high);
        ei_free_linked_points_list(low);
        ei_free_linked_points_list(all);
    } else {
        ei_linked_point_t *all = ei_rounded_frame_all(widget->screen_location, 0);
        ei_draw_polygon(surface, all, *color, border_clipper);
        ei_free_linked_points_list(all);
    }
    /* filling the pick_surface with the widget's pick_color */
    ei_linked_point_t *pick_pts = ei_rounded_frame_all(widget->screen_location, corner_radius);
    ei_draw_polygon(pick_surface, pick_pts, *widget->pick_color, border_clipper);
    ei_free_linked_points_list(pick_pts);

    if(text) {
        ei_point_t where;
        /* computing text width and text height */
        int tw; int th;
        hw_text_compute_size(*text, *text_font, &tw, &th);
        /* getting surface position and dimension */
        const int sw = widget->content_rect->size.width;
        const int sh = widget->content_rect->size.height;
        const int sx = widget->content_rect->top_left.x;
        const int sy = widget->content_rect->top_left.y;
        /* Computing text location */
        switch(*text_anchor) {
            case ei_anc_none:
            case ei_anc_center:
                where = (ei_point_t){sx+(sw-tw)/2, sy+(sh-th)/2};
                break;
            case ei_anc_north:
                where = (ei_point_t){sx+(sw-tw)/2, sy};
                break;
            case ei_anc_northeast:
                where = (ei_point_t){sx+sw-tw, sy};
                break;
            case ei_anc_east:
                where = (ei_point_t){sx+sw-tw, sy+(sh-th)/2};
                break;
            case ei_anc_southeast:
                where = (ei_point_t){sx+sw-tw, sy+sh-th};
                break;
            case ei_anc_south:
                where = (ei_point_t){sx+(sw-tw)/2, sy+sh-th};
                break;
            case ei_anc_southwest:
                where = (ei_point_t){sx, sy+sh-th};
                break;
            case ei_anc_west:
                where = (ei_point_t){sx, sy+(sh-th)/2};
                break;
            case ei_anc_northwest:
                where = (ei_point_t){sx, sy};
                break;
        }
        if((ei_widget_t*)button_pressed == widget && pressing_over) {
            where.x += *widget_button->border_width*0.65;
            where.y += *widget_button->border_width*0.65;
        }
        ei_draw_text(surface, &where, *text, *text_font, *text_color, &final_clipper);
    } else if(img) {
        ei_point_t where;
        /* getting image width and text height */
        int iw; int ih;
        if(img_rect) {
            iw = img_rect->size.width;
            ih = img_rect->size.height;
        } else {
            ei_size_t s_size = hw_surface_get_size(*img);
            iw = s_size.width;
            ih = s_size.height;
        }
        /* getting surface position and dimension */
        const int sw = widget->content_rect->size.width;
        const int sh = widget->content_rect->size.height;
        const int sx = widget->content_rect->top_left.x;
        const int sy = widget->content_rect->top_left.y;
        switch(*img_anchor) {
            case ei_anc_none:
            case ei_anc_center:
                where = (ei_point_t){sx+(sw-iw)/2, sy+(sh-ih)/2};
                break;
            case ei_anc_north:
                where = (ei_point_t){sx+(sw-iw)/2, sy};
                break;
            case ei_anc_northeast:
                where = (ei_point_t){sx+sw-iw, sy};
                break;
            case ei_anc_east:
                where = (ei_point_t){sx+sw-iw, sy+(sh-ih)/2};
                break;
            case ei_anc_southeast:
                where = (ei_point_t){sx+sw-iw, sy+sh-ih};
                break;
            case ei_anc_south:
                where = (ei_point_t){sx+(sw-iw)/2, sy+sh-ih};
                break;
            case ei_anc_southwest:
                where = (ei_point_t){sx, sy+sh-ih};
                break;
            case ei_anc_west:
                where = (ei_point_t){sx, sy+(sh-ih)/2};
                break;
            case ei_anc_northwest:
                where = (ei_point_t){sx, sy};
                break;
        }
        if((ei_widget_t*)button_pressed == widget && pressing_over) {
            where.x += *widget_button->border_width*0.65;
            where.y += *widget_button->border_width*0.65;
        }
        hw_surface_lock(*img);
        ei_rect_t dst_rect = (ei_rect_t){where, (ei_size_t){iw, ih}};
        dst_rect = get_ei_rect_intersection(dst_rect, final_clipper);
        ei_rect_t src_rect;
        if(img_rect)
            src_rect = (ei_rect_t){img_rect->top_left, dst_rect.size};
        else
            src_rect = (ei_rect_t){{0, 0}, dst_rect.size};
        ei_copy_surface(surface, &dst_rect, *img, &src_rect, EI_FALSE);
        hw_surface_unlock(*img);
    }
}


ei_color_t darken_color(ei_color_t color) {
    ei_color_t new_color = {color.red*0.8, color.green*0.8, color.blue*0.8, color.alpha};
    return new_color;
}

ei_color_t lighten_color(ei_color_t color) {
    uint16_t red = (color.red+50)*1.2;
    uint16_t green = (color.green+50)*1.2;
    uint16_t blue = (color.blue+50)*1.2;
    ei_color_t new_color = {red&0xff00 ? 255 : (uint8_t)red, green&0xff00 ? 255 : (uint8_t)green, blue&0xff00 ? 255 : (uint8_t)blue, color.alpha};
    return new_color;
}
