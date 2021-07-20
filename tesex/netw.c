#include "netw.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32

#define neterror WSAGetLastError()

#include <WinInet.h> // InternetOpen and friends, link to wininet
#include <Windows.h>

#else // _WIN32

#define neterror errno

#include <curl/curl.h> // all things curl, link to curl
#include <unistd.h> // close()
#include <arpa/inet.h> // inet_addr
#include <string.h>

#endif // UNIX

#include <stdlib.h>

/*
the last byte will be a line break, on WIN and UNIX you'll see
that the last character is replaced by a null terminator.
*/
#define URL_GET_IPV4 "http://icanhazip.com"

static char g_netw_inited = 0;

#ifdef _WIN32
int netw_public_ip_v4(char *buf, int *len)
{
    HINTERNET h_internet, h_file;
    DWORD ret = 0;

    if ((h_internet = InternetOpenA(
        NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) == NULL)
    {
        return GetLastError();
    }
    if ((h_file = InternetOpenUrlA(
        h_internet, URL_GET_IPV4, NULL, 0, INTERNET_FLAG_RELOAD, 0)) == NULL)
    {
        ret = GetLastError();
        InternetCloseHandle(h_internet);
        return ret;
    }

    WINBOOL read_ret = InternetReadFile(h_file, buf, *len, &ret);
    *len = ret;
    ret = read_ret ? 0 : GetLastError();

    if (*len > 0)
    {
        buf[*len - 1] = '\0';
    }        

    InternetCloseHandle(h_file);
    InternetCloseHandle(h_internet);
    return ret;
}
#else
typedef struct
{
    char *mem;
    int len;
    int len_new;
} Buffer;

static size_t write_data(void *data, size_t size, size_t nmemb, void *ptr)
{
    Buffer *buf = (Buffer *)ptr;
    size_t memgain = size * nmemb;
    if (memgain > buf->len - buf->len_new)
    {
        memgain = buf->len - buf->len_new;
    }

    memcpy(buf->mem + buf->len_new, data, memgain);

    buf->len_new += memgain;

    return memgain;
}

int netw_public_ip_v4(char *buf, int *len)
{
    int ret = 0;
    CURL *handle = curl_easy_init();

    if (ret = curl_easy_setopt(handle, CURLOPT_URL, URL_GET_IPV4))
    {
        curl_easy_cleanup(handle);
        curl_global_cleanup();
        return ret;
    }

    Buffer buffer = {
        .mem = buf,
        .len = *len,
        .len_new = 0
    };

    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&buffer);
    // some servers need a user agent
    //curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    ret = curl_easy_perform(handle);

    *len = buffer.len_new;
    if (*len > 0)
    {
        buf[*len - 1] = '\0';
    }

    curl_easy_cleanup(handle);
    return ret;
}
#endif

int netw_init()
{
    if (g_netw_inited)
    {
        return 0;
    }
    g_netw_inited = 1;
#ifdef _WIN32   
    WSADATA wsa_data;
	return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
    return curl_global_init(CURL_GLOBAL_ALL);
#endif
}

int netw_terminate()
{
    g_netw_inited = 0;
#ifdef _WIN32
    return WSACleanup();
#else
    curl_global_cleanup();
    return 0;
#endif
}

int netw_socket_close(Socket sock)
{
#ifdef _WIN32
    return !closesocket(sock) ? 0 : neterror;
#else
    return !close(sock) ? 0 : neterror;
#endif
}

int netw_client_create(const char *ip, unsigned short port, Protocol proto, Socket *client)
{
    *client = socket(AF_INET, (int)proto, (int)proto * 11 - 5);
    if (*client == INVALID_SOCKET)
    {
        return neterror;
    }

    struct sockaddr_in serv_addr;

#ifdef _WIN32
    serv_addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
    serv_addr.sin_addr.s_addr = inet_addr(ip);
#endif
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memset(serv_addr.sin_zero, 0, sizeof serv_addr.sin_zero);

    return !connect(*client, (struct sockaddr *)&serv_addr, sizeof serv_addr) ?
        0 : neterror;
}

int netw_server_create(Socket *serv, Protocol proto)
{
    // expression in a third argument just maps
    // SOCK_DGRAM(2) => UDP(17) and SOCK_STREAM(1) => TCP(6)
    *serv = socket(AF_INET, (int)proto, (int)proto * 11 - 5);
    return *serv != INVALID_SOCKET ? 0 : neterror;
}

int netw_server_bind(Socket *serv, unsigned short port)
{
    struct sockaddr_in serv_addr;

#ifdef _WIN32
    serv_addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    serv_addr.sin_addr.s_addr = INADDR_ANY;
#endif
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memset(serv_addr.sin_zero, 0, sizeof serv_addr.sin_zero);

    return !bind(*serv, (struct sockaddr *)&serv_addr, sizeof serv_addr) ?
        0 : neterror;
}

int netw_server_listen(Socket *serv, int queue)
{
    return !listen(*serv, queue) ? 0 : neterror;
}

int netw_server_accept(Socket *serv, Socket *out, struct sockaddr_in *client)
{
    socklen_t len = sizeof(struct sockaddr_in);
    *out = accept(*serv, (struct sockaddr *)client, &len);
    return *out != INVALID_SOCKET ? 0 : neterror;
}

#ifdef __cplusplus
}
#endif