#ifndef REQUESTHANDLER_LIB_H
#define REQUESTHANDLER_LIB_H
#include<stdlib.h>
#include "HttpMessage.h"

struct Reply *handle_post_request(struct Request* req);
struct Reply * handle_get_request(struct Request* req);


#endif //HTTPMESSAGE_LIB_H
