package musicDB;

import org.apache.commons.configuration.CompositeConfiguration;
import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class Config {
	
    private static Log logger = LogFactory.getLog(Config.class);
    private static CompositeConfiguration config = null;
    
	static {
        config = new CompositeConfiguration();
        try {
            config.addConfiguration(new PropertiesConfiguration("musicDB.properties.user"));
        } catch (ConfigurationException e1) {
            logger.warn ("No user properties found. Using all defaults.");
            logger.warn (e1);
        }

        try {
            config.addConfiguration(new PropertiesConfiguration("musicDB.properties"));
        } catch (ConfigurationException e1) {
            logger.error ("Error loading properties. Aborting.",e1);
            throw new RuntimeException(e1);
        }
	}
	
	/**
	 * Liefert den Wert fuer das Property mit dem Namen propertyName
	 * zurueck
	 * @param propertyName propertyName
	 * @return String Wert des Property's
	 */
    
    public static String getProperty (String propertyName) {
        return config.getString(propertyName);
    }

	/**
	 * Liefert den Wert fuer das Property mit dem Namen propertyName
	 * zurueck, oder den default Wert
	 * @param propertyName propertyName
	 * @param def Defaultwert der zurueck geliefert wird, falls das Property
	 * nicht existiert
	 * @see Config#getProperty(String)
	 * @return String Wert des Property bzw der Defaultwert
	 */
    
    public static String getProperty (String propertyName,String def) {
    	String nm = config.getString(propertyName); 
        return (nm==null)? def : nm;
    }
}
