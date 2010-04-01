/* 
**  mod_ifdefine_extended.c -- Apache sample ifdefine_extended module
**  [Autogenerated via ``apxs -n ifdefine_extended -g'']
**
**  To play with this sample module first compile it into a
**  DSO file and install it into Apache's modules directory 
**  by running:
**
**    $ apxs -c -i mod_ifdefine_extended.c
**
**  Then activate it in Apache's httpd.conf file for instance
**  for the URL /ifdefine_extended in as follows:
**
**    #   httpd.conf
**    LoadModule ifdefine_extended_module modules/mod_ifdefine_extended.so
**    <Location /ifdefine_extended>
**    SetHandler ifdefine_extended
**    </Location>
**
*/

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "ap_config.h"

#include "json/json.h"

static const char *delimiter = "->";

static struct json_object *json;

/* ap_hook_pre_config */
static int ifdefine_extended_pre_config(apr_pool_t * pconf,
                                        apr_pool_t * plog, apr_pool_t * ptemp)
{
    if (json) {
        json_object_put(json);
        json = NULL;
    }
    return DECLINED;
}

static const char *set_ifdefine_extend_delimiter(cmd_parms * cmd, void *cfg,
                                                 const char *arg)
{
    delimiter = (char *) apr_pstrdup(cmd->pool, arg);
    return NULL;
}

static const char *set_ifdefine_extend_data(cmd_parms * cmd, void *cfg,
                                            const char *path)
{
    if (!json) {
        if ((json = json_object_from_file((char *) path)) == NULL) {
            ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
                         "failed json_object_from_file");
            exit(1);
        }
    }
    return NULL;
}

static int is_define(cmd_parms * cmd, char *expr)
{
    char *key, *last;
    struct json_object *tmp_obj = json;

    if (!expr)
        return false;

    if (!json) {
        ap_log_error(APLOG_MARK, APLOG_ERR, 0, NULL,
                     "json is not initialized");
        return false;
    }

    key = apr_strtok(expr, delimiter, &last);
    while (key) {
        tmp_obj = json_object_object_get(tmp_obj, key);
        if (!tmp_obj)
            break;

        switch (json_object_get_type(tmp_obj)) {
        case json_type_object:
            break;
        case json_type_boolean:
            return json_object_get_boolean(tmp_obj) == TRUE ? true : false;
            break;
        case json_type_null:
            return false;
            break;
        case json_type_int:
            return json_object_get_int(tmp_obj) > 0 ? true : false;
            break;
        case json_type_string:
            /* todo */
            break;
        default:
            break;
        }
        key = apr_strtok(NULL, delimiter, &last);
    }

    return false;
}

static const char *start_if_define_extended(cmd_parms * cmd, void *dummy,
                                            const char *arg)
{
    const char *endp;
    char *expr;
    int define;
    int not = 0;

    endp = ap_strrchr_c(arg, '>');
    if (endp == NULL) {
        return (const char *) apr_pstrcat(cmd->pool, cmd->cmd->name,
                                          "> directive missing closing '>'",
                                          NULL);
    }

    expr = apr_pstrndup(cmd->temp_pool, arg, endp - arg);
    if (expr[0] == '!') {
        not = 1;
        expr++;
    }

    define = is_define(cmd, expr);
    if ((!not && define) || (not && !define)) {
        ap_directive_t *parent = NULL;
        ap_directive_t *current = NULL;
        const char *retval;

        retval = ap_build_cont_config(cmd->pool, cmd->temp_pool, cmd,
                                      &current, &parent, "<IfDefineExtended");
        *(ap_directive_t **) dummy = current;
        return retval;
    }
    else {
        *(ap_directive_t **) dummy = NULL;
        return ap_soak_end_container(cmd, "<IfDefineExtended");
    }
}

static const command_rec ifdefine_extended_commands[] = {
    AP_INIT_TAKE1("<IfDefineExtended", start_if_define_extended, NULL,
                  EXEC_ON_READ | RSRC_CONF,
                  "Container for directives based on existance of specific file"),
    AP_INIT_TAKE1("IfDefineExtendedJSON", set_ifdefine_extend_data, NULL,
                  RSRC_CONF | EXEC_ON_READ,
                  "Path to JSON"),
    AP_INIT_TAKE1("IfDefineExtendedDelimiter", set_ifdefine_extend_delimiter,
                  NULL,
                  RSRC_CONF | EXEC_ON_READ,
                  "Delimiter"),
};

static void register_hooks(apr_pool_t * p)
{
    ap_hook_pre_config(ifdefine_extended_pre_config, NULL, NULL,
                       APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA ifdefine_extended_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                       /* create per-dir    config structures */
    NULL,                       /* merge  per-dir    config structures */
    NULL,                       /* create per-server config structures */
    NULL,                       /* merge  per-server config structures */
    ifdefine_extended_commands, /* table of config file commands       */
    register_hooks,             /* register hooks                      */
};
