/**
 *  @file   ei_draw.c
 *  @brief  Graphical primitives to draw lines, polygons, text, and operation of drawing
 *      surfaces.
 *
 */

#include "ei_draw.h"
#include "ei_types.h"
#include "hw_interface.h"
#include "ei_calculations.h"


void ei_draw_text(ei_surface_t surface, const ei_point_t* where, const char* text, const ei_font_t font, ei_color_t color, const ei_rect_t* clipper) {
    /* creating the text surface  and locking it */
    const ei_surface_t txt_surface = hw_text_create_surface(text, font ? font : ei_default_font, color);
    hw_surface_lock(txt_surface);
    ei_rect_t dst_rect = {*where, hw_surface_get_size(txt_surface)};
    if(clipper) {
        /* if there is a clipper, then intersecting the clipper with the destination... */
        dst_rect = get_ei_rect_intersection(dst_rect, *clipper);
        /* ... and the src is also modified to display only the text that is inside the rect */
        ei_rect_t src_rect = {{max(0, dst_rect.top_left.x-where->x), max(0, dst_rect.top_left.y-where->y)}, dst_rect.size};
        /* at last we copy the text surface onto the destination */
        ei_copy_surface(surface, &dst_rect, txt_surface, &src_rect, EI_TRUE);
    } else
        ei_copy_surface(surface, &dst_rect, txt_surface, NULL, EI_TRUE);
    hw_surface_unlock(txt_surface);
    /* we don't need this surface anymore */
    hw_surface_free(txt_surface);
}


uint32_t ei_map_rgba(ei_surface_t surface, const ei_color_t* color) {
    /* channels indexs (red, green, blue, alpha)*/
    int ir, ig, ib, ia;

    /* affects ir, ig, ib, ia to the index of the red, blue, green and alpha channels */
    hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);

    /* int value of the color */
    uint32_t int_color = 0;
    
    /* the red byte */
    int_color += ((uint32_t) color->red) << (8 * ir);
    /* the green byte */
    int_color += ((uint32_t) color->green) << (8 * ig);
    /* the blue byte */
    int_color += ((uint32_t) color->blue) << (8 * ib);
    /* the alpha byte (if ia is -1, the surface does not handle alpha)*/
    int_color += (ia == -1) ? 0 : ((uint32_t) color->alpha) << (8 * ia);

    return int_color;
}


void ei_fill(ei_surface_t surface, const ei_color_t* color, const ei_rect_t* clipper) {
    /* gets the rectangle and the size of the surface */
    ei_rect_t surface_rect = hw_surface_get_rect(surface);
    ei_size_t surface_size = hw_surface_get_size(surface);

    /* int value of the color */
    uint32_t int_color = ei_map_rgba(surface, color); 

    /* pointer to the (0;0) pixel */
    uint32_t* pixel_ptr = (uint32_t *) hw_surface_get_buffer(surface);

    /* case : there is a clipper */
    if(clipper != NULL){
        /* the rectangle that is actually about to be filled (cut by the clipper) */
        
        /* points to the top left corner of the rectangle that is about to be filled */
        pixel_ptr += clipper->top_left.x + (clipper->top_left.y * surface_rect.size.width);
        
        /* changes the color of all the pixel within the rectangle */
        for(int y = 0; y < clipper->size.height; y++){
            for(int x = 0; x < clipper->size.width; x++)
                *(pixel_ptr++) = int_color;

            /* skip the pixels of the next line at the left of the rectangle */
            pixel_ptr += surface_rect.size.width - clipper->size.width;
        }
    } else {
        for(int y = 0; y < surface_size.height; y++) {
            for(int x = 0; x < surface_size.width; x++)
                *(pixel_ptr++) = int_color;
        }
    }
}


int ei_copy_surface(ei_surface_t destination, const ei_rect_t* dst_rect, const ei_surface_t source, const ei_rect_t* src_rect, const ei_bool_t alpha) {
    /* if no dst_rect or src_rect was given, then we take the whole surfaces */
    const ei_rect_t rect1 = {{0, 0}, hw_surface_get_size(source)};
    const ei_rect_t rect2 = {{0, 0}, hw_surface_get_size(destination)};
    if(!src_rect)
        src_rect = &rect1;
    if(!dst_rect)
        dst_rect = &rect2;
    if(dst_rect->size.width!=src_rect->size.width || dst_rect->size.height!=src_rect->size.height)
        return 1;

    /* gets the color configuration of the destination surface */
    int dir;
    int dig;
    int dib;
    int dia;
    hw_surface_get_channel_indices(destination, &dir, &dig, &dib, &dia);
    /* gets the color configuration of the source surface */
    int sir;
    int sig;
    int sib;
    int sia;
    hw_surface_get_channel_indices(source, &sir, &sig, &sib, &sia);

    ei_size_t src_size = hw_surface_get_size(source);
    ei_size_t dst_size = hw_surface_get_size(destination);
    /* pointer to the (0;0) pixel */
    uint32_t* src_ptr = (uint32_t *) hw_surface_get_buffer(source);
    uint32_t* dst_ptr = (uint32_t *) hw_surface_get_buffer(destination);
    src_ptr += src_rect->top_left.x + (src_rect->top_left.y * src_size.width);
    dst_ptr += dst_rect->top_left.x + (dst_rect->top_left.y * dst_size.width);

    /* copying from src_ptr to dst_ptr */
    for(int y = 0; y < src_rect->size.height; y++) {
        for(int x = 0; x < src_rect->size.width; x++) {
            /* decomposing the color at *src_ptr */
            ei_color_t src_color = {(*src_ptr>>(8*sir))&0xff, (*src_ptr>>(8*sig))&0xff, (*src_ptr>>(8*sib))&0xff, (*src_ptr>>(8*sia))&0xff};
            if(alpha) {
                /* if we need alpha, we compute a weighted mean (by the alpha canal of the source pixel) between the source pixel and the destination pixel */
                float cnl = ((float)src_color.alpha)/256;
                ei_color_t dst_color = {(*dst_ptr>>(8*dir))&0xff, (*dst_ptr>>(8*dig))&0xff, (*dst_ptr>>(8*dib))&0xff, (*dst_ptr>>(8*dia))&0xff};
                const ei_color_t color = {src_color.red*cnl+dst_color.red*(1-cnl), src_color.green*cnl+dst_color.green*(1-cnl), src_color.blue*cnl+dst_color.blue*(1-cnl), 255};
                *dst_ptr = ei_map_rgba(destination, &color);
            } else
                *dst_ptr = ei_map_rgba(destination, &src_color);
            dst_ptr++;
            src_ptr++;
        }
        /* jumping one pixel below */
        dst_ptr += dst_size.width - src_rect->size.width;
        src_ptr += src_size.width - src_rect->size.width;
    }
    return 0;
}
