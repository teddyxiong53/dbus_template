#include <errno.h>
#include <gio/gio.h>
#include <glib.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "myservice_gdbus.h"

#define TIMEOUT_INTERVAL  100
#define TIMEOUT_TIMES     100 // timeout time 100*100ms=10s
gint counter = TIMEOUT_TIMES;


static Myservice *ms_proxy = NULL;
static GMainLoop *main_loop = NULL;
static pthread_t main_loop_thread = 0;

static GMainLoop *main_loop_service_ready = NULL;
static void cb_OwnerNameChangedNotify(GObject *object, GParamSpec *pspec, gpointer userdata)
{
  gchar *owner_name = NULL;
  bool DBus_Service_Ready = false;

  owner_name = g_dbus_proxy_get_name_owner((GDBusProxy*)object);
  if (NULL != owner_name) {
    printf( "Owner Name: %s, DBus service is ready!\n", owner_name);
    g_free(owner_name);
    DBus_Service_Ready = true;

    if (main_loop_service_ready != NULL) {
      counter = 1;
    }
  } else {
    printf( "DBus service is NOT ready!\n");
    DBus_Service_Ready = false;
  }
}

bool MyServiceIsReady(void)
{
  gchar *owner_name = NULL;

  owner_name = g_dbus_proxy_get_name_owner((GDBusProxy*)ms_proxy);
  if (NULL != owner_name)
  {
    printf( "Owner Name: %s\n", owner_name);
    g_free(owner_name);
    return true;
  } else {
    printf( "Owner Name is NULL\n");
    return false;
  }
}

gboolean timeout_callback(gpointer arg)
{
  if (--counter == 0) {
    if (main_loop_service_ready != NULL) {
      g_main_loop_quit(main_loop_service_ready);
      printf( "timeout_callback main_loop_Service_Ready quit\n");
    }
    printf( "timeout_callback exit\n");
    return FALSE;
  }

  return TRUE;
}


int InitDbus() {
  GError *error = NULL;
  if (NULL != ms_proxy) {
    printf( "AudioManager has been initialized\n");
    return 0;
  }

  // Connect to dbus
  ms_proxy = myservice_proxy_new_for_bus_sync(
      G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE,
      "myipc.myservice", "/myipc/myservice", NULL,
      &error);

  if (0 == ms_proxy) {
    printf(
               "InitDBusCommunication: Failed to create proxy. Reason: %s.\n",
               error->message);
    g_error_free(error);
    return -1;
  }

  g_signal_connect(ms_proxy, "notify::g-name-owner",
                  G_CALLBACK (cb_OwnerNameChangedNotify), NULL);

  if (!MyServiceIsReady()) {
    printf( "main_loop_Service_Ready enter\n");
    g_timeout_add(TIMEOUT_INTERVAL, timeout_callback, NULL);

    main_loop_service_ready = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop_service_ready);
    g_main_loop_unref(main_loop_service_ready);
    printf( "main_loop_Service_Ready exit\n");
  }
  
  return 0;
}
static void help()
{
    printf("./myservice_cmd \"{\"cmd\": \"set\", \"type\": \"volume\", \"value\": 100.0}\" \n");
}

int cmd_process(char *json_str)
{
    cJSON *root;
    root = cJSON_Parse(json_str);
    if (!root) {
        printf("str json parse fail: %s\n", json_str);
        return -1;
    }

    cJSON_Delete(root);
}
int main(int argc, char const *argv[])
{
    if (argc < 2) {
        help();
        return -1;
    }
    InitDbus();

    return 0;
}
