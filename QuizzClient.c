#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;
int main(int argc, char *argv[])
{
    int sd;                    // descriptorul de socket
    struct sockaddr_in server; // structura folosita pentru conectare
                               // mesajul trimis
    int nr = 0;
    char buf[10];

    /* exista toate argumentele in linia de comanda? */
    if (argc != 3)
    {
        printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi(argv[2]);

    /* cream socketul */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons(port);

    /* ne conectam la server */
    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Eroare la connect().\n");
        return errno;
    }

    /* citirea mesajului */
    printf("Meniu: \n");
    printf("1. Logare\n");
    printf("2. Creare joc\n");
    printf("3. Alaturare joc\n");
    fflush(stdout);
    //int logare = 0;
    read(0, buf, sizeof(buf));
    nr = atoi(buf);
    int cod;
    char rs[1024];
    int log = 0;
    //rs2[1024];
    bzero(rs, 1024);
    /* trimiterea mesajului la server */
    if (write(sd, &nr, sizeof(int)) <= 0)
    {
        perror("[client]Eroare la write() spre server.\n");
        return errno;
    }
    if (nr == 1)
    {
        printf("Scrie username: \n");
        strcpy(rs, "");
        scanf("%s", rs);
        if (write(sd, rs, strlen(rs)) <= 0)
        {
            perror("[client]Eroare la write() spre server.\n");
            return errno;
        }
        strcpy(rs, "");
        if (read(sd, rs, sizeof(rs)) < 0)
        {
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }
        printf("%s\n", rs);
        if (read(sd, &log, sizeof(int)) < 0)
        {
            perror("[client]Eroare la read() de la server.\n");
            printf("ce se intampla\n");
            return errno;
        }
        printf("%d\n", log);
        printf("hola\n");
    }
    if (nr == 2)
    {
        strcpy(rs, "");
        if (read(sd, rs, sizeof(rs)) < 0)
        {
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }
        printf("%s\n", rs);
    }
    if (nr == 3)
    {
        strcpy(rs, "");
        if (read(sd, rs, sizeof(rs)) < 0)
        {
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }
        printf("%s\n", rs);
    }

    /* afisam mesajul primit */
    /* inchidem conexiunea, am terminat */
    close(sd);
}
