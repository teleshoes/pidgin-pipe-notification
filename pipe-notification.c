/*
 * Pipe Notification
 * Copyright (C) 2008  Armin Preiml
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Armin Preiml <b.nutzer@gmail.com>
 *
 * set ts=4
 */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef PURPLE_PLUGINS
# define PURPLE_PLUGINS
#endif

#include <glib.h>
#include <string.h>

#include "notify.h"
#include "plugin.h"
#include "pluginpref.h"
#include "prefs.h"
#include "version.h"
#include "status.h"
#include "savedstatuses.h"
#include "signals.h"

#include "gtkconv.h"


char * get_current_status_name( void ) {
    PurpleSavedStatus *current;

    if ( purple_prefs_get_bool("/plugins/gtk/pipe-notification/showstates") ) {
        current = (PurpleSavedStatus *)purple_savedstatus_get_current();
        return (char *)purple_savedstatus_get_title(current);
    } else {
        return "On";
    }
}

static void set_status(char *status) {
        FILE *pipe;

        char *home;
        char plugindir[] = "/.purple/plugins/pipe";
        char *pipedir;


        home = getenv("HOME");

        pipedir = (char *)malloc(sizeof(char) * (strlen(home) + strlen(plugindir) + 1));

        strncpy(pipedir,home,strlen(home)+1);
        strcat(pipedir,plugindir);

        if ( (pipe = fopen(pipedir,"w")) < 0 ) {
                printf("Can't open file.");
                exit(EXIT_FAILURE);
        } else {
            fwrite(status, sizeof(char), strlen(status), pipe);

            fclose(pipe);
        }
}



GList *get_pending_list(guint max) {

    const char *im=purple_prefs_get_string("/plugins/gtk/pipe-notification/im");
    const char *chat=purple_prefs_get_string("/plugins/gtk/pipe-notification/chat");


    GList *l_im = NULL;
    GList *l_chat = NULL;

    if (im != NULL && strcmp(im, "always") == 0) {
        l_im = pidgin_conversations_find_unseen_list(PURPLE_CONV_TYPE_IM,
                                                     PIDGIN_UNSEEN_TEXT,
                                                     FALSE, max);
    } else if (im != NULL && strcmp(im, "hidden") == 0) {
        l_im = pidgin_conversations_find_unseen_list(PURPLE_CONV_TYPE_IM,
                                                     PIDGIN_UNSEEN_TEXT,
                                                     TRUE, max);
    }

    if (chat != NULL && strcmp(chat, "always") == 0) {
        l_chat = pidgin_conversations_find_unseen_list(PURPLE_CONV_TYPE_CHAT,
                                                       PIDGIN_UNSEEN_TEXT,
                                                       FALSE, max);
    } else if (chat != NULL && strcmp(chat, "nick") == 0) {
        l_chat = pidgin_conversations_find_unseen_list(PURPLE_CONV_TYPE_CHAT,
                                                       PIDGIN_UNSEEN_NICK,
                                                       FALSE, max);
    }


    if (l_im != NULL && l_chat != NULL)
        return g_list_concat(l_im, l_chat);
    else if (l_im != NULL)
        return l_im;
    else
        return l_chat;
}


static void conversation_updated(PurpleConversation *conv,
                                        PurpleConvUpdateType type) {
    GList *list;

    if( type != PURPLE_CONV_UPDATE_UNSEEN ) {
        return;
    }

    list=get_pending_list(1);

    if(list!=NULL) {
        set_status("New Message");
    } else {
        set_status(get_current_status_name());
    }
    g_list_free(list);
}

static void
account_status_changed(PurpleAccount *account, PurpleStatus *old, PurpleStatus *new,
                                                gpointer data)
{
    if ( purple_prefs_get_bool("/plugins/gtk/pipe-notification/showstates") ) {
        set_status((char *)purple_status_get_name(new));
    } else {
        set_status(get_current_status_name());
    }
}


static PurplePluginPrefFrame *
get_plugin_pref_frame(PurplePlugin *plugin) {
        PurplePluginPrefFrame *frame;
           PurplePluginPref *ppref;

        frame = purple_plugin_pref_frame_new();

        ppref = purple_plugin_pref_new_with_label("Select if all states should be shown, if not only On, Off and New Message will be activated");
        purple_plugin_pref_frame_add(frame,ppref);

        ppref = purple_plugin_pref_new_with_name_and_label("/plugins/gtk/pipe-notification/showstates",
                                                           "Show all states ( You'll need to restart pidgin or reload the plugin if you change this )");
        purple_plugin_pref_frame_add(frame, ppref);

        ppref = purple_plugin_pref_new_with_label("Inform about unread ...");
        purple_plugin_pref_frame_add(frame,ppref);

        ppref = purple_plugin_pref_new_with_name_and_label(
                                                        "/plugins/gtk/pipe-notification/im",
                                                        "Instant Messages");
        purple_plugin_pref_set_type(ppref, PURPLE_PLUGIN_PREF_CHOICE);
        purple_plugin_pref_add_choice(ppref, "Never", "never");
        purple_plugin_pref_add_choice(ppref, "In hidden conversations", "hidden");
        purple_plugin_pref_add_choice(ppref, "Always", "always");
        purple_plugin_pref_frame_add(frame, ppref);

        ppref = purple_plugin_pref_new_with_name_and_label(
                                                        "/plugins/gtk/pipe-notification/chat",
                                                        "Chat Messages");
        purple_plugin_pref_set_type(ppref, PURPLE_PLUGIN_PREF_CHOICE);
        purple_plugin_pref_add_choice(ppref, "Never", "never");
        purple_plugin_pref_add_choice(ppref, "When my nick is said", "nick");
        purple_plugin_pref_add_choice(ppref, "Always", "always");
        purple_plugin_pref_frame_add(frame, ppref);

        return frame;
}


static gboolean plugin_load(PurplePlugin *plugin) {
    purple_signal_connect(purple_accounts_get_handle(), "account-status-changed",
                                                plugin, PURPLE_CALLBACK(account_status_changed), NULL);


    purple_signal_connect(purple_conversations_get_handle(),
                          "conversation-updated", plugin,
                          PURPLE_CALLBACK(conversation_updated), NULL);

    set_status(get_current_status_name());

    return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin) {
    purple_signal_disconnect(purple_accounts_get_handle(), "account-status-changed",
                                                plugin, PURPLE_CALLBACK(account_status_changed));

    purple_signal_disconnect(purple_conversations_get_handle(),
                             "conversation-updated", plugin,
                             PURPLE_CALLBACK(conversation_updated));
    set_status("Off");
    return TRUE;
}

void plugin_destroy(PurplePlugin *plugin) {
    set_status("Off");
}

static PurplePluginUiInfo prefs_info = {
        get_plugin_pref_frame,
        0,   /* page_num (Reserved) */
        NULL, /* frame (Reserved) */
        /* Padding */
        NULL,
        NULL,
        NULL,
        NULL
};

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC,
    PURPLE_MAJOR_VERSION,
    PURPLE_MINOR_VERSION,
    PURPLE_PLUGIN_STANDARD,
    NULL,
    0,
    NULL,
    PURPLE_PRIORITY_DEFAULT,

    "core-pipe_notification",
    "Pipe Notification",
    "0.2",

    "Writes your current status in a fifo pipe",
    "Writes your current status in a fifo pipe",
    "ape <b.nutzer@gmail.com>",
    "http://code.google.com/p/pipe-notification/",

    plugin_load,
    plugin_unload,
    plugin_destroy,

    NULL,
    NULL,
    &prefs_info,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static void
init_plugin(PurplePlugin *plugin)
{
    purple_prefs_add_none("/plugins/gtk/pipe-notification");
    purple_prefs_add_string("/plugins/gtk/pipe-notification/im", "always");
    purple_prefs_add_string("/plugins/gtk/pipe-notification/chat", "nick");

    //if disabled, only On, Off and New Message will be shown
    purple_prefs_add_bool("/plugins/gtk/pipe-notification/showstates",TRUE);
}

PURPLE_INIT_PLUGIN(hello_world, init_plugin, info)

