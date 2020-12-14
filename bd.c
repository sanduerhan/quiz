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

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for (i = 0; i < argc; i++)
  {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void CreareBaza()
{
  sqlite3 *db;
  char *sql, *sql2, *sql3;
  int rc, rc2, rc3;
  char *zErrMsg = 0;
  char *zErrMsg2 = 0;
  char *zErrMsg3 = 0;
  rc = sqlite3_open("baza2.db", &db);
  if (rc)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return;
  }
  else
  {
    fprintf(stdout, "Opened database successfully\n");
  }
  sql = "CREATE TABLE QUESTIONS("
        "ID INT PRIMARY KEY NOT NULL, "
        "QUESTION  TEXT NOT NULL);";

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else
  {
    fprintf(stdout, "Table created successfully\n");
  }

  sql2 = "CREATE TABLE OPTIONS("
         "IDOPT INT PRIMARY KEY NOT NULL, "
         "IDQ INT  NOT NULL, "
         "OPTION  TEXT NOT NULL, "
         "ANSWER BOOLEAN NOT NULL, "
         "FOREIGN KEY(IDQ) REFERENCES QUESTIONS(ID));";

  rc2 = sqlite3_exec(db, sql2, callback, 0, &zErrMsg2);
  if (rc2 != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", zErrMsg2);
    sqlite3_free(zErrMsg2);
  }
  else
  {
    fprintf(stdout, "Table created successfully\n");
  }
  sql3 = "CREATE TABLE USERI("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
        "USER TEXT NOT NULL);";

  rc3 = sqlite3_exec(db, sql3, callback, 0, &zErrMsg3);
  if (rc3 != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", zErrMsg3);
    sqlite3_free(zErrMsg3);
  }
  else
  {
    fprintf(stdout, "Table created successfully\n");
  }
  sqlite3_close(db);
}

void InsertBaza()
{
  sqlite3 *db;
  char *zErrMsg = 0;
  char *zErrMsg2 = 0;
  int rc, rc2;
  char *sql, *sql2;

  /* Open database */
  rc = sqlite3_open("baza.db", &db);

  if (rc)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return;
  }
  else
  {
    fprintf(stderr, "Opened database successfully\n");
  }

  /* Create SQL statement */
  sql = "INSERT INTO QUESTIONS (ID,QUESTION) "
        "VALUES (1, 'Care este capitala Romaniei?' ); "
        "INSERT INTO QUESTIONS (ID,QUESTION) "
        "VALUES (2, 'Care este capitala Rusiei?' ); "
        "INSERT INTO QUESTIONS (ID,QUESTION) "
        "VALUES (3, 'Care este capitala Belgiei?' ); "
        "INSERT INTO QUESTIONS (ID,QUESTION) "
        "VALUES (4, 'Care este capitala Finlandei?' ); ";

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if (rc != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else
  {
    fprintf(stdout, "Records created successfully\n");
  }

  sql2 = "INSERT INTO OPTIONS (IDOPT,IDQ,OPTION,ANSWER) "
         "VALUES (1, 1,'Iasi',0 ); "
         "INSERT INTO OPTIONS (IDOPT,IDQ,OPTION,ANSWER) "
         "VALUES (2, 1,'Bucuresti',1 ); ";

  /* Execute SQL statement */
  rc2 = sqlite3_exec(db, sql2, callback, 0, &zErrMsg2);

  if (rc2 != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", zErrMsg2);
    sqlite3_free(zErrMsg2);
  }
  else
  {
    fprintf(stdout, "Records created successfully\n");
  }

  sql2 = "INSERT INTO USERI (ID, USER) "
         "VALUES (1, 'sandu' ); ";
  rc2 = sqlite3_exec(db, sql2, callback, 0, &zErrMsg2);

  if (rc2 != SQLITE_OK)
  {
    fprintf(stderr, "SQL error: %s\n", zErrMsg2);
    sqlite3_free(zErrMsg2);
  }
  else
  {
    fprintf(stdout, "Records created successfully\n");
  }
  sqlite3_close(db);
}

int main()
{
  CreareBaza();
  InsertBaza();
  return 0;
}
