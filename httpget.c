/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== httpget.c ========
 *  HTTP Client GET example application
 */

/* BSD support */
#include "string.h"
#include <ti/display/Display.h>
#include <ti/net/http/httpclient.h>
#include "semaphore.h"

#define HOSTNAME              "stephen.glass:443"
#define REQUEST_URI           "/monitor/testPost.php"
#define USER_AGENT            "HTTPClient (ARM; TI-RTOS)"
#define HTTP_MIN_RECV         (256)

#define SL_SSL_CA_CERT        "geo1.der"

#define headerf   "Cache-Control: no-cache\r\n Content-Type: application/json\r\n"
#define con_type "application/json\r\n"

#define POST_DATA             "{\n\"name\":\"xyz\",\n\"address\":\n{\n\"plot#\":12,\n\"street\":\"abc\",\n\"city\":\"ijk\"\n},\n\"age\":30\n}"

//extern Display_Handle display;
Display_Handle display;
extern sem_t ipEventSyncObj;
//extern void printError(char *errString, int code);

void printError(char *errString,
                int code)
{
    Display_printf(display, 0, 0, "Error! code = %d, Description = %s\n", code,
                   errString);
    while(1)
    {
        ;
    }
}

/*
 *  ======== httpTask ========
 *  Makes a HTTP GET request
 */
void* httpTask(void* pvParameters)
{

    bool moreDataFlag = false;
    char data[HTTP_MIN_RECV];
    int16_t ret = 0;
    int16_t len = 0;

    Display_printf(display, 0, 0, "Sending a HTTP POST request to '%s'\n",
                   HOSTNAME);

    HTTPClient_Handle httpClientHandle;
    int16_t statusCode;
    httpClientHandle = HTTPClient_create(&statusCode,0);
    if(statusCode < 0)
    {
        printError("httpTask: creation of https client handle failed",
                   statusCode);
    }

    ret =
        HTTPClient_setHeader(httpClientHandle,
                             HTTPClient_HFIELD_REQ_USER_AGENT,
                             USER_AGENT,strlen(USER_AGENT)+1,
                             HTTPClient_HFIELD_PERSISTENT);

    //ret = HTTPClient_setHeader(httpClientHandle, HTTPClient_HFIELD_REQ_CONTENT_TYPE,con_type,sizeof(con_type),HTTPClient_HFIELD_PERSISTENT);

    if(ret < 0)
    {
        printError("httpTask: setting request header failed", ret);
    }

    HTTPClient_extSecParams httpClientSecParams;
    httpClientSecParams.rootCa = SL_SSL_CA_CERT;
    httpClientSecParams.clientCert = NULL;
    httpClientSecParams.privateKey = NULL;


    ret = HTTPClient_connect(httpClientHandle,HOSTNAME,&httpClientSecParams,0);
    if(ret < 0)
    {
        printError("httpTask: connect failed", ret);
    }
    ret =
        HTTPClient_sendRequest(httpClientHandle,HTTP_METHOD_POST,REQUEST_URI,
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

    Display_printf(display, 0, 0, "HTTP Response Status Code: %d\n", ret);

    len = 0;
    do
    {
        ret = HTTPClient_readResponseBody(httpClientHandle, data, sizeof(data),
                                          &moreDataFlag);
        if(ret < 0)
        {
            printError("httpTask: response body processing failed", ret);
        }
        Display_printf(display, 0, 0, "%.*s \r\n",ret,data);
        len += ret;
    }
    while(moreDataFlag);

    Display_printf(display, 0, 0, "Received %d bytes of payload\n", len);

    ret = HTTPClient_disconnect(httpClientHandle);
    if(ret < 0)
    {
        printError("httpTask: disconnect failed", ret);
    }

    HTTPClient_destroy(httpClientHandle);
    return(0);
}

void httpTaskNonOS(void)
{

    bool moreDataFlag = false;
    char data[HTTP_MIN_RECV];
    int16_t ret = 0;
    int16_t len = 0;

    Display_printf(display, 0, 0, "Sending a HTTP POST request to '%s'\n",
                   HOSTNAME);

    HTTPClient_Handle httpClientHandle;
    int16_t statusCode;
    httpClientHandle = HTTPClient_create(&statusCode,0);
    if(statusCode < 0)
    {
        printError("httpTask: creation of https client handle failed",
                   statusCode);
    }

    ret =
        HTTPClient_setHeader(httpClientHandle,
                             HTTPClient_HFIELD_REQ_USER_AGENT,
                             USER_AGENT,strlen(USER_AGENT)+1,
                             HTTPClient_HFIELD_PERSISTENT);

    //ret = HTTPClient_setHeader(httpClientHandle, HTTPClient_HFIELD_REQ_CONTENT_TYPE,con_type,sizeof(con_type),HTTPClient_HFIELD_PERSISTENT);

    if(ret < 0)
    {
        printError("httpTask: setting request header failed", ret);
    }

    HTTPClient_extSecParams httpClientSecParams;
    httpClientSecParams.rootCa = SL_SSL_CA_CERT;
    httpClientSecParams.clientCert = NULL;
    httpClientSecParams.privateKey = NULL;


    ret = HTTPClient_connect(httpClientHandle,HOSTNAME,&httpClientSecParams,0);
    if(ret < 0)
    {
        printError("httpTask: connect failed", ret);
    }
    ret =
        HTTPClient_sendRequest(httpClientHandle,HTTP_METHOD_POST,REQUEST_URI,
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

    Display_printf(display, 0, 0, "HTTP Response Status Code: %d\n", ret);

    len = 0;
    do
    {
        ret = HTTPClient_readResponseBody(httpClientHandle, data, sizeof(data),
                                          &moreDataFlag);
        if(ret < 0)
        {
            printError("httpTask: response body processing failed", ret);
        }
        Display_printf(display, 0, 0, "%.*s \r\n",ret,data);
        len += ret;
    }
    while(moreDataFlag);

    Display_printf(display, 0, 0, "Received %d bytes of payload\n", len);

    ret = HTTPClient_disconnect(httpClientHandle);
    if(ret < 0)
    {
        printError("httpTask: disconnect failed", ret);
    }

    HTTPClient_destroy(httpClientHandle);
    return(0);
}

