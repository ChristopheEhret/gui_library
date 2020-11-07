/**
 *  @file	ei_toplevelclass.h
 *  @brief	allocfunc, releasefunc, drawfunc, setdefaultsfunc, geomnotifyfunc of button class.
 *
 */

#ifndef EI_TOPLEVELCLASS_H
#define EI_TOPLEVELCLASS_H

#include "ei_widgetclass.h"
#include "ei_widget.h"
#include "ei_buttonclass.h"
#include "ei_frameclass.h"
#include "ei_event.h"


/**
 * we set the default text size to 40 characters
 */
typedef char 		ei_widget_text_t[40];


/* Toplevel default topbar height */
static int default_topbar_height = 30;


/**
 * \brief	The toplevel widget class
 *      attributes are pointers (except 'widget' for polymorphism): 
 *      we can know if an attribute has been 
 *      initialised before by testing if it is NULL
 */
typedef struct ei_toplevel_t {
    ei_widget_t         widget;
    ei_color_t*         background_color;
    ei_rect_t*          draw_rect;
               
    char**              title;
    ei_font_t*          title_font;

    int*                border_width;
    ei_bool_t*          closable;
    ei_axis_set_t*      resizable;
    ei_size_t*          min_size;
} ei_toplevel_t;


/**
 * \brief	A function that allocates a block of memory that is big enough to store the
 *		attributes of a widget of a class toplevel. After allocation, the function *must*
 *		initialize the memory to 0.
 *
 * @return		A block of memory with all bytes set to 0.
 */
void*       alloctoplevel		(void);

/**
 * \brief	A function that releases the memory used by a toplevel before it is destroyed.
 *		The \ref ei_widget_t structure itself, passed as parameter, must *not* be freed by
 *		these functions. Can be set to NULL in \ref ei_widgetclass_t if no memory is used by
 *		a class of widget.
 *
 * @param	widget		The toplevel which resources are to be freed.
 */
void		releasetoplevel	(struct ei_widget_t*	widget);

/**
 * \brief	A function that draws widgets of the class toplevel.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		Where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The surface used for picking (i.e. find the widget below the mouse pointer).
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference button).
 */
void	drawtoplevel		(struct ei_widget_t*		widget,
								ei_surface_t		surface,
								ei_surface_t		pick_surface,
								ei_rect_t*			clipper);

/**
 * \brief	A function that sets the default values for the class toplevel.
 *
 * @param	widget		A pointer to the widget instance to initialize.
 */
void	setdefaultstoplevel(struct ei_widget_t*	widget);

/**
 * \brief 	A function that is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 */
void	geomnotifytoplevel	(struct ei_widget_t*	widget);




/**
 * \brief The callback to quit a toplevel
 * 
 * @param w_quitbutton The quit button to end the toplevel
 * @param event The event telling to quit
 * @param params Additionnal parameters the user can add
 * 
 * @return The boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t quit_toplevel_event(ei_widget_t* w_quitbutton, ei_event_t* event, void* params);


/**
 * \brief the callback to begin the resize of a toplevel
 * 
 * @param w_quitbutton : the resize button that resizes the toplevel
 * @param event : the event telling to begin the resize
 * @param params : additionnal parameters the user can add
 * 
 * @return the boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t beg_resize(ei_widget_t* w_resizebutton, ei_event_t* event, void* params);


/**
 * \brief the callback to end the resize of a toplevel
 * 
 * @param w_quitbutton : the resize button that resizes the toplevel
 * @param event : the event telling to end the resize
 * @param params : additionnal parameters the user can add
 * 
 * @return the boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t end_resize(ei_widget_t* w_resizebutton, ei_event_t* event, void* params);


/**
 * \brief the callback to resize a toplevel
 * 
 * @param w_quitbutton : the resize button that resizes the toplevel
 * @param event : the event telling to resize
 * @param params : additionnal parameters the user can add : the two firsts parameters
 * 		must be the distance between the mouse and the right side, and the distance
 * 		between the mouse and the bottom side
 * 
 * @return the boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t resize(ei_widget_t* w_resizebutton, ei_event_t* event, void* params);


/**
 * \brief the callback to begin the move of a toplevel
 * 
 * @param w_quitbutton : the toplevel to move
 * @param event : the event telling to begin the move
 * @param params : additionnal parameters the user can add
 * 
 * @return the boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t beg_move(ei_widget_t* w_toplevel, ei_event_t* event, void* params);


/**
 * \brief the callback to move a toplevel
 * 
 * @param w_quitbutton : the toplevel to move
 * @param event : the event telling to move
 * @param params : additionnal parameters the user can add
 * 
 * @return the boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t move(ei_widget_t* w_toplevel, ei_event_t* event, void* params);


/**
 * \brief the callback to end the move a toplevel
 * 
 * @param w_quitbutton : the toplevel to move
 * @param event : the event telling the end of the move
 * @param params : additionnal parameters the user can add
 * 
 * @return the boolean of the callback (true if the event occurs, false otherwise)
 */
ei_bool_t end_move(ei_widget_t* w_toplevel, ei_event_t* event, void* params);


#endif
