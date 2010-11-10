import java.io.*;
import java.util.*;

import org.jdom.*;
import org.jdom.filter.ElementFilter;
import org.jdom.input.*;
import org.jdom.output.*;

public class XMLExtraktiona {

    public static void main(String[] args) throws Exception {
        if (args.length != 2) {
            System.err.println("Bitte genau zwei Parameter angeben!");
            System.err.println("1. Parameter: URI des Quell-XML-Dokuments");
            System.err.println("2. Parameter: URI des Ausgabe-XML-Dokuments");
            return;
        }

		try {
			// ---- Einlesen der XML Quelle ----
 
			// Einstellungen zum Validieren der Datei: http://forum.java.sun.com/thread.jspa?threadID=570593&tstart=120
			//Create a SAXBuilder parser.
			SAXBuilder saxBuilder =new SAXBuilder("org.apache.xerces.parsers.SAXParser",true);
 
			//Set the validation feature.
			saxBuilder.setFeature("http://xml.org/sax/features/validation",  true);
 
			//Set the validation/schema feature.
			saxBuilder.setFeature("http://apache.org/xml/features/validation/schema",  true);
 
			//Set validation/schema-full-checking feature.
			saxBuilder.setFeature("http://apache.org/xml/features/validation/schema-full-checking", true);
                                                                  
			Document docQuelle = saxBuilder.build( new File( args[0] ) );

			// ---- Suchen und Erzeugen der XML-Daten ----
			Document docZiel = new Document( new Element("Angestellte") );
			Element eQuellWurzel = docQuelle.getRootElement();
			Element eZielWurzel = docZiel.getRootElement();
			
			// Angestellte suchen
			Iterator iAngestellteOriginal = eQuellWurzel.getDescendants( new ElementFilter("Angestellter") ); 
			for (; iAngestellteOriginal.hasNext(); )
			{
				Element eAngestellterOriginal = (Element) iAngestellteOriginal.next();
				if( null == eAngestellterOriginal )  continue;
				Element eAngestellter = (Element)eAngestellterOriginal.clone();
				
				// Verweise/Referenzen auf Fähigkeiten suchen
				Iterator iRefrenzenOriginal = eAngestellterOriginal.getDescendants( new ElementFilter("Referenz") ); 
				for (; iRefrenzenOriginal.hasNext(); )
				{
					// System.out.println("Referenz");
					Element eReferenzOriginal = (Element) iRefrenzenOriginal.next();
					if( null == eReferenzOriginal )  continue;
					// Wert des Verweises ermitteln
					String strReferenzOriginal = eReferenzOriginal.getAttributeValue("Fähigkeit");
					// System.out.println("Referenzwert: " + strReferenzOriginal);					

					// passende Fähigkeit suchen
					Iterator iFähigkeiten = eQuellWurzel.getDescendants( new ElementFilter("Fähigkeit") ); 
					for (; iFähigkeiten.hasNext(); )
					{
						Element eFaehigkeitOriginal = (Element) iFähigkeiten.next();
						if( null == eFaehigkeitOriginal || eFaehigkeitOriginal.getAttributeValue("Schlüssel").compareTo(strReferenzOriginal) != 0) continue; 
						
						// Fähigkeit hinzufügen
						Element eFaehigkeit = (Element)eFaehigkeitOriginal.clone();						
						eAngestellter.addContent( eFaehigkeit );
						// System.out.println("Fähigkeit hinzugefügt");
					}
				}
				// Entfernen der Verweise
				eAngestellter.removeContent(new ElementFilter("Referenz"));


				// Zuordnungen suchen
				Iterator iZuordnungenOriginal = eQuellWurzel.getDescendants( new ElementFilter("Zuordnung") ); 
				for (; iZuordnungenOriginal.hasNext(); )
				{
					// System.out.println("Zuordung");
					Element eZuordnungOriginal = (Element) iZuordnungenOriginal.next();
					if( null == eZuordnungOriginal  || eZuordnungOriginal.getAttributeValue("Angestellter").compareTo(eAngestellter.getAttributeValue("Schlüssel")) != 0)  continue;

					// Schlüssel der Aufgabe ermitteln
					String strAufgabeOriginal = eZuordnungOriginal.getAttributeValue("Aufgabe");
					// System.out.println("Aufgabenschlüssel: " + strAufgabeOriginal);					

					// passende Aufgabe suchen
					Iterator iAufgabenOriginal = eQuellWurzel.getDescendants( new ElementFilter("Aufgabe") ); 
					for (; iAufgabenOriginal.hasNext(); )
					{
						Element eAufgabeOriginal = (Element) iAufgabenOriginal.next();
						if( null == eAufgabeOriginal || eAufgabeOriginal.getAttributeValue("Schlüssel").compareTo(strAufgabeOriginal) != 0) continue; 
						
						// Aufgabe hinzufügen
						Element eAufgabe = (Element)eAufgabeOriginal.clone();
						eAngestellter.addContent(eAufgabe);
						eAufgabe.removeContent(new ElementFilter("Referenz"));

						// System.out.println("Aufgabe hinzugefügt");
					}
				}

				
				// bearbeiteten Angestellten hinzufügen
				eZielWurzel.addContent( eAngestellter );
			}
			// ---- Ausgabe der neuen XML Datei ----
			Format fAusgabeformat = Format.getPrettyFormat();
			fAusgabeformat.setIndent("    ");
			XMLOutputter outp = new XMLOutputter(fAusgabeformat);
			outp.output( docZiel, new FileOutputStream( new File( args[1] ) ) );
		} catch( Exception ex ) {
		ex.printStackTrace();
		}
	}
}