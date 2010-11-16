package Aufgabe1;

/*
 * TODO:
 *  - ('extra') Fehlerbehandlung -> z.b. pruefen, ob benutzte Dateien schon/ueberhaupt vorhanden sind?!
 *  	-> Ordner?!
 *  - Validierung der XML-Dateien?
 *  - Wie koennen "Listen" noch (anders) gespeichert werden?
 *  - weitere Parameter!?
 *  - ueberpruefung der zugegriffenen Elemente??
 *  - Kommentare aus Ziel-XML-Datei entfernen??
 *  - Obligatorische Faehigkeit beachten??? -> steht nix im Schema!?!
 *  - Faehigkeiten-Elemente werden zur Zeit nicht verwendet!
 *  - Sonstiges????? :P
 *  	-> TESTS!
 *  	-> Word-Xml-Dokument erstellen? (Fkt.)
 */

	import java.io.*;
	import java.util.*;

	import org.jdom.*;
	import org.jdom.filter.ElementFilter;
	import org.jdom.input.*;
	import org.jdom.output.*;

	public class Beauftragung {

		public static void main(String[] args) throws Exception {
	        if (args.length != 2) {
	            System.err.println("Bitte genau zwei Parameter angeben!");
	            System.err.println("1. Parameter: URI des FOAF-XML-Dokuments");
	            System.err.println("2. Parameter: URI des AufgabenFaehigkeitenZuordnung-XML-Dokuments");
	            return;
	        }

			try {
				SAXBuilder builder = new SAXBuilder();
				// Dokumente laden
				Document FOAF_doc = builder.build(args[0]);
				Document AufgFaehig_doc = builder.build(args[1]);
				
				// Wurzelelemente ermitteln
				Element FOAF_doc_Root = FOAF_doc.getRootElement();
				Element AufgFaehig_doc_Root = AufgFaehig_doc.getRootElement();
				
				// Namensraeume festelegen
				Namespace foafNS = Namespace.getNamespace("foaf", "http://xmlns.com/foaf/0.1/");
				Namespace rdfNS = Namespace.getNamespace("rdf", "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
				Namespace aufgNS = Namespace.getNamespace("aufg", "http://www.example.com/Aufgaben");
				Namespace aufgDefaultNS = Namespace.getNamespace("http://www.example.com/Aufgabendefinition");
				
				// Alle moeglichen Aufgaben ermitteln und deren genutzten Faehigkeiten in einer "Liste" speichern
				Map<String, Set<String>> aufgabenListe = new HashMap<String, Set<String>>();
				
				List<Element> aufgabenF = AufgFaehig_doc_Root.getChild("Aufgaben",aufgDefaultNS).getChildren("Aufgabe",aufgNS);
				ListIterator<Element> aufgabenFI = aufgabenF.listIterator();
				for (; aufgabenFI.hasNext(); ){
					Element aufgabe = aufgabenFI.next();
					List<Element> referenzen = aufgabe.getChildren("Referenz",aufgNS);
					// Aufgabenfaehigkeiten ermitteln und in einer "Menge" (Set) speichern
					Set<String> faehigkeitenSet = new HashSet<String>();
					ListIterator<Element> referenz = referenzen.listIterator();
					for (; referenz.hasNext(); ){
						faehigkeitenSet.add(referenz.next().getAttributeValue("Fähigkeit"));
					}
					aufgabenListe.put(aufgabe.getChildText("Bezeichnung", aufgNS), faehigkeitenSet);
				}
				
				// Ueber alle Personen iterieren
				List<Element> personen = FOAF_doc_Root.getChildren("Person",foafNS);
				ListIterator<Element> it = personen.listIterator();
				for (; it.hasNext(); ){
					Element person = it.next();
					if( null == person )  continue;
					
					Set<String> faehigkeitenSet = new HashSet<String>(); 
					
					// Faehigkeiten der Person ermitteln und in einer Menge speichern
					Iterator faehigkeiten = person.getDescendants(new ElementFilter("interest",foafNS));
					for (; faehigkeiten.hasNext(); ){
						faehigkeitenSet.add(((Element) faehigkeiten.next()).getChild("Description", rdfNS).getAttributeValue("about", rdfNS));
					}
					
					// Mengenvergleich: Alle Aufgaben testen, ob die Aufgabenfaehigkeiten Teilmenge der Personenfaehigkeiten ist => Aufgabe kann von Person ausgefuehrt werden
					Vector<String> bezeichnung = new Vector<String>();
					for ( String elem : aufgabenListe.keySet() ){
						if(faehigkeitenSet.containsAll(aufgabenListe.get(elem))){
							bezeichnung.add(elem);
						}
					}
					
					// ODT-XML-Dokument fuer jede Person generieren/erstellen
					createBeauftragungODT(person.getChild("name", foafNS).getText(),
							person.getChild("title", foafNS).getText(),
							person.getChild("phone", foafNS).getAttributeValue("resource", rdfNS),
							person.getChild("workplaceHomepage", foafNS).getAttributeValue("resource", rdfNS),
							bezeichnung);
				}
			} catch( Exception ex ) {
			ex.printStackTrace();
			}
			System.out.println("Fertig!");
		}

		/**
	     * Funktion zum erstellen der Ausgabe-XML-Datei fuer ODT-Dateien
	     * @param name - der zu erstellenden Personen-Datei
	     * @param title - Ansprache (Herr/Frau)
	     * @param telefon - Telefonnr der Person
	     * @param webseite - Webseite des Arbeitgebers
	     * @param bezeichnung - StringArray fuer die zugewiesenen Aufgaben
	     */
	    public static void createBeauftragungODT(String name, String title, String telefon, String webseite, Vector<String> bezeichnung) {
	    	SAXBuilder builder = new SAXBuilder();
	    	try {
	    		// Namensraum "text" setzen/ermitteln
	    		Namespace textNS = Namespace.getNamespace("text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");

	    		// ODT-Vorlage laden
				Document Odt_doc = builder.build("xml/beauftragung/content.xml");
				
				// Wurzel bestimmen
				Element odtRoot = Odt_doc.getRootElement();
				
				// Inhaltselement ermitteln
				Element odtBodyText = odtRoot.getChild("body",odtRoot.getNamespace()).getChild("text",odtRoot.getNamespace());

				// erstes zu aendernde Element ermitteln und Text ersetzen
				odtBodyText.getChild("h",textNS).setText("Beauftragung für "+name);

				// Personenbeschreibungselement ermitteln und Namen einfuegen
				odtBodyText.getChild("p",textNS).getChild("span",textNS).setText("Name: "+title+" "+name);

				// Position ermitteln, ab der die "restlichen" Elemente eingefuegt werden koennen
				int nodeStartPosition = odtBodyText.indexOf(odtBodyText.getChild("p",textNS))+1;
				
				// fehlende Elemente (Telefon, Webseite) hinzufuegen (an entsprechender Position)
				odtBodyText.addContent(nodeStartPosition++, new Element("p",textNS).setAttribute(new Attribute("style-name","Text_20_body",textNS)).addContent(new Element("span",textNS).setAttribute(new Attribute("style-name","T1",textNS)).setText("Telefon: "+telefon)));
				odtBodyText.addContent(nodeStartPosition++, new Element("p",textNS).setAttribute(new Attribute("style-name","Text_20_body",textNS)).addContent(new Element("span",textNS).setAttribute(new Attribute("style-name","T1",textNS)).setText("Webseite der Arbeitsstelle: "+webseite)));
				
				// 2. Ueberschrift einfuegen
				odtBodyText.addContent(nodeStartPosition++, new Element("h",textNS).setAttribute(new Attribute("style-name","Heading_20_2",textNS)).setAttribute(new Attribute("outline-level","2",textNS)).setText("Zugewiesene Aufgaben"));
				
				// Fuer jede gefundene Aufgabe, deren Beschreibung ausgeben. Falls keine Aufgaben vorhanden - ebenfalls ausgeben
				if(bezeichnung.size() <= 0) odtBodyText.addContent(nodeStartPosition++, new Element("p",textNS).setAttribute(new Attribute("style-name","Text_20_body",textNS)).addContent(new Element("span",textNS).setAttribute(new Attribute("style-name","T1",textNS)).setText("Keine Aufgaben vorhanden!")));
				else {
					Iterator<String> it = bezeichnung.iterator();
					for(;it.hasNext();){
						odtBodyText.addContent(nodeStartPosition++, new Element("p",textNS).setAttribute(new Attribute("style-name","Text_20_body",textNS)).addContent(new Element("span",textNS).setAttribute(new Attribute("style-name","T1",textNS)).setText("Bezeichnung: "+it.next())));
					}
				}

				// ---- Write XML file ----
				XMLOutputter outp = new XMLOutputter();
		        outp.setFormat( Format.getPrettyFormat() );
			    outp.output( Odt_doc, new FileOutputStream( new File( "xml/content_"+name+".xml" ) ) );
	    	} catch( Exception ex ) {
				ex.printStackTrace();
			}
		}
	    public static void createBeauftragungWord(String name, String title, String telefon, String webseite, Vector<String> bezeichnung) {
	    	// TODO
	    }
	}