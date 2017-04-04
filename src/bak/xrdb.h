#pragma once
#ifdef __cplusplus
extern "C" {
#endif



char * x_get_string_resource (XrmDatabase rdb, char *name, char *a_class);

XrmDatabase x_load_resources (Display *display, char *xrm_string, 
		char *myclass);


#ifdef __cplusplus
}
#endif

