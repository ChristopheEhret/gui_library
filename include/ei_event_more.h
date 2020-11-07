/**
 *  @file	ei_event_more.h
 *  @brief	Extension of the ei_event.h header.
 *
 */

#ifndef EI_EVENT_MORE_H
#define EI_EVENT_MORE_H

#include "ei_event.h"

/* Event binded by ei_bind. Registered in a linked list */
typedef struct ei_linked_binded_event {
    ei_eventtype_t eventtype;
    ei_widget_t *widget;
    ei_tag_t tag;
    ei_callback_t callback;
    void * user_param;
    ei_bool_t pickable;

    struct ei_linked_binded_event *next;
} ei_linked_binded_event;


/**
 * \brief Returns the first binded event of the list of binded events.
 * @return The first binded event.
*/
ei_linked_binded_event *get_top_event_bind();


/**
 * \brief Destroys the events binded to the button.
 * @param The widget to "isolate".
*/
void destroy_event_binded_to_widget(ei_widget_t *widget);

#endif
