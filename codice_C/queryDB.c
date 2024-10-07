#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "dependencies/include/libpq-fe.h"

#define PG_HOST    "127.0.0.1"
#define PG_USER    "matteo"
#define PG_DB      "progetto"
#define PG_PASS    "admin"
#define PG_PORT    5432

void checkCommand(PGresult *res, const PGconn *conn) {
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Comando fallito %s\n", PQerrorMessage(conn));
        PQclear(res);
        exit(1);
    }
}

void checkResults(PGresult *res, const PGconn *conn) {
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Risultati inconsistenti %s\n", PQerrorMessage(conn));
        PQclear(res);
        exit(1);
    }
}

static void menu(){
    printf("-----------------------------------------------------------------\n");
    printf("| Digitare un numero scegliendo tra una delle query disponibili |\n");
    printf("-----------------------------------------------------------------\n");
    printf("1) Numero libri di autori [nazionalità] per ogni piano della libreria\n");
    printf("2) Autori con almeno [tot] recensioni di punteggio massimo (punteggio = 5)\n");
    printf("3) I primi 5 clienti che hanno acquistato il maggior numero di libri dalla sezione [nome]\n");
    printf("4) Casa editrice con sede a [città] che ha venduto il maggior numero di libri\n");
    printf("5) Titolo del libro che ha incassato di più per la casa editrice [nome]\n");
}

void printResults(const PGresult* res){
        char headerPrint[500];
        strcpy(headerPrint, "|");
        int h;
        for (h = 0; h < PQnfields(res); h++){
                strcat(headerPrint, " ");
                strcat(headerPrint, PQfname(res, h));
                strcat(headerPrint, " |");
                if (h == PQnfields(res)-1){
                    strcat(headerPrint, "\n");
                }
        }
        printf("%s", headerPrint);

        int i;
        char resultPrint[500];
        strcpy(resultPrint, "|");
        for (i = 0; i < PQntuples(res); i++){
            int j;
            for (j = 0; j < PQnfields(res); j++){
                strcat(resultPrint, " ");
                strcat(resultPrint, PQgetvalue(res, i, j));
                strcat(resultPrint, " |");
                if (j == PQnfields(res)-1){
                    strcat(resultPrint, "\n");
                }
            }
            printf("%s", resultPrint);
            strcpy(resultPrint, "|");
        }
}
void makeQueryDQL(const char* queryName, const char* query, PGconn* conn, const int numParam, const int* paramFormats, const char** paramValues, bool view){ 
    PGresult* res = PQprepare(conn, queryName, query, numParam, NULL);
    res = PQexecPrepared(conn, queryName, numParam, paramValues, NULL, paramFormats, 0);
    if(view){     //per le query di tipo CREATE VIEW o DROP VIEW
        checkCommand(res, conn);
    }
    else{     //per le query di tipo SELECT...
        checkResults(res, conn);
        printResults(res);
    }
    PQclear(res);
}

int main(int argc, char **argv) {
    bool check = true;
    char conninfo [250];
    sprintf ( conninfo , " user=%s password=%s dbname=%s host=%s port=%d " , PG_USER , PG_PASS , PG_DB , PG_HOST , PG_PORT ) ;
    
    PGconn *conn;
    conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("Errore di connessione: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    char* query;
    char param[30];
    const char* paramValues[1] = {param};
    const int paramFormats[1] = {0};
    while(check == true){
        menu();
        int scelta;
        scanf("%d", &scelta);
        memset(param,0,sizeof(param));  //pulisce la stringa del parametro
        switch (scelta){
        case 1:
            query = "SELECT sezione.numeropiano, COUNT(DISTINCT scrittura.libro) AS numLibri FROM prodotto, scrittura, autore, sezione WHERE prodotto.ean13 = scrittura.libro AND scrittura.autore = autore.cf AND autore.nazionalita = $1 AND prodotto.sezione = sezione.nome GROUP BY sezione.numeropiano ORDER BY sezione.numeropiano;";            
            printf("Inserire nazionalità autore: (ad esempio: 'Italian', 'American', 'British')\n");
            scanf("%s", param);
            makeQueryDQL("query1", query, conn, 1, paramFormats, paramValues, false);
            break;
        case 2:
            query = "SELECT nome, cognome FROM autore WHERE cf IN (SELECT autore.cf FROM recensione, scrittura, autore WHERE recensione.libro = scrittura.libro AND scrittura.autore = autore.cf AND recensione.punteggio = 5 GROUP BY (autore.cf) HAVING COUNT(*) >= $1);";
            printf("Inserire numero: (per vedere risultati inserire numeri <= 8)\n");
            scanf("%s", param);
            makeQueryDQL("query2", query, conn, 1, paramFormats, paramValues, false);
            break;
        case 3:
            query = "SELECT cliente.cf, cliente.nome, cliente.cognome, SUM(scontrino.quantita) AS numLibri FROM cliente, scontrino, prodotto WHERE cliente.cf = scontrino.cliente AND scontrino.prodotto = prodotto.ean13 AND prodotto.sezione = $1 GROUP BY cliente.cf ORDER BY numLibri DESC LIMIT 5;";
            printf("Inserire nome sezione: (ad esempio: 'Horror', 'Dystopian', 'Thriller')\n");
            scanf("%s", param);
            makeQueryDQL("query3", query, conn, 1, paramFormats, paramValues, false);
            break;
        case 4:
            query = "DROP VIEW IF EXISTS numLibriVendutiCaseEditriciPerCitta;";
            makeQueryDQL("DropView", query, conn, 0, NULL, NULL, true);
            printf("Inserire città: (ad esempio: 'Milano', 'Torino', 'Roma')\n");
            scanf("%s", param);
            query = "CREATE VIEW numLibriVendutiCaseEditriciPerCitta AS SELECT casaeditrice.nome, casaeditrice.citta, SUM(scontrino.quantita) AS numLibri FROM scontrino, libro, casaeditrice WHERE scontrino.prodotto = libro.ean13 AND libro.casaeditrice = casaeditrice.piva GROUP BY casaeditrice.nome, casaeditrice.citta;";
            makeQueryDQL("CreateView", query, conn, 0, NULL, NULL, true);
            query = "SELECT * FROM numLibriVendutiCaseEditriciPerCitta WHERE numLibri = (SELECT MAX(numLibri) FROM numLibriVendutiCaseEditriciPerCitta WHERE numLibriVendutiCaseEditriciPerCitta.citta = $1) AND numLibriVendutiCaseEditriciPerCitta.citta = $1;";
            makeQueryDQL("query4", query, conn, 1, paramFormats, paramValues, false);
            break;
        case 5:
            query = "DROP VIEW IF EXISTS IncassiPerLibro_CasaEditrice;";
            makeQueryDQL("DropView", query, conn, 0, NULL, NULL, true);
            printf("Inserire nome casa editrice: (ad esempio: 'Mondadori', 'Giunti', 'Feltrinelli')\n");
            scanf("%s", param);
            query = "CREATE VIEW IncassiPerLibro_CasaEditrice AS SELECT libro.ean13, casaeditrice.nome AS casaeditrice, SUM(scontrino.quantita*prodotto.prezzo) AS incasso FROM prodotto, libro, scontrino, casaeditrice WHERE libro.ean13 = scontrino.prodotto AND prodotto.ean13 = libro.ean13 AND libro.casaeditrice = casaeditrice.piva GROUP BY libro.ean13, casaeditrice.nome;";
            makeQueryDQL("CreateView", query, conn, 0, NULL, NULL, true);
            query = "SELECT prodotto.titolo, IncassiPerLibro_CasaEditrice.incasso FROM prodotto, IncassiPerLibro_CasaEditrice WHERE prodotto.ean13 = IncassiPerLibro_CasaEditrice.ean13 AND IncassiPerLibro_CasaEditrice.casaeditrice = $1 AND IncassiPerLibro_CasaEditrice.incasso = (SELECT MAX(incasso) FROM IncassiPerLibro_CasaEditrice WHERE IncassiPerLibro_CasaEditrice.casaeditrice = $1);";
            makeQueryDQL("query5", query, conn, 1, paramFormats, paramValues, false);
            break;
        default:
            printf("Errore! Scegli una delle query digitando da a 1 a 5\n");
            break;
        }
        
        
        printf("Vuoi eseguire un'altra query? (y/n)\n");
        char risposta;
        scanf(" %c", &risposta);
        while(!(risposta == 'n' || risposta == 'y')){
            printf("Vuoi eseguire un'altra query? (y/n)\n");
            scanf(" %c", &risposta);
        }
        if(risposta == 'n')
            check = false;
        else if(risposta == 'y')
            check = true; 
    }
    
    PQfinish(conn);

}
