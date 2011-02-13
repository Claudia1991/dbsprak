/**
 * 
 */
package de.hu_berlin.informatik.dbis.nk.a4.util;

/**
 * @author rico
 * 
 */
public class Decoder {

	public static int decodeType(int encodedInt) {
		return encodedInt % 4;
	}

	public static int decodeEvent(int encodedInt) {
		return encodedInt % 3;
	}
}
