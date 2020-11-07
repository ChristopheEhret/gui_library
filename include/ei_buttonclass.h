/**
 *  @file	ei_buttonclass.h
 *  @brief	allocfunc, releasefunc, drawfunc, setdefaultsfunc, geomnotifyfunc of button class.
 *
 */

#ifndef EI_BUTTONCLASS_H
#define EI_BUTTONCLASS_H

#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "ei_event.h"


/**
 * we set the default text size to 40 characters
 */
typedef char 		ei_widget_text_t[40];


/**
 * \brief	The button widget class
 *      attributes are pointers (excpet 'widget' for polymorphism): 
 *      we can know if an attribute has been 
 *      initialised before by testing if it is NULL
 */
typedef struct ei_button_t {
    ei_widget_t        widget;
    ei_color_t*        color;
    int*               border_width;
    int*               corner_radius;
    ei_relief_t*       relief;
    ei_widget_text_t*  text;
    ei_font_t*         text_font;
    ei_color_t*        text_color;
    ei_anchor_t*       text_anchor;
    ei_surface_t*      img;
    ei_rect_t*         img_rect;
    ei_anchor_t*       img_anchor;
    ei_callback_t*     callback;
    void**             user_param;
    ei_bool_t          no_clipping;
    ei_bool_t          is_quit_button;
    ei_bool_t          is_resize_button;
} ei_button_t;


/**
 * \brief	A function that allocates a block of memory that is big enough to store the
 *		attributes of a widget of a class button. After allocation, the function *must*
 *		initialize the memory to 0.
 *
 * @return		A block of memory with all bytes set to 0.
 */
void*       allocbutton		(void);

/**
 * \brief	A function that releases the memory used by a button before it is destroyed.
 *		The \ref ei_widget_t structure itself, passed as parameter, must *not* be freed by
 *		these functions. Can be set to NULL in \ref ei_widgetclass_t if no memory is used by
 *		a class of widget.
 *
 * @param	widget		The button which resources are to be freed.
 */
void		releasebutton	(struct ei_widget_t*	widget);

/**
 * \brief	A function that draws widgets of a class.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		Where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The surface used for picking (i.e. find the widget below the mouse pointer).
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference button).
 */
void	drawbutton		(struct ei_widget_t*		widget,
								ei_surface_t		surface,
								ei_surface_t		pick_surface,
								ei_rect_t*			clipper);

/**
 * \brief	A function that sets the default values for a class.
 *
 * @param	widget		A pointer to the widget instance to initialize.
 */
void	setdefaultsbutton(struct ei_widget_t*	widget);

/**
 * \brief 	A function that is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 */
void	geomnotifybutton	(struct ei_widget_t*	widget);


/**
 * \brief A callback that is used to animate buttons (downward).
 * 
 * @param widget : Set to NULL.
 * @param event : The event information.
 * @param params : Set to NULL.
 * 
 * @return the boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t ei_handle_button_down(ei_widget_t *widget, ei_event_t* event, void* user_param);


/**
 * \brief A callback that is used to animate buttons (upward).
 * 
 * @param widget : Set to NULL.
 * @param event : The event information.
 * @param params : Set to NULL.
 * 
 * @return the boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t ei_handle_button_up(ei_widget_t *widget, ei_event_t* event, void* user_param);


/**
 * \brief the callback that is used to animate buttons. This function handles the exit of the mouse.
 * 
 * @param widget : Set to NULL.
 * @param event : The event information.
 * @param params : Set to NULL.
 * 
 * @return the boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t ei_handle_button_move(ei_widget_t *widget, ei_event_t* event, void* user_param);


#endif
