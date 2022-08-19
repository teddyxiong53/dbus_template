#include <assert.h>
#include <gio/gio.h>
#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "myservice_gdbus.h"
#define null_str "NULL"


static Myservice *skeleton = NULL;
static GMainLoop *main_loop = NULL;
#define GDBUS_API __attribute__((visibility("hidden")))
GDBUS_API void GBusAcquired_Callback(GDBusConnection *connection,
                                     const gchar *name, gpointer user_data);

GDBUS_API void GBusNameAcquired_Callback(GDBusConnection *connection,
                                         const gchar *name, gpointer user_data);

GDBUS_API void GBusNameLost_Callback(GDBusConnection *connection,
                                     const gchar *name, gpointer user_data);

static gboolean on_handle_set(Myservice *object,
                                             GDBusMethodInvocation *invocation,
                                             const gchar *arg_settings) {
  gint reply;
  printf( "Method call: SetInputSettings\n%s\n", arg_settings);
  reply = 0;
  myservice_complete_set(object, invocation, reply);

  return TRUE;
}

static gboolean on_handle_get(Myservice *object,
                                             GDBusMethodInvocation *invocation,
                                             const gchar *arg_input) {
  gint reply;
  gchar *settings = NULL;

  printf( "Method call: \n%s\n", arg_input);

  reply = 0;
  if (settings) {
    myservice_complete_get(object, invocation, settings,
                                             reply);
    free(settings);
  } else
    myservice_complete_get(object, invocation, null_str,
                                             reply);

  return TRUE;
}

void GBusAcquired_Callback(GDBusConnection *connection, const gchar *name,
                           gpointer user_data) {
  GError *error = NULL;


  skeleton = myservice_skeleton_new();
    // install the method
  g_signal_connect(skeleton, "handle-set",
                   G_CALLBACK(on_handle_set), NULL);
  g_signal_connect(skeleton, "handle-get",
                   G_CALLBACK(on_handle_get), NULL);
  g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(skeleton),
                                   connection, "/myservice",
                                   &error);
}
void GBusNameAcquired_Callback(GDBusConnection *connection, const gchar *name,
                               gpointer user_data) {
  printf( "GBusNameAcquired_Callback has been invoked\n");
}

void GBusNameLost_Callback(GDBusConnection *connection, const gchar *name,
                           gpointer user_data) {
  printf("GBusNameLost_Callback has been invoked\n");
}


int main(int argc, char const *argv[])
{
  char *s = "myservice";
    guint own_id = g_bus_own_name(
      G_BUS_TYPE_SESSION, "myipc.myservice",
      G_BUS_NAME_OWNER_FLAGS_NONE, GBusAcquired_Callback,
      GBusNameAcquired_Callback, GBusNameLost_Callback, (gpointer)s, NULL);
    
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    g_bus_unown_name(own_id);
    return 0;
}
