package de.hu_berlin.informatik.dbis.nk.a4.util;

import org.apache.hadoop.io.Text;

/**
 * Do not change anything here!
 * 
 * @author rico
 * 
 */
public class EventText {

	public static Text getEventTextKey(Event event) {
		return new Text("");
	}

	public static Text getEventTextValue(Event event) {
		Text text = new Text();

		StringBuffer stb = new StringBuffer();
		stb.append("Event: Police from [");
		stb.append(event.policemanFromLine);
		stb.append(",");
		stb.append(event.policemanFromColumn);
		stb.append("] to [");
		stb.append(event.policemanToLine);
		stb.append(",");
		stb.append(event.policemanToColumn);
		stb.append("] Criminal from [");
		stb.append(event.criminalFromLine);
		stb.append(",");
		stb.append(event.criminalFromColumn);
		stb.append("] to [");
		stb.append(event.criminalToLine);
		stb.append(",");
		stb.append(event.criminalToColumn);
		stb.append("]");

		text.set(stb.toString());
		return text;
	}
}
