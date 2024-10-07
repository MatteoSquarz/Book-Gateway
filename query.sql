/*
-- QUERY 1: Numero libri di autori [nazionalità] per ogni piano della libreria

SELECT sezione.numeropiano, COUNT(DISTINCT scrittura.libro) AS numLibri
FROM prodotto, scrittura, autore, sezione
WHERE prodotto.ean13 = scrittura.libro AND scrittura.autore = autore.cf 
	AND autore.nazionalita = 'British' AND prodotto.sezione = sezione.nome
GROUP BY sezione.numeropiano
ORDER BY sezione.numeropiano;
*/


/*
-- QUERY 2: Autori con almeno [tot] recensioni di punteggio massimo (punteggio = 5)

SELECT nome, cognome FROM autore 
WHERE cf IN
	(SELECT autore.cf
	 FROM recensione, scrittura, autore
	 WHERE recensione.libro = scrittura.libro 
	  AND scrittura.autore = autore.cf
	  AND recensione.punteggio = 5
	 GROUP BY (autore.cf)
	 HAVING COUNT(*) >= 4);
*/
/*
-- Eventuale indice utilizzabile per query 2
DROP INDEX IF EXISTS idx_PunteggioLibro;
CREATE INDEX idx_PunteggioLibro ON recensione (punteggio, libro); 
*/



/*
-- QUERY 3: I 5 clienti che hanno acquistato il maggior numero di libri dalla sezione [nome]

SELECT cliente.cf, cliente.nome, cliente.cognome, SUM(scontrino.quantita) AS numLibri
FROM cliente, scontrino, prodotto
WHERE cliente.cf = scontrino.cliente
  AND scontrino.prodotto = prodotto.ean13
  AND prodotto.sezione = 'Horror'
GROUP BY cliente.cf
ORDER BY numLibri DESC
LIMIT 5;
*/


/*
-- QUERY 4: Casa editrice con sede a [città] che ha venduto il maggior numero di libri

DROP VIEW IF EXISTS numLibriVendutiCaseEditriciPerCitta;
CREATE VIEW numLibriVendutiCaseEditriciPerCitta AS
	SELECT casaeditrice.nome, casaeditrice.citta, SUM(scontrino.quantita) AS numLibri 
	FROM scontrino, libro, casaeditrice
	WHERE scontrino.prodotto = libro.ean13 
      AND libro.casaeditrice = casaeditrice.piva
	GROUP BY casaeditrice.nome, casaeditrice.citta;

SELECT * FROM numLibriVendutiCaseEditriciPerCitta 
WHERE numLibri = (SELECT MAX(numLibri)
				  FROM numLibriVendutiCaseEditriciPerCitta
				  WHERE numLibriVendutiCaseEditriciPerCitta.citta = 'Milano')
  AND numLibriVendutiCaseEditriciPerCitta.citta = 'Milano';
*/


/*
-- QUERY 5: Titolo del libro che ha incassato di più per la casa editrice [nome]

DROP VIEW IF EXISTS IncassiPerLibro_CasaEditrice;
CREATE VIEW IncassiPerLibro_CasaEditrice AS
	SELECT libro.ean13, casaeditrice.nome AS casaeditrice, SUM(scontrino.quantita*prodotto.prezzo) AS incasso 
	FROM prodotto, libro, scontrino, casaeditrice
	WHERE libro.ean13 = scontrino.prodotto 
	  AND prodotto.ean13 = libro.ean13
	  AND libro.casaeditrice = casaeditrice.piva
	GROUP BY libro.ean13, casaeditrice.nome;


SELECT prodotto.titolo, IncassiPerLibro_CasaEditrice.incasso 
FROM prodotto, IncassiPerLibro_CasaEditrice 
WHERE prodotto.ean13 = IncassiPerLibro_CasaEditrice.ean13
  AND IncassiPerLibro_CasaEditrice.casaeditrice = 'Mondadori'
  AND IncassiPerLibro_CasaEditrice.incasso = (SELECT MAX(incasso)
											  FROM IncassiPerLibro_CasaEditrice
											  WHERE IncassiPerLibro_CasaEditrice.casaeditrice = 'Mondadori');
*/
