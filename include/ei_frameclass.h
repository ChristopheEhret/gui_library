/**
 *  @file	ei_frameclass.h
 *  @brief	allocfunc, releasefunc, drawfunc, setdefaultsfunc, geomnotifyfunc of frame class.
 *
 */

#ifndef EI_FRAMECLASS_H
#define EI_FRAMECLASS_H

#include "ei_widget.h"
#include "ei_widgetclass.h"


/**
 * we set the default text size to 40 characters
 */
typedef char 		ei_widget_text_t[40];

/**
 * \brief	The frame widget class
 *      attributes are pointers (excpet 'widget' for polymorphism): 
 *      we can know if an attribute has been 
 *      initialised before by testing if it is NULL
 */
typedef struct ei_frame_t {
    ei_widget_t widget;     ///< The widget seen as a frame, for polymorphism.
    int*	border_width;   ///< Border width of the frame.
    ei_relief_t* relief;    ///< Relief of the frame.
    ei_color_t* color;      ///< Color of the frame.

    /* for frames having text */
    ei_widget_text_t* text; ///< Text, array of 40 characters at most.
    ei_font_t* text_font;   ///< Text font.
    ei_color_t* text_color; ///< Text color.
    ei_anchor_t* text_anchor;   ///< Anchor, to know where to put the text.

    /* if frame does not have text, it has an image */
    ei_surface_t* img;      ///< The image.
    ei_rect_t* img_rect;   ///< A subrectangle of the image we want to handle.
    ei_anchor_t* img_anchor;    ///< Anchor, to know where to put the image.
} ei_frame_t;


/**
 * \brief	A function that allocates a block of memory that is big enough to store the
 *		attributes of a widget of a class frame. After allocation, the function *must*
 *		initialize the memory to 0.
 *
 * @return		A block of memory with all bytes set to 0.
 */
void*       allocframe		(void);

/**
 * \brief	A function that releases the memory used by a frame before it is destroyed.
 *		The \ref ei_widget_t structure itself, passed as parameter, must *not* be freed by
 *		these functions. Can be set to NULL in \ref ei_widgetclass_t if no memory is used by
 *		a class of widget.
 *
 * @param	widget		The frame which resources are to be freed.
 */
void		releaseframe	(struct ei_widget_t*	widget);

/**
 * \brief	A function that draws widgets of a class.
 *
 * @param	widget		A pointer to the widget instance to draw.
 * @param	surface		Where to draw the widget. The actual location of the widget in the
 *				surface is stored in its "screen_location" field.
 * @param	pick_surface	The surface used for picking (i.e. find the widget below the mouse pointer).
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void	drawframe		(struct ei_widget_t*		widget,
								ei_surface_t		surface,
								ei_surface_t		pick_surface,
								ei_rect_t*			clipper);

/**
 * \brief	A function that sets the default values for a class.
 *
 * @param	widget		A pointer to the widget instance to initialize.
 */
void	setdefaultsframe(struct ei_widget_t*	widget);

/**
 * \brief 	A function that is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 */
void	geomnotifyframe	(struct ei_widget_t*	widget);



#endif
