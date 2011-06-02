/***********************************************************************
*
* Program:
*   Conky Images Display
*
* License :
*  This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License, version 2.
*   If you don't know what that means take a look at:
*      http://www.gnu.org/licenses/licenses.html#GPL
*
* Original idea :
*   Charlie MERLAND, July 2008.
*
***********************************************************************/
/*
   *
   *                             cid-animation.h
   *                                -------
   *                          Conky Images Display
   *             --------------------------------------------
   *
*/
#ifndef __CID_ANIMATION__
#define  __CID_ANIMATION__

#include <gtk/gtk.h>
#include <gdk/gdkscreen.h>


#include "cid-struct.h"

/**
 * Fonction appelee au focus
 */
void cid_focus (GtkWidget *pWidget, GdkEventExpose *event, gpointer *userdata);

/**
 * Fonction appelée par cid_threaded_animation
 */
gboolean cid_rotate_on_changing_song (void *ptr);

/**
 * Fonction appelée pour animer CID
 */
void cid_animation (AnimationType iAnim);

/**
 * Fonction permettant de lancer une animation threadée
 */
void cid_threaded_animation (AnimationType iAnim, gint iDelay);

G_END_DECLS
#endif
