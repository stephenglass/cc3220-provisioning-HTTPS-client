/*
 * http_push.c
 *
 *  Created on: Feb 28, 2019
 *      Author: Stephen Glass
 * 
 *  Description: HTTPS Client POST application
 */

/* BSD support */
#include "string.h"
#include <uart_term.h>
#include <ti/net/http/httpclient.h>
#include "semaphore.h"

/* Enable UART Log */
#define LOG_MESSAGE_ENABLE
#define LOG_MESSAGE UART_PRINT

#define HOSTNAME              "stephen.glass:443"
#define REQUEST_URI           "/monitor/testPost.php"

#define USER_AGENT            "HTTPClient (ARM; TI-RTOS)"
#define HTTP_MIN_RECV         (256)

#define SSL_CA_CERT           "geo1.der"

#define POST_DATA             "{\n\"name\":\"xyz\",\n\"address\":\n{\n\"plot#\":12,\n\"street\":\"abc\",\n\"city\":\"ijk\"\n},\n\"age\":30\n}"

void printError(char *errString, int code) {
    LOG_MESSAGE("Error! code = %d, Description = %s\n", code, errString);
    while (1) {
        ;
    }
}

/* 
Performs a HTTPS POST using secure SSL protocol
*/
void* httpPostTask(void *pvParameters) {
    bool moreDataFlag = false;
    char data[HTTP_MIN_RECV];
    int16_t ret = 0;
    int16_t len = 0;

    LOG_MESSAGE("Sending a HTTPS POST request to '%s'\r\n", HOSTNAME);

    HTTPClient_Handle httpClientHandle;
    int16_t statusCode;
    httpClientHandle = HTTPClient_create(&statusCode,0);
    if(statusCode < 0)
    {
        printError("httpTask: creation of https client handle failed", statusCode);
    }

    ret = HTTPClient_setHeader(httpClientHandle,
                             HTTPClient_HFIELD_REQ_USER_AGENT,
                             USER_AGENT,strlen(USER_AGENT)+1,
                             HTTPClient_HFIELD_PERSISTENT);

    //ret = HTTPClient_setHeader(httpClientHandle, HTTPClient_HFIELD_REQ_CONTENT_TYPE,con_type,sizeof(con_type),HTTPClient_HFIELD_PERSISTENT);

    if(ret < 0)
    {
        printError("httpTask: setting request header failed", ret);
    }

    HTTPClient_extSecParams httpClientSecParams;
    httpClientSecParams.rootCa = SSL_CA_CERT;
    httpClientSecParams.clientCert = NULL;
    httpClientSecParams.privateKey = NULL;


    ret = HTTPClient_connect(httpClientHandle,HOSTNAME,&httpClientSecParams,0);
    if(ret < 0)
    {
        printError("httpTask: connect failed", ret);
    }
    ret = HTTPClient_sendRequest(httpClientHandle,HTTP_METHOD_POST,REQUEST_URI,
                               POST_DATA,sizeof(POST_DATA),
                               0);
    if(ret < 0)
    {
        printError("httpTask: send failed", ret);
    }

    if(ret != HTTP_SC_OK)
    {
        printError("httpTask: cannot get status", ret);
    }

    LOG_MESSAGE("HTTPS Response Code: %d\r\n", ret);

    len = 0;
    do
    {
        ret = HTTPClient_readResponseBody(httpClientHandle, data, sizeof(data),
                                          &moreDataFlag);
        if(ret < 0)
        {
            printError("httpTask: response body processing failed", ret);
        }
        LOG_MESSAGE("%.*s \r\n", ret, data);
        len += ret;
    }
    while(moreDataFlag);

    LOG_MESSAGE("Received %d bytes of payload\r\n", len);

    ret = HTTPClient_disconnect(httpClientHandle);
    if(ret < 0)
    {
        printError("httpTask: disconnect failed", ret);
    }

    HTTPClient_destroy(httpClientHandle);
    return(0);
}
