#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sqlite3.h>

/* portul folosit */
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData
{
  int idThread; //id-ul thread-ului tinut in evidenta de acest program
  int cl;       //descriptorul intors de accept
} thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);
void crearejoc(void *);
void alaturarejoc(void *);
int logare = 0;
static int callback(void *data, int argc, char **argv, char **azColName)
{
  int i;
  //fprintf(stderr, "%s: ", (const char*)data);

  for (i = 0; i < argc; i++)
  {
    //printf("%s\n", argv[i] ? argv[i] : "NULL");
    if (argv[i] ? argv[i] : "NULL")
    {
      logare = 1;
    }
  }
  printf("%d\n", logare);
  return 0;
}

void login(void *arg)
{
  char username[1024] = "";
  struct thData tdL;
  tdL = *((struct thData *)arg);
  int ok, rc, log;
  sqlite3 *db;
  char *zErrMsg = 0;
  //char *sql;
  char sql[1024] = "";
  char rs[1024] = "";
  rc = sqlite3_open("baza2.db", &db);
  if (rc)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return;
  }
  else
  {
    fprintf(stderr, "Opened database successfully\n");
  }
  printf("hey\n");
  if (read(tdL.cl, username, sizeof(username)) <= 0)
  {
    printf("[Thread %d]\n", tdL.idThread);
    perror("Eroare la read() de la client.\n");
  }
  printf("hey1\n");
  snprintf(sql, 1024, "SELECT user FROM USERI WHERE user='%s'", username);
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  printf("hey2\n");
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  if (logare == 1)
  {
    strcpy(rs, "");
    strcat(rs, "Te-ai logat cu succes");
    if (write(tdL.cl, rs, sizeof(rs)) <= 0)
    {
      printf("[Thread %d] ", tdL.idThread);
      perror("[Thread]Eroare la write() catre client.\n");
    }
    log = logare;
    if (write(tdL.cl, &log, sizeof(log)) <= 0)
    {
      printf("[Thread %d] ", tdL.idThread);
      perror("[Thread]Eroare la write() catre client.\n");
    }
    printf("hehehuhsihs\n");
    return;
  }
  else
  {

    //printf("hehi\n");
    strcpy(sql, "");
    snprintf(sql, 1024, "INSERT INTO USERI (USER)"
                        "VALUES ('%s')",
             username);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
      fprintf(stdout, "Records created successfully\n");
      strcpy(rs, "");
      strcat(rs, "Am creat cont si te-am logat cu succes\n");
      if (write(tdL.cl, rs, strlen(rs)) <= 0)
      {
        printf("[Thread %d] ", tdL.idThread);
        perror("[Thread]Eroare la write() catre client.\n");
      }
      return;
    }
  }
  return;
}

static void *treat(void *arg)
{
  struct thData tdL;
  tdL = *((struct thData *)arg);
  printf("[thread %d]-- Asteptam mesajul...\n", tdL.idThread);
  fflush(stdout);
  pthread_detach(pthread_self());
  raspunde((struct thData *)arg);
  /* am terminat cu acest client, inchidem conexiunea */
  close((intptr_t)arg);
  return (NULL);
};

void raspunde(void *arg)
{
  int nr, i = 0;
  struct thData tdL;
  char rs[1024];
  tdL = *((struct thData *)arg);
  //while (1)
  //{
  if (read(tdL.cl, &nr, sizeof(int)) <= 0)
  {
    printf("[Thread %d]\n", tdL.idThread);
    perror("Eroare la read() de la client.\n");
  }
  printf("[Thread %d]Mesajul a fost receptionat...%d\n", tdL.idThread, nr);
  if (nr == 1)
  {
    login((struct thData *)arg);
  }

  if (nr == 2 && logare == 1)
  {
    crearejoc((struct thData *)arg);
    bzero(rs, 1024);
    strcat(rs, "Jocul va incepe curand ");
    /* returnam mesajul clientului */
    if (write(tdL.cl, rs, strlen(rs)) <= 0)
    {
      printf("[Thread %d] ", tdL.idThread);
      perror("[Thread]Eroare la write() catre client.\n");
    }
    else
      printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", tdL.idThread);
  }

  if (nr == 3 && logare == 1)
  {
    alaturarejoc((struct thData *)arg);
    bzero(rs, 1024);
    strcat(rs, "Jocul va incepe curand ");
    /* returnam mesajul clientului */
    if (write(tdL.cl, rs, strlen(rs)) <= 0)
    {
      printf("[Thread %d] ", tdL.idThread);
      perror("[Thread]Eroare la write() catre client.\n");
    }
    else
      printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", tdL.idThread);
  }
  //}
}

void crearejoc(void *arg)
{
  char rs[1024];
  int cod;
  struct thData tdL;
  tdL = *((struct thData *)arg);
  bzero(rs, 1024);
  strcat(rs, "Jocul s-a creat cu codul: ");
  cod = rand() % 50;
  char buf[1024];
  //itoa(cod,buf,10);
  snprintf(buf, 10, "%d\n", cod);
  strcat(rs, buf);
  /* returnam mesajul clientului */
  if (write(tdL.cl, rs, strlen(rs)) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    perror("[Thread]Eroare la write() catre client.\n");
  }
  //if (write(tdL.cl, &cod, sizeof(int)) <= 0)
  //{
  //  printf("[Thread %d] ", tdL.idThread);
  //  perror("[Thread]Eroare la write() catre client.\n");
  //}
}

void alaturarejoc(void *arg)
{
  char rs[1024];
  int cod;
  struct thData tdL;
  tdL = *((struct thData *)arg);
  bzero(rs, 1024);
  strcat(rs, "Introduceti codul pentru alaturare:");
  /* returnam mesajul clientului */
  if (write(tdL.cl, rs, strlen(rs)) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    perror("[Thread]Eroare la write() catre client.\n");
  }
  if (write(tdL.cl, &cod, sizeof(int)) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    perror("[Thread]Eroare la write() catre client.\n");
  }
}

int main()
{
  struct sockaddr_in server; // structura folosita de server
  struct sockaddr_in from;
  int nr; //mesajul primit de trimis la client
  int sd; //descriptorul de socket
  int pid;
  pthread_t th[100]; //Identificatorii thread-urilor care se vor crea
  int i = 0;

  /* crearea unui socket */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("[server]Eroare la socket().\n");
    return errno;
  }
  /* utilizarea optiunii SO_REUSEADDR */
  int on = 1;
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  /* pregatirea structurilor de date */
  bzero(&server, sizeof(server));
  bzero(&from, sizeof(from));

  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
  server.sin_family = AF_INET;
  /* acceptam orice adresa */
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  /* utilizam un port utilizator */
  server.sin_port = htons(PORT);

  /* atasam socketul */
  if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[server]Eroare la bind().\n");
    return errno;
  }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen(sd, 5) == -1)
  {
    perror("[server]Eroare la listen().\n");
    return errno;
  }
  /* servim in mod concurent clientii...folosind thread-uri */
  while (1)
  {
    int client;
    thData *td; //parametru functia executata de thread
    int length = sizeof(from);

    printf("[server]Asteptam la portul %d...\n", PORT);
    fflush(stdout);

    // client= malloc(sizeof(int));
    /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
    if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
    {
      perror("[server]Eroare la accept().\n");
      continue;
    }

    /* s-a realizat conexiunea, se astepta mesajul */

    // int idThread; //id-ul threadului
    // int cl; //descriptorul intors de accept

    td = (struct thData *)malloc(sizeof(struct thData));
    td->idThread = i++;
    td->cl = client;

    pthread_create(&th[i], NULL, &treat, td);

  } //while
};
