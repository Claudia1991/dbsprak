-- =================================================================
-- | WICHTIG:                                                      |
-- |   Dieses Skript muss von einer Shell auf den Vogelrechnern der| 
-- |   Studentenpools aus wie folgt aufrufbar sein:                |
-- |         db2 -tvf dropMusicObjects.sql	                   |
-- |    Der Dateiname ist nicht zu veraendern                      |
-- =================================================================

CONNECT TO DBPrak;

-- Aufblatt 5 ------------------------------------------------------

-- Aufgabe 1.3 -----------------------------------------------------
-- Loeschen aller in 1.1 und 1.2 erzeugten Objekte -----------------
--------------------------------------------------------------------
DROP TRIGGER updatePrice;
ALTER TABLE CDs DROP CONSTRAINT minNew;
ALTER TABLE CDs DROP CONSTRAINT minUsed;
ALTER TABLE Tracks DROP CONSTRAINT minDisc;
ALTER TABLE Tracks DROP CONSTRAINT minTrack;
ALTER TABLE Tracks DROP CONSTRAINT fkey;
ALTER TABLE Tracks DROP CONSTRAINT pkey;
ALTER TABLE CDs DROP CONSTRAINT pkey;
DROP TABLE Tracks;
DROP TABLE CDs;
CONNECT RESET;
