package musicDB;
 
import java.io.PrintWriter;
import java.sql.*;	// JDBC classes

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class MusicDB {
	//	A connection (session) with a specific database
	private Connection co 	  = null;
	private static Log logger = LogFactory.getLog(MusicDB.class);

	/**
	 * Konstructor fuer MusicDB
	 * Aufbau der Verbindung zur Datenbank
	 * 
	 * @param dbName Datenbankname
	 */
	public MusicDB(String dbName){
		createDBConnection(dbName);
	}

	/**
	 * Konstructor fuer MusicDB
	 * Schliessen der Verbindung zur Datenbank
	 */
	public void finalize(){
		closeDBConnection();
	}

	/**
	 * Verbindung zum Datenbank-Server aufnehmen
	 * @param dbName datenbank name
	 * 
	 * 4 Punkte
	 */
	private void createDBConnection(String dbName) {
		try {
			Class.forName("COM.ibm.db2.jdbc.app.DB2Driver").newInstance();
			co = DriverManager.getConnection("jdbc:db2:"+dbName);
		} catch(Exception e) {
			System.out.println("Error:" + e);
		}
	}
	
	/**
	 * Verbindung zum Datenbank-Server schliessen.
	 * 
	 * 2 Punkte
	 */ 
	private void closeDBConnection() {
		try {
			if (co != null) co.close();
		} catch(SQLException se) {
			System.out.println("Error:" + se);
		}
	}
	
	/**
	 * Zeigt die Informationen des ResultSets an: Spaltennamen und Werte
	 *  - Zeichenketten sollen in Hochkommata (') stehen
	 *  - die einzelnen Spalten sollen durch Tabs separiert werden (\t)
	 * @param writer PrinterWriter, auf den das Ergebnis geschrieben werden soll
	 * @param rs ResultSet
	 * @return int Anzahl der selektierten Tupel des Ergebnisses
	 * @throws SQLException
	 * 
 	 * 4 Punkte
	 */
	private int printResult(ResultSet rs, PrintWriter writer) throws SQLException {
		// TODO
		int cnt = 0;
		writer.println("      ASIN");
		writer.println("----------");
		while (rs.next()) {
			writer.println(rs.getString(1));
			cnt++;
		}
		writer.println();
		writer.println(cnt+" record(s) selected.");
		return cnt;
	}


	/**
	 * Zeigt alle CDs ohne ihre Tracks an,
	 * d. h. die Informationen zu ASIN ARTIST TITLE LABEL
	 * LOWNEWPRICE LOWUSEDPRICE NUMOFDISC
	 * @param writer PrinterWriter, auf den das Ergebnis geschrieben werden soll
	 * @return int Anzahl der selektierten Tupel des Ergebnisses
	 * 
	 * 3 Punkte
	 */
	public int showAllCDs(PrintWriter writer) {
		int cnt = 0;
		try{
			PreparedStatement stmt = co.prepareStatement("SELECT * FROM CDs");
			ResultSet rs = stmt.executeQuery();
			cnt = printResult(rs, writer);
		} catch(SQLException se) {
			System.out.println("Error:" + se);
		}
		return cnt;
	}

	/**
	 * Zeigt eine CD anhand ihres ASIN mit allen Discs und allen Tracks an,
	 * d. h. zuerst die Informationen zu ASIN ARTIST TITLE LABEL
	 * LOWNEWPRICE LOWUSEDPRICE NUMOFDISC
	 * und dann DISCNUM:
	 * TRACKNUM TRACKTITLE
	 * @param asinCode zu selektierende CDs
	 * @param writer PrinterWriter, auf den das Ergebnis geschrieben werden soll
	 * @return int Anzahl der selektierten Tupel des Ergebnisses
	 * 
	 * 6 Punkte
	 */
	public int showSingleCD(String asinCode, PrintWriter writer) {
		int cnt = 0;
		/* BEGIN */
/* HIER muss Code eingefuegt werden */
		/* END */
		return cnt;
	}
	
	/**
	 * Ausgabe des Durchschnittspreises, MIN, MAX aller CD bzgl. LowUsedPrice
	 * und LowNewPrice
	 * @param writer PrinterWriter, auf den das Ergebnis geschrieben werden soll
	 * @return int Anzahl der selektierten Tupel des Ergebnisses
	 * 
	 * 3 Punkte
	 */
	public int showGroupAllPrices(PrintWriter writer) {
		int cnt = 0;
		/* BEGIN */
/* HIER muss Code eingefuegt werden */
		/* END */
		return cnt;
	}

	/**
	 * Ausgabe des Durchschnittspreises, MIN, MAX aller CDs eines Kuenstlers bzgl.
	 * LowUsedPrice und LowNewPrice mit Hilfe von printResult.
	 * @param artist Name des Kuenstlers
	 * @param writer PrinterWriter, auf den das Ergebnis geschrieben werden soll
	 * @return int Anzahl der selektierten Tupel des Ergebnisses
	 * 
	 * 3 Punkte
	 */
	public int showGroupPricesArtist(String artist, PrintWriter writer) {
		int cnt = 0;
		/* BEGIN */
/* HIER muss Code eingefuegt werden */
		/* END */
		return cnt;
	}

	/**
	 * Aendert den LowNewPrice einer CD
	 * @param asin String
	 * @param price float
	 * 
	 * 2 Punkte
	 */
	public void updateNewPrice(String asin, float price)	{
		/* BEGIN */
/* HIER muss Code eingefuegt werden */
		/* END */
	}

	/**
	 * Loescht eine CD und ihre Tracks aus der DB
	 * @param asin String
	 * 
	 * 3 Punkte
	 */
	public void deleteSingleCD(String asin)	{
		/* BEGIN */
/* HIER muss Code eingefuegt werden */
		/* END */
	}
}	
