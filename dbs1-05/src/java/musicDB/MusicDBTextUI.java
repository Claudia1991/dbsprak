package musicDB;
 
import java.io.PrintStream;
import java.io.PrintWriter;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class MusicDBTextUI 
{
	//	A connection (session) with a specific database
	private MusicDB musicDB;
	private PrintWriter out;
	private static Log logger = LogFactory.getLog(MusicDBTextUI.class);
	
	public MusicDBTextUI(PrintStream stream){
		musicDB = new MusicDB(Config.getProperty("database","dbprak"));
		out = new PrintWriter(stream,true);
	}
	
	/**
	 * UI zum Einlesen des ASIN_CODE (Relation CD) als Tupelidentifikator durch den 
	 * Nutzer. Testen auf Einhaltung des Formats (zehnstellig):
	 * 
	 * @return ASIN String
	 */
	
	private String getASINCodeTextUI()
	{
		String asinCode = "";
		
		while(asinCode.length()!= 10)
		{
			out.println ("Which ASIN: ");
			asinCode = TextIO.getWord();
			
			if (asinCode.length()!= 10)
				out.println ("ASIN has wrong length(10).");
		}
		
		return asinCode ;
	}

	/**
	 * Zeigt alle CDs ohne ihre Tracks an.
	 * D.h. die Informationen zu ASIN ARTIST TITLE LABEL
	 * LOWNEWPRICE LOWUSEDPRICE NUMOFDISC
	 * 
	 * @return int row count
	 */

	public int showAllCDsTextUI()
	{
		out.println ("\n...SHOW ALL CDs...\n");
		return musicDB.showAllCDs(out);
	}

	/**
	 * Zeigt eine CDs anhand ihres ASIN Codes mit allen Discs und allen Tracks an.
	 * D.h. zuerst die Informationen zu ASIN ARTIST TITLE LABEL
	 * LOWNEWPRICE LOWUSEDPRICE NUMOFDISC
	 * dann DISCNUM:
	 * TRACKNUM TRACKTITLE
	 * 
	 * @return int row count
	 */

	public int showSingleCDTextUI()
	{
		out.println ("\n...SHOW SELECTED CD AND TRACKS...\n"); 
		return musicDB.showSingleCD(getASINCodeTextUI(),out);
	}
	
	/**
	 * Ausgabe des Durchschnittspreises,MIN,MAX aller CD bzgl LowUsedPrice
	 * und LowNewPrice
	 *
	 */

	public void showGroupAllPricesTextUI()
	{
		out.println ("\n...SHOW AVG,MIN,MAX PRICE...\n"); 
		musicDB.showGroupAllPrices(out);
	}

	/**
	 * Ausgabe des Durchschnittspreises,MIN,MAX aller CD bzgl LowUsedPrice
	 * und LowNewPrice
	 * 
	 */

	public int showGroupPricesArtistTextUI()
	{
		String artist;
		out.println ("\n...SHOW AVG,MIN,MAX PRICE FOR AN ARTIST...\n"); 
		out.println ("Which Artist: ");
		artist = TextIO.getLine();
		return musicDB.showGroupPricesArtist(artist,out);
	}
	
	/**
	 * UI zum Einfuegen von Tupeln aus einem XML File
	 * 
	 */
	
	public void insertFromXMLTextUI()
	{
		System.out.println ("\n...INSERT INTO MUSICDB...\n");
		System.out.println ("FileName:");
		String fileName = TextIO.getWord();
		ParseMusicXML.loadXMLMusicContentIntoDB(fileName,Config.getProperty("database","dbprak"));
	}

	/**
	 * UI zu updateNewPrice
	 * Aendert den newprice einer CD
	 * 
	 * @see MusicDB#updateNewPrice(String, float)
	 */
	
	public void updateNewPriceTextUI()
	{
		out.println ("... UPDATE SINGLE CD NEWPRICE...\n");
		
		String asinCode = getASINCodeTextUI();
		if(musicDB.showSingleCD(asinCode,out)== 1){
			out.println ("New LowNewPrice: ");
			float price 	= TextIO.getFloat();
			musicDB.updateNewPrice(asinCode,price);
			musicDB.showSingleCD(asinCode,out);
		}
	}
	
	/**
	 * UI zu deleteSingleCD
	 * Loescht eine CD und seine Tracks aus der DB
	 * 
	 * @see MusicDB#deleteSingleCD(String)
	 */
	
	public void deleteSingleCDTextUI()
	{
		out.println ("\n...DELETE A CD FROM MUSICDB...\n");
		String asinCode = getASINCodeTextUI();
		if(1==musicDB.showSingleCD(asinCode,out)){
			musicDB.deleteSingleCD(asinCode);
		}
	}
	
	/**
	 * Menueprogramm zu MusicDB
	 * @param args
	 */

	public static void main(String[] args) 
	{
		logger.debug("LIBRARYPATH: " + System.getProperty("java.library.path"));
		logger.debug("CLASSPATH: " + System.getProperty("java.class.path"));

		char dec;
		MusicDBTextUI musicDBTextUI = new MusicDBTextUI(System.out);

		System.out.println();
		System.out.println("Aufgabenblatt 5 - 'MusicDB'");
		// Ausgabe des Hauptmenus
		do
		{
			System.out.println ();
			System.out.println ("Main Menu");
			System.out.println ("========================================");
			System.out.println ("[L] ..Show all CDs sorted by artisit,title without Tracks");
			System.out.println ("[T] ..Show a selected CD by 'ASIN' and its sorted Tracks");
			System.out.println ("[P] ..Show average, min and max new and used prices");
			System.out.println ("[A] ..Show average, min and max new and used prices for an artist's CDs");
			System.out.println ("[I] ..Insert tuple from XML File");
			System.out.println ("[U] ..Update new price for a CD by 'ASIN'");
			System.out.println ("[D] ..Delete a CD by 'ASIN'");
			System.out.println ();
			System.out.println ("[Q] ..Quit");
					
			dec = Character.toUpperCase(TextIO.getChar());	
			
			switch (dec)
			{
				case 'L': musicDBTextUI.showAllCDsTextUI(); break;
				case 'T': musicDBTextUI.showSingleCDTextUI(); break;
				case 'P': musicDBTextUI.showGroupAllPricesTextUI(); break;
				case 'A': musicDBTextUI.showGroupPricesArtistTextUI(); break;
				case 'I': musicDBTextUI.insertFromXMLTextUI(); break;
				case 'U': musicDBTextUI.updateNewPriceTextUI(); break;
				case 'D': musicDBTextUI.deleteSingleCDTextUI(); break;
				case 'Q': break;
				default: System.out.println("Falsche Eingabe!"); break;
			}
		}while (dec != 'Q');
	}
}	
