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
   *                    cid-rhythmbox.c
   *                       -------
   *                 Conky Images Display
   *             ----------------------------
   *
   *
   *    Source originale: applet rhythmbox pour Cairo-dock
   *    Auteur origial: Adrien Pilleboue
*/

#include "cid-rhythmbox.h"

#include "../tools/cid-dbus.h"
#include "../tools/cid-utilities.h"

#include "../cid-callbacks.h"

#include "../cid-messages.h"
#include "../cid-struct.h"
#include "../cid-constantes.h"
#include "../cid-cover.h"

extern CidMainContainer *cid;

static DBusGProxy *dbus_proxy_player = NULL;
static DBusGProxy *dbus_proxy_shell = NULL;

gchar *
cid_rhythmbox_cover() 
{
    if (dbus_detect_rhythmbox()) 
    {
        if (rhythmbox_getPlaying ()) 
        {
            rhythmbox_getPlayingUri();
            getSongInfos();
            if (musicData.playing_cover != NULL) 
            {
                cid_set_state_icon();
                return musicData.playing_cover;
            }
        }
    }
    return cid->config->cDefaultImage;
}

gboolean 
rhythmbox_dbus_connect_to_bus (void) 
{
    g_type_init ();
    if (dbus_is_enabled ()) 
    {
        dbus_proxy_player = create_new_session_proxy (
            "org.gnome.Rhythmbox",
            "/org/gnome/Rhythmbox/Player",
            "org.gnome.Rhythmbox.Player"
        );
        
        dbus_proxy_shell = create_new_session_proxy (
            "org.gnome.Rhythmbox",
            "/org/gnome/Rhythmbox/Shell",
            "org.gnome.Rhythmbox.Shell"
        );
        
        dbus_g_proxy_add_signal(dbus_proxy_player, "playingChanged",
            G_TYPE_BOOLEAN,
            G_TYPE_INVALID);
        dbus_g_proxy_add_signal(dbus_proxy_player, "playingUriChanged",
            G_TYPE_STRING,
            G_TYPE_INVALID);
        dbus_g_proxy_add_signal(dbus_proxy_player, "elapsedChanged",
            G_TYPE_UINT,
            G_TYPE_INVALID);
        dbus_g_proxy_add_signal(dbus_proxy_player, "rb:CovertArt-uri",
            G_TYPE_STRING,
            G_TYPE_INVALID);
        
        dbus_g_proxy_connect_signal(dbus_proxy_player, 
                                    "playingChanged",
                                    G_CALLBACK(rb_onChangeState), 
                                    NULL, 
                                    NULL);
            
        dbus_g_proxy_connect_signal(dbus_proxy_player, 
                                    "playingUriChanged",
                                    G_CALLBACK(rb_onChangeSong), 
                                    NULL, 
                                    NULL);
        
        dbus_g_proxy_connect_signal(dbus_proxy_player, 
                                    "elapsedChanged",
                                    G_CALLBACK(rb_onElapsedChanged), 
                                    NULL, 
                                    NULL);
        
        dbus_g_proxy_connect_signal(dbus_proxy_player, 
                                    "rb:CovertArt-uri",
                                    G_CALLBACK(rb_onCovertArtChanged), 
                                    NULL, 
                                    NULL);
        
        return TRUE;
    }
    return FALSE;
}

void 
rhythmbox_dbus_disconnect_from_bus (void) 
{
    if (dbus_proxy_player != NULL) 
    {
        dbus_g_proxy_disconnect_signal(dbus_proxy_player, 
                                       "playingChanged",
                                       G_CALLBACK(rb_onChangeState), 
                                       NULL);
        cid_debug ("playingChanged deconnecte");
        
        dbus_g_proxy_disconnect_signal(dbus_proxy_player, 
                                       "playingUriChanged",
                                       G_CALLBACK(rb_onChangeSong), 
                                       NULL);
        cid_debug ("playingUriChanged deconnecte");
        
        dbus_g_proxy_disconnect_signal(dbus_proxy_player, 
                                       "elapsedChanged",
                                       G_CALLBACK(rb_onElapsedChanged), 
                                       NULL);
        cid_debug ("elapsedChanged deconnecte");
        
        dbus_g_proxy_disconnect_signal(dbus_proxy_player, 
                                    "rb:CovertArt-uri",
                                    G_CALLBACK(rb_onCovertArtChanged), 
                                    NULL);
        cid_debug ("onCovertArtChanged deconnecte");
        
        g_object_unref (dbus_proxy_player);
        dbus_proxy_player = NULL;
    }
    if (dbus_proxy_shell != NULL) 
    {
        g_object_unref (dbus_proxy_shell);
        dbus_proxy_shell = NULL;
    }
}

gboolean 
dbus_detect_rhythmbox(void) 
{
    musicData.opening = dbus_detect_application ("org.gnome.Rhythmbox");
    return musicData.opening;
}


//**********************************************************************
// rhythmbox_getPlaying() : Test si Rhythmbox joue de la musique ou non
//**********************************************************************
gboolean 
rhythmbox_getPlaying (void) 
{
    musicData.playing = dbus_get_boolean (dbus_proxy_player, 
                                          "getPlaying");
    return musicData.playing;
}


//**********************************************************************
// rhythmbox_getPlayingUri() : Retourne l'adresse de la musique jouée
//**********************************************************************
gchar *
rhythmbox_getPlayingUri(void) 
{
    
    g_free (musicData.playing_uri);
    musicData.playing_uri = NULL;
    
    musicData.playing_uri = dbus_get_string (dbus_proxy_player, 
                                             "getPlayingUri");
    return musicData.playing_uri;
}


void 
getSongInfos(void) 
{   
    GHashTable *data_list = NULL;
    GValue *value;
    const gchar *data;
    
    if(dbus_g_proxy_call (dbus_proxy_shell, "getSongProperties", NULL,
                            G_TYPE_STRING, musicData.playing_uri,
                            G_TYPE_INVALID,
                            dbus_g_type_get_map("GHashTable",
                                                G_TYPE_STRING, 
                                                G_TYPE_VALUE),
                            &data_list,
                            G_TYPE_INVALID)) 
    {
        g_free (musicData.playing_artist);
        value = (GValue *) g_hash_table_lookup(data_list, "artist");
        if (value != NULL && G_VALUE_HOLDS_STRING(value)) 
            musicData.playing_artist = 
                    g_strdup (g_value_get_string(value));
        else 
            musicData.playing_artist = NULL;
        cid_message ("  playing_artist <- %s", 
                     musicData.playing_artist);
        
        g_free (musicData.playing_album);
        value = (GValue *) g_hash_table_lookup(data_list, "album");
        if (value != NULL && G_VALUE_HOLDS_STRING(value)) 
            musicData.playing_album = 
                    g_strdup (g_value_get_string(value));
        else 
            musicData.playing_album = NULL;
        cid_message ("  playing_album <- %s", 
                     musicData.playing_album);
        
        g_free (musicData.playing_title);
        value = (GValue *) g_hash_table_lookup(data_list, "title");
        if (value != NULL && G_VALUE_HOLDS_STRING(value)) 
            musicData.playing_title = 
                    g_strdup (g_value_get_string(value));
        else 
            musicData.playing_title = NULL;
        cid_message ("  playing_title <- %s", 
                     musicData.playing_title);
        
        value = (GValue *) g_hash_table_lookup(data_list, 
                                               "track-number");
        if (value != NULL && G_VALUE_HOLDS_UINT(value)) 
            musicData.playing_track = g_value_get_uint(value);
        else 
            musicData.playing_track = 0;
        cid_message ("  playing_track <- %d", 
                     musicData.playing_track);
        
        value = (GValue *) g_hash_table_lookup(data_list, "duration");
        if (value != NULL && G_VALUE_HOLDS_UINT(value)) 
            musicData.playing_duration = g_value_get_uint(value);
        else 
            musicData.playing_duration = 0;
        cid_message ("  playing_duration <- %ds", 
                     musicData.playing_duration);
        
        value = (GValue *) g_hash_table_lookup(data_list, 
                                               "rb:coverArt-uri");
        g_free (musicData.playing_cover);
        musicData.playing_cover = NULL;
        if (value != NULL && G_VALUE_HOLDS_STRING(value)) 
        {
            GError *erreur = NULL;
            const gchar *cString = g_value_get_string(value);
            if (cString != NULL && strncmp (cString, "file://", 7) == 0) 
            {
                musicData.playing_cover = 
                        g_filename_from_uri (cString, NULL, &erreur);
                if (erreur != NULL) 
                {
                    cid_warning ("Attention : %s\n", erreur->message);
                    g_error_free (erreur);
                }
            } 
            else 
            {
                musicData.playing_cover = g_strdup (cString);
            }
        } 
        else 
        {
            // on teste d'abord dans le repertoire de la chanson.
            gchar *cSongPath = 
                    g_filename_from_uri (musicData.playing_uri, 
                                         NULL, 
                                         NULL);  
            if (cSongPath != NULL)
            {
                gchar *cSongDir = g_path_get_dirname (cSongPath);
                g_free (cSongPath);
                
                musicData.playing_cover = 
                        g_strdup_printf(
                            "%s/.gnome2/rhythmbox/covers/%s - %s.jpg", 
                            g_getenv("HOME"),
                            musicData.playing_artist, 
                            musicData.playing_album);
                cid_debug ("   test de %s (.gnome2)", 
                           musicData.playing_cover);
                               
                if (! g_file_test (musicData.playing_cover, 
                                   G_FILE_TEST_EXISTS))
                {
                    g_free (musicData.playing_cover);
                    musicData.playing_cover = 
                        g_strdup_printf(
                            "%s/.cache/rhythmbox/covers/%s - %s.jpg", 
                            g_getenv("HOME"),
                            musicData.playing_artist, 
                            musicData.playing_album);
                    cid_debug ("    test de %s (.cache)", 
                               musicData.playing_cover);
                }
                
                if (! g_file_test (musicData.playing_cover, 
                                   G_FILE_TEST_EXISTS))
                {
                    gchar *cCoverSave = 
                            g_strdup (musicData.playing_cover);
                    g_free (musicData.playing_cover);
                    musicData.playing_cover = 
                            cid_cover_lookup (&cid, 
                                              musicData.playing_artist, 
                                              musicData.playing_album,
                                              cSongDir);
                    
                    if (musicData.playing_cover == NULL)
                    {
                        musicData.playing_cover = g_strdup (cCoverSave);
                    }
                    g_free (cCoverSave);
                }
                g_free (cSongDir);
            }
        }
        cid_message ("  playing_cover <- %s", musicData.playing_cover);
        
        g_hash_table_destroy (data_list);
    } 
    else 
    {
        cid_error ("  can't get song properties");
        g_free (musicData.playing_uri);
        musicData.playing_uri = NULL;
        g_free (musicData.playing_cover);
        musicData.playing_cover = NULL;
    }
}


//**********************************************************************
// rhythmbox_onChangeSong() : Fonction executée à chaque 
// changement de musique
//**********************************************************************
void 
rb_onChangeSong(DBusGProxy *player_proxy,
                const gchar *uri, 
                gpointer data) 
{
    cid_message ("-> %s (%s)",__func__,uri);
    
    g_free (musicData.playing_uri);
    if(uri != NULL && *uri != '\0') 
    {
        musicData.playing_uri = g_strdup (uri);
        musicData.opening = TRUE;
        getSongInfos();
        cid_display_image(cid_rhythmbox_cover());
        cid_animation(cid->config->iAnimationType);
    } 
    else 
    {
        musicData.playing_uri = NULL;
        musicData.cover_exist = FALSE;
        
        g_free (musicData.playing_artist);
        musicData.playing_artist = NULL;
        g_free (musicData.playing_album);
        musicData.playing_album = NULL;
        g_free (musicData.playing_title);
        musicData.playing_title = NULL;
        g_free (musicData.playing_cover);
        musicData.playing_cover = NULL;
        musicData.playing_duration = 0;
        musicData.playing_track = 0;
        
        cid_display_image(cid_rhythmbox_cover());
        cid_animation(cid->config->iAnimationType);

        dbus_detect_rhythmbox();
    }
}

//**********************************************************************
// rhythmbox_onChangeState() : Fonction executée à chaque 
// changement play/pause
//**********************************************************************
void 
rb_onChangeState(DBusGProxy *player_proxy, 
                 gboolean a_playing, 
                 gpointer data) 
{
    cid_message ("-> %s () : %s",__func__,a_playing ? "PLAY" : "PAUSE");
    musicData.playing = a_playing;
    if(! musicData.cover_exist && musicData.playing_uri != NULL) 
    {
//      g_printerr ("  playing_uri : %s\n", musicData.playing_uri);
        if(musicData.playing) 
        {
            cid->runtime->bCurrentlyPlaying = TRUE;
        //  rhythmbox_set_surface (PLAYER_PLAYING);
        } 
        else 
        {
            cid->runtime->bCurrentlyPlaying = FALSE;
        //  rhythmbox_set_surface (PLAYER_PAUSED);
        }
    }
    cid_set_state_icon();
}

//**********************************************************************
// rhythmbox_elapsedChanged() : Fonction executée à chaque 
// changement de temps joué
//**********************************************************************
void 
rb_onElapsedChanged(DBusGProxy *player_proxy,int elapsed, gpointer data) 
{
    if(elapsed > 0) 
    {
        //g_print ("%s () : %ds\n", __func__, elapsed);
/*
        if(myConfig.quickInfoType == MY_APPLET_TIME_ELAPSED)
        {
            CD_APPLET_SET_MINUTES_SECONDES_AS_QUICK_INFO (elapsed)
            CD_APPLET_REDRAW_MY_ICON
        }
        // avec un '-' devant.
        else if(myConfig.quickInfoType == MY_APPLET_TIME_LEFT)  
        {
            CD_APPLET_SET_MINUTES_SECONDES_AS_QUICK_INFO 
                        (elapsed - musicData.playing_duration)
            CD_APPLET_REDRAW_MY_ICON
        }
        else if(myConfig.quickInfoType == MY_APPLET_PERCENTAGE)
        {
            CD_APPLET_SET_QUICK_INFO_ON_MY_ICON_PRINTF 
                ("%d%%", 
                 (int) (100.*elapsed/musicData.playing_duration))
            CD_APPLET_REDRAW_MY_ICON
        }
*/
    }
}


void 
rb_onCovertArtChanged(DBusGProxy *player_proxy,
                      const gchar *cImageURI, 
                      gpointer data) 
{
    cid_debug ("%s (%s)",__func__,cImageURI);
    g_free (musicData.playing_cover);
    musicData.playing_cover = g_strdup (cImageURI);
    cid_display_image(musicData.playing_cover);
}

void 
_playPause_rhythmbox (CidMainContainer **pCid) 
{
    dbus_call_boolean (dbus_proxy_player,
                       "playPause", 
                       !musicData.playing);
}

void 
_next_rhythmbox (CidMainContainer **pCid) 
{
    dbus_call (dbus_proxy_player,"next");
}

void 
_previous_rhythmbox (CidMainContainer **pCid) 
{
    dbus_call (dbus_proxy_player,"previous");
}

void 
_add_to_queue_rhythmbox (gchar *cPath) 
{
    dbus_g_proxy_call_no_reply (dbus_proxy_shell, "addToQueue",
        G_TYPE_STRING,
        cPath,
        G_TYPE_INVALID);
}

void 
cid_build_rhythmbox_menu (void) 
{
    cid->runtime->pMonitorList->p_fPlayPause = _playPause_rhythmbox;
    cid->runtime->pMonitorList->p_fNext = _next_rhythmbox;
    cid->runtime->pMonitorList->p_fPrevious = _previous_rhythmbox;
    cid->runtime->pMonitorList->p_fAddToQueue = _add_to_queue_rhythmbox;
}
