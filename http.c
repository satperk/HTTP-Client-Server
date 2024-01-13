#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "http.h"


/**
 * httprequest_parse_headers
 * 
 * Populate a `req` with the contents of `buffer`, returning the number of bytes used from `buf`.
 */
ssize_t httprequest_parse_headers(HTTPRequest *req, char *buffer, ssize_t buffer_len) {
  char *theRest = malloc(sizeof(char*));
  char *buff = malloc(buffer_len);
  memcpy(buff,buffer,buffer_len);
  char * token = strtok_r(buff, " ", theRest);
  req->action = strdup(token);
  token = strtok_r(NULL, " ",theRest);
  req->path = strdup(token);
  token = strtok_r(NULL, "\r\n", theRest);ow 
  req->version = strdup(token);
  req->head = NULL;
  req->tail = NULL;
  req->payload = NULL; 

  for (size_t i=0; token != NULL; i++) {
    char* to_read = strdup(token);
    if (strstr(to_read,":")) {
      char* key = strtok(to_read,":");
      char* val = strtok(NULL," ");
      if (val == NULL || key == NULL) {
        return sizeof(req->version) + sizeof(req->path) + sizeof(req->action);
      } else {       
        if (req->head != NULL) {
          HeaderMap * to_add =  malloc(sizeof(HeaderMap));
          to_add->value = malloc(strlen(val)+1);
          strcpy(to_add->value, val);
          to_add->key = malloc(strlen(key)+1);
          strcpy(to_add->key, key);
          req->tail->next = to_add;
          req->tail = to_add;
        } else {
          HeaderMap * to_add = malloc(sizeof(HeaderMap));
          to_add->value = malloc(strlen(val)+1);
          strcpy(to_add->value, val);
          to_add->key = malloc(strlen(key)+1);
          strcpy(to_add->key, key);
          req->head = to_add;
          req->tail = to_add;
          req->head->next = req->tail;
          req->tail->next = NULL;
        }
        if (strcmp(key, "Content-Length") == 0) {
          req->payload = strdup(strtok_r(NULL, "\r\n\r\n", theRest));
          return sizeof(req->version) + sizeof(req->path) + sizeof(req->action);
        }   
      }
    }
    token = strtok_r(NULL, "\r\n",theRest);      
  }

  return -1;
}


/**
 * httprequest_read
 * 
 * Populate a `req` from the socket `sockfd`, returning the number of bytes read to populate `req`.
 */
ssize_t httprequest_read(HTTPRequest *req, int sockfd) {
  unsigned int pop_num = 4096;
  char* buff = malloc(pop_num);
  size_t bytesUsed = 0;

  while(1) {      
    ssize_t len = read(sockfd, buff, pop_num);
    bytesUsed += len;
    if (len == 0) {
      return httprequest_parse_headers(req, buff, bytesUsed);
    }  
    if (bytesUsed % pop_num  < 100) {
      buff = realloc(buff, sizeof(buff) + pop_num);
    }
  }
  return httprequest_parse_headers(req, buff, bytesUsed);
}


/**
 * httprequest_get_action
 * 
 * Returns the HTTP action verb for a given `req`.
 */
const char *httprequest_get_action(HTTPRequest *req) {
  return req->action;
}


/**
 * httprequest_get_header
 * 
 * Returns the value of the HTTP header `key` for a given `req`.
 */
const char *httprequest_get_header(HTTPRequest *req, const char *key) {
  for (HeaderMap* curr = req->head; curr != NULL; curr = curr->next) {
    if (strcmp(curr->key, key) == 0) {
      return curr->value;
    }
  }
  return NULL;
}


/**
 * httprequest_get_path
 * 
 * Returns the requested path for a given `req`.
 */
const char *httprequest_get_path(HTTPRequest *req) {
  return req->path;
}


/**
 * httprequest_destroy
 * 
 * Destroys a `req`, freeing all associated memory.
 */
void httprequest_destroy(HTTPRequest *req) {
  /**
  free(req->action);
  free(req->path);
  free(req->payload);
  free(req->version);
  HeaderMap* curr = req -> head;
  while (curr != NULL) {
    HeaderMap* next = curr;
    free(curr);
    curr = next;
  }
  **/
  return;
}
