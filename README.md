# Book-Gateway
Progetto di coppia per il corso di Basi di Dati della laurea triennale in Informatica (Università di Padova). 
Il progetto mira a progettare ed implementare una base dati per una libreria come viene descritto più dettagliatamente nella [relazione](https://github.com/MatteoSquarz/Book-Gateway/blob/main/Relazione_Squarzoni_Lapenna.pdf)

## Installazione
È necessario avere installati localmente il database Postgres e PgAdmin4. Dunque sarà sufficiente avviare Postgres, e poi collegarsi al server tramite PgAdmin4 dove è necessario, tramite l'interfaccia grafica, creare il database ed eseguire lo [script](https://github.com/MatteoSquarz/Book-Gateway/blob/main/init_tabelle.sql) per inizializzare e popolare le tabelle. Le variabili della stringa di connessione al database sono modificabili nel codice.

Prestare attenzione alla cartella ```dependecies/``` la quale deve contenere le corrette dipendenze che possono variare in base al sistema operativo per importare le librerie. Nel codice caricato è possibile trovare le classiche dipendenze per un sistema Linux. 

In seguito è possibile compilare ed eseguire il codice con i comandi:
```
gcc queryDB.c -o queryDB -I /usr/include/postgresql -lpq
./queryDB
```

## Utilizzo
Una volta che si accede al programma da terminale è possibile effettuare una scelta digitando un numero da 1 a 5 per selezionare la query che si vuole eseguire, successivamente inserire i parametri desiderati. I risultati vengono poi mostrati a schermo.

<img width="631" height="225" alt="image" src="https://github.com/user-attachments/assets/50bd6f67-47d4-4542-bada-61a934db4ce8" />

