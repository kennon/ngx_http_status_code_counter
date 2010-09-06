
/*
 * Copyright (C) Kennon Ballou
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define NGX_HTTP_LAST_LEVEL_500  508
#define NGX_HTTP_NUM_STATUS_CODES (NGX_HTTP_LAST_LEVEL_500 - NGX_HTTP_OK)

/* 
  This is really wasteful to store such a sparse array of status code counts,
  but this will allow new status codes to be counted without having any hardcoding 
*/
ngx_atomic_t ngx_http_status_code_counts[NGX_HTTP_NUM_STATUS_CODES];

static char *ngx_http_set_status_code_counter(ngx_conf_t *cf, ngx_command_t *cmd,
                                 void *conf);

static ngx_int_t ngx_http_status_code_counter_init(ngx_conf_t *cf);

static ngx_command_t  ngx_http_status_code_counter_commands[] = {

    { ngx_string("show_status_code_count"),
      NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_http_set_status_code_counter,
      0,
      0,
      NULL },

      ngx_null_command
};



static ngx_http_module_t  ngx_http_status_code_counter_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_status_code_counter_init,     /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};


ngx_module_t  ngx_http_status_code_counter_module = {
    NGX_MODULE_V1,
    &ngx_http_status_code_counter_module_ctx,      /* module context */
    ngx_http_status_code_counter_commands,              /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_int_t ngx_http_status_code_counter_handler(ngx_http_request_t *r)
{
    size_t             size;
    ngx_int_t          rc, i, j;
    ngx_buf_t         *b;
    ngx_chain_t        out;

    if (r->method != NGX_HTTP_GET && r->method != NGX_HTTP_HEAD) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    rc = ngx_http_discard_request_body(r);

    if (rc != NGX_OK) {
        return rc;
    }

    ngx_str_set(&r->headers_out.content_type, "text/plain");

    if (r->method == NGX_HTTP_HEAD) {
        r->headers_out.status = NGX_HTTP_OK;

        rc = ngx_http_send_header(r);

        if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
            return rc;
        }
    }

    /* count number of seen status codes to determine how much buffer we need */
    j = 0;
    for(i = 0; i < NGX_HTTP_NUM_STATUS_CODES; i++ )
    {
      if(ngx_http_status_code_counts[i] > 0)
      {
        j++;
      }
    }

    size = sizeof("HTTP status code counts:\n")
         + sizeof("XXX \n") + (j * NGX_ATOMIC_T_LEN);

    b = ngx_create_temp_buf(r->pool, size);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    out.buf = b;
    out.next = NULL;

    b->last = ngx_sprintf(b->last, "HTTP status code counts:\n");

    for(i = 0; i < NGX_HTTP_NUM_STATUS_CODES; i++ )
    {
      if(ngx_http_status_code_counts[i] > 0)
      {
        b->last = ngx_sprintf(b->last, "%uA %uA\n", i+NGX_HTTP_OK, ngx_http_status_code_counts[i]);
      }
    }

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = b->last - b->pos;

    b->last_buf = 1;

    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    return ngx_http_output_filter(r, &out);
}


static char *ngx_http_set_status_code_counter(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_status_code_counter_handler;

    return NGX_CONF_OK;
}

ngx_int_t
ngx_http_status_code_count_handler(ngx_http_request_t *r)
{
  if( r->headers_out.status >= NGX_HTTP_OK && r->headers_out.status < NGX_HTTP_LAST_LEVEL_500)
  {
    ngx_http_status_code_counts[r->headers_out.status - NGX_HTTP_OK]++;
  }

  return NGX_OK;
}


static ngx_int_t
ngx_http_status_code_counter_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;
    ngx_int_t i;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_LOG_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_status_code_count_handler;

    for(i = 0; i < NGX_HTTP_NUM_STATUS_CODES; i++) 
    {
      ngx_http_status_code_counts[i] = 0;
    }

    return NGX_OK;
}


