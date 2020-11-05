/**
 *  @file   ei_event.c
 *  @brief  Allows the binding and unbinding of callbacks to events.
 *
 */

#include "ei_event_more.h"

/* List of all the binded events */
ei_linked_binded_event * top_event_bind = NULL;


void ei_bind(ei_eventtype_t eventtype, ei_widget_t* widget, ei_tag_t tag, ei_callback_t callback, void* user_param) {
    /* Checks if the target is valid */
    if(widget && tag){
        perror("In ei_bind : 'widget' must be NULL if 'tag' isn't.\n");
        return;
    } else if (!widget && !tag) {
        perror("In ei_bind : 'widget' or 'tag' must be not null.\n");
        return;
    }

    /* New binded event */
    ei_linked_binded_event * binded_event = malloc(sizeof(ei_linked_binded_event));

    binded_event->eventtype = eventtype;
    binded_event->widget = widget;
    binded_event->tag = tag;
    binded_event->callback = callback;
    binded_event->user_param = user_param;
    binded_event->pickable = (eventtype >= ei_ev_mouse_buttondown && eventtype < ei_ev_last);
    binded_event->next = NULL;

    /* Inserts new binded event at the end of the linked list */
    if(!top_event_bind) {
        top_event_bind = binded_event;
        return;
    }
    ei_linked_binded_event *current_event = top_event_bind;
    while(current_event->next)
        current_event = current_event->next;

    if(top_event_bind)
        current_event->next = binded_event;
    else
        top_event_bind = binded_event;
}


void ei_unbind(ei_eventtype_t eventtype, ei_widget_t* widget, ei_tag_t tag, ei_callback_t callback, void* user_param) {
    /* Navigator throught binded event linked list */
    ei_linked_binded_event *current_bind = get_top_event_bind();
    /* Previous binded event in the list */
    ei_linked_binded_event *last_bind = NULL;

    /* Go through all the binded events  */
    while(current_bind){
        /* If the wanted event has been found, it is deleted from the list */
        if( current_bind->eventtype == eventtype &&
            current_bind->widget == widget &&
            current_bind->tag == tag &&
            current_bind->callback == callback && 
            current_bind->user_param == user_param) 
        {
            if(last_bind)
                last_bind->next = current_bind->next;
            else
                top_event_bind = current_bind->next;
            free(current_bind);
            return;
        }

        /* Continues throught the list */
        last_bind = current_bind;
        current_bind = current_bind->next;
    }
}


ei_linked_binded_event *get_top_event_bind() {
    return top_event_bind;
}

void destroy_event_binded_to_widget(ei_widget_t * widget) {
    ei_linked_binded_event * last_bind = NULL;
    ei_linked_binded_event * curr_bind = get_top_event_bind();
    ei_linked_binded_event * next_bind;
    
    while(curr_bind){
        next_bind = curr_bind->next;

        if(curr_bind->widget == widget){
            if(!last_bind){
                top_event_bind = curr_bind->next;
            } else {
                last_bind->next = curr_bind->next;
            }

            free(curr_bind);
        }

        last_bind = curr_bind;
        curr_bind = next_bind;
    }
}
