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
			logger.error("Error: "+e.getMessage());
			System.err.println("Es konnte keine Verbindung zu Datenbank hergestellt werden!\nProgramm wird beendet...");
			System.exit(-1);
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
			logger.error("Error: "+se.getMessage());
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
		int cnt = 0;
		ResultSetMetaData meta = rs.getMetaData();
		for (int i = 1; i <= meta.getColumnCount(); i++){
			writer.format("%20s",meta.getColumnLabel(i)+"\t");
		}
		writer.println();
		writer.println("--------------------------------------------------------------------------------");
		
		while (rs.next()) {
			for (int i = 1; i <= meta.getColumnCount(); i++){
				if(meta.getColumnType(i) == 12 || meta.getColumnType(i) == 1) writer.format("%20s","'"+rs.getString(i)+"'\t");
				else writer.format("%20s",rs.getString(i)+"\t");
			}
			writer.println();
			writer.flush();
			cnt++;
		}
		writer.println();
		writer.println("\t"+cnt+" record(s) selected.");
		writer.println();
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
			PreparedStatement stmt = co
					.prepareStatement("SELECT ASIN,  " +
													 "ARTIST, TITLE,  LABEL, " +
													 "DEC(ROUND(LOWESTNEWPRICE,2),8,2) AS LOWESTNEWPRICE, " +
													 "DEC(ROUND(LOWESTUSEDPRICE,2),8,2) AS LOWESTUSEDPRICE," +
													 "NUMBEROFDISCS FROM CDS ORDER BY ARTIST, TITLE");
			ResultSet rs = stmt.executeQuery();
			cnt = printResult(rs, writer);
		} catch(SQLException se) {
			logger.error("Error: "+se.getMessage());
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
		int discNumber = 0;
		try{
			PreparedStatement stmt = co.prepareStatement("SELECT * FROM CDs WHERE ASIN=?");
			stmt.setString(1, asinCode);
			ResultSet rs = stmt.executeQuery();
			cnt = printResult(rs, writer);
			stmt = co.prepareStatement("SELECT NumberOfDiscs FROM CDs WHERE ASIN=?");
			stmt.setString(1, asinCode);
			rs = stmt.executeQuery();
			if(rs.next()) discNumber = rs.getInt(1);
			int i = 1;
			while (i <= discNumber) {
				PreparedStatement stmt2 = co.prepareStatement("SELECT TRACKNUMBER, TITLE FROM Tracks WHERE ASIN=? AND DiscNumber=? ORDER BY TRACKNUMBER");
				stmt2.setString(1, asinCode);
				stmt2.setInt(2, i);
				ResultSet rs2 = stmt2.executeQuery();
				writer.println("DiscNum: " + i);
				printResult(rs2, writer);
				i++;
			}
		} catch(SQLException se) {
			logger.error("Error: "+se.getMessage());
		}
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
		try{
			PreparedStatement stmt = 
				co.prepareStatement("SELECT DEC(ROUND(AVG(LowestNewPrice),2),8,2) AVG_New," +
													 "DEC(ROUND(AVG(LowestUsedPrice),2),8,2) 	AVG_Used, " +
													 "DEC(ROUND(MIN(LowestNewPrice),2),8,2)		MIN_New, " +
													 "DEC(ROUND(MIN(LowestUsedPrice),2),8,2) 	MIN_Used, " +
													 "DEC(ROUND(MAX(LowestNewPrice),2),8,2) 	MAX_New, " +
													 "DEC(ROUND(MAX(LowestUsedPrice),2),8,2) 	MAX_Used FROM CDs");
			ResultSet rs = stmt.executeQuery();
			cnt = printResult(rs, writer);
		} catch(SQLException se) {
			logger.error("Error: "+se.getMessage());
		}
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
		try{
			PreparedStatement stmt = 
				co.prepareStatement("SELECT DEC(ROUND(AVG(LowestNewPrice),2),8,2) AVG_New," +
													 "DEC(ROUND(AVG(LowestUsedPrice),2),8,2) 	AVG_Used, " +
													 "DEC(ROUND(MIN(LowestNewPrice),2),8,2)		MIN_New, " +
													 "DEC(ROUND(MIN(LowestUsedPrice),2),8,2) 	MIN_Used, " +
													 "DEC(ROUND(MAX(LowestNewPrice),2),8,2) 	MAX_New, " +
													 "DEC(ROUND(MAX(LowestUsedPrice),2),8,2) 	MAX_Used FROM CDs WHERE Artist=?");
			stmt.setString(1, artist);
			ResultSet rs = stmt.executeQuery();
			cnt = printResult(rs, writer);
		} catch(SQLException se) {
			logger.error("Error: "+se.getMessage());
		}
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
		try{
			System.out.println(price);
			PreparedStatement stmt = co.prepareStatement("UPDATE CDs SET LowestNewPrice=? WHERE ASIN=?");
			stmt.setFloat(1, price);
			stmt.setString(2, asin);
			stmt.execute();
		} catch(SQLException se) {
			logger.error("Error: "+se.getMessage());
		}
	}

	/**
	 * Loescht eine CD und ihre Tracks aus der DB
	 * @param asin String
	 * 
	 * 3 Punkte
	 */
	public void deleteSingleCD(String asin)	{
		try{
			PreparedStatement stmt1 = co.prepareStatement("DELETE FROM Tracks WHERE ASIN=?");
			stmt1.setString(1, asin);
			stmt1.execute();
			PreparedStatement stmt2 = co.prepareStatement("DELETE FROM CDs WHERE ASIN=?");
			stmt2.setString(1, asin);
			stmt2.execute();
		} catch(SQLException se) {
			logger.error("Error: "+se.getMessage());
		}
	}
}
