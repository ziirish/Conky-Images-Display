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
   *                            cid-callbacks.h
   *                                -------
   *                          Conky Images Display
   *                    --------------------------------
   *
*/
#ifndef __CID_CALLBACKS__
#define __CID_CALLBACKS__

#include <gtk/gtk.h>
#include <gdk/gdkscreen.h>
#include "cid-struct.h"

G_BEGIN_DECLS

/**
 * Fonction exécutée à l'arret du programme 
 * @param p_widget non utilisé
 * @param user_data non utilisé
 */
void _cid_quit (GtkWidget *p_widget, gpointer *user_data);

/**
 * Fonction exécutée en cas d'interruption du programme 
 */
void cid_interrupt (CidMainContainer **pCid);

/**
 * Fonction executée lors d'un clic de souris
 * @param p_widget objet cliqué
 * @param pButton evènement
 */
void on_clic (GtkWidget *p_widget, GdkEventButton* pButton);

/**
 * Fonction executee lors d'un drog'n'drop
 * @param wgt widget that received datas
 * @param seldata dropped data
 */
void on_dragNdrop_data_received (GtkWidget *wgt, 
                                 GdkDragContext *context, 
                                 int x, 
                                 int y,
                                 GtkSelectionData *seldata, 
                                 guint info, 
                                 guint time,
                                 gpointer userdata);

/**
 * Fonction executée lors du survol
 * @param p_widget objet survole
 * @param event evenement
 */                        
void on_motion (GtkWidget *widget, GdkEventMotion *event);

/**
 * Fonction executée pour générer la page "À propos"
 * @param pMenuItem element déclencheur
 * @param data non utilisé
 */
void _cid_about (GtkMenuItem *pMenuItem, gpointer *data);

/**
 * Ajouter une page à la page "À propos"
 * @param pNoteBook sous-page
 * @param cPageLabel label
 * @param cAboutText contenu
 */
static void _cid_add_about_page (GtkWidget *pNoteBook, 
                                 const gchar *cPageLabel, 
                                 const gchar *cAboutText);

/**
 * Fonction executée pour générer l'interface de configuration
 * @param pItemMenu element déclencheur
 * @param data non utilisé
 */
void _cid_conf_panel (GtkMenuItem *pItemMenu, gpointer *data);


/**
 * Vérifie si l'image à afficher existe
 * @param pointeur de données (non utilisé)
 * @return VRAI ou FAUX
 */
gboolean _check_cover_is_present (CidMainContainer **pCid);

gpointer _cid_launch_threaded (gchar *cCommand);


G_END_DECLS
#endif
