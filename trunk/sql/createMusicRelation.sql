-- =================================================================
-- | WICHTIG:                                                      |
-- |   Dieses Skript muss von einer Shell auf den Vogelrechnern der| 
-- |   Studentenpools aus wie folgt aufrufbar sein:                |
-- |         db2 -tvf createMusicRelations.sql                     |
-- |    Der Dateiname ist nicht zu veraendern                      |
-- =================================================================

CONNECT TO DBPrak;

-- Aufblatt 5 ------------------------------------------------------

-- Aufgabe 1.1 -----------------------------------------------------
-- Erzeugen der Tabellen zum Speichern der Musik Daten -------------
--------------------------------------------------------------------
CREATE TABLE CDs(
  ASIN              CHAR(10)        NOT NULL,
  Artist            VARCHAR(25),
  Title             VARCHAR(50),
  Label             VARCHAR(25),
  NumberOfDiscs     INT,
  LowestNewPrice    NUMERIC(5,2),
  LowestUsedPrice   NUMERIC(5,2)
);
CREATE TABLE Tracks(
  ASIN              CHAR(10)        NOT NULL,
  DiscNumber        INT             NOT NULL,
  TrackNumber       INT             NOT NULL,
  Title             VARCHAR(50)
);
-- Aufgabe 1.2 -----------------------------------------------------
-- Erstellen von Primaer-/Fremdschluesseln, Contraints, Triggern ---
--------------------------------------------------------------------
-- Primarykeys
ALTER TABLE CDs ADD CONSTRAINT pkey PRIMARY KEY(ASIN);
ALTER TABLE Tracks ADD CONSTRAINT pkey PRIMARY KEY(ASIN, DiscNumber, TrackNumber);
-- Foreignkeys
ALTER TABLE Tracks ADD CONSTRAINT fkey FOREIGN KEY(ASIN) REFERENCES CDs(ASIN);
-- Constraints
ALTER TABLE Tracks ADD CONSTRAINT minDisc CHECK (DiscNumber > 0);
ALTER TABLE Tracks ADD CONSTRAINT minTrack CHECK (TrackNumber > 0);
ALTER TABLE CDs ADD CONSTRAINT minNew CHECK (LowestNewPrice >= 0);
ALTER TABLE CDs ADD CONSTRAINT minUsed CHECK (LowestUsedPrice >= 0);
-- Trigger
CREATE TRIGGER updatePrice BEFORE UPDATE OF LowestNewPrice ON CDs REFERENCING NEW AS newtab OLD AS oldtab FOR EACH ROW SET newtab.LowestUsedPrice = (oldtab.LowestUsedPrice * (newtab.LowestNewPrice / oldtab.LowestNewPrice));
-- evtl. Views

CONNECT RESET;
