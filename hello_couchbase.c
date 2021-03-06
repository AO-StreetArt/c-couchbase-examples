#include <libcouchbase/couchbase.h>
#include <stdio.h>
#include <stdlib.h>

static void storage_callback(lcb_t instance, const void *cookie, lcb_storage_t op, 
   lcb_error_t err, const lcb_store_resp_t *resp)
{
  printf("Stored %.*s\n", (int)resp->v.v0.nkey, (char*)resp->v.v0.key);
}

static void get_callback(lcb_t instance, const void *cookie, lcb_error_t err, 
   const lcb_get_resp_t *resp)
{
  printf("Retrieved key %.*s\n", (int)resp->v.v0.nkey, (char*)resp->v.v0.key);
  printf("Value is %.*s\n", (int)resp->v.v0.nbytes, (char*)resp->v.v0.bytes);
}

int main(void)
{

  // initializing
  
  struct lcb_create_st cropts;
  cropts.version = 3;
  cropts.v.v3.connstr = "couchbase://localhost/default";
  lcb_error_t err;
  lcb_t instance;
  err = lcb_create(&instance, &cropts);
  if (err != LCB_SUCCESS) {
    printf("Couldn't create instance!\n");
    exit(1);
  }
  
  // connecting
  
  lcb_connect(instance);
  lcb_wait(instance);
  if ( (err = lcb_get_bootstrap_status(instance)) != LCB_SUCCESS ) {
    printf("Couldn't bootstrap!\n");
    exit(1);
  }
  
  // installing callbacks
  
  lcb_set_store_callback(instance, storage_callback);
  lcb_set_get_callback(instance, get_callback);
  
  
  // scheduling operations
  
  lcb_store_cmd_t scmd;
  const lcb_store_cmd_t *scmdlist = &scmd;
  scmd.v.v0.key = "Hello";
  scmd.v.v0.nkey = 5;
  scmd.v.v0.bytes = "Couchbase";
  scmd.v.v0.nbytes = 9;
  scmd.v.v0.operation = LCB_SET;
  err = lcb_store(instance, NULL, 1, &scmdlist);
  if (err != LCB_SUCCESS) {
    printf("Couldn't schedule storage operation!\n");
    exit(1);
  }
  lcb_wait(instance); //storage_callback is invoked here
  
  lcb_get_cmd_t gcmd;
  const lcb_get_cmd_t *gcmdlist = &gcmd;
  gcmd.v.v0.key = "Hello";
  gcmd.v.v0.nkey = 5;
  err = lcb_get(instance, NULL, 1, &gcmdlist);
  if (err != LCB_SUCCESS) {
    printf("Couldn't schedule get operation!\n");
    exit(1);
  }
  lcb_wait(instance); // get_callback is invoked here
  lcb_destroy(instance);
  return 0;
}
