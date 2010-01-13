package musicDB;
import java.io.PrintWriter;
import java.io.StringWriter;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

public class MusicDBGraphicUI {

	private Text text;
	private static Log logger = LogFactory.getLog(MusicDBGraphicUI.class);
	private MusicDB musicDB;
	
	public class ASINDialog extends Dialog {
		String result;
			
		public ASINDialog (Shell parent, int style) {
			super (parent, style);
		}
		public ASINDialog (Shell parent) {
			this (parent, 0);
		}
		public String open () {
			Shell parent = getParent();
			Shell shell = new Shell(parent, SWT.DIALOG_TRIM | SWT.APPLICATION_MODAL);
			shell.setText("Enter ASIN Code");
			GridLayout gl = new GridLayout();
			gl.horizontalSpacing = 10;
			gl.makeColumnsEqualWidth = false;
			gl.marginBottom = 5;
			gl.marginTop = 5;
			gl.marginLeft = 5;
			gl.marginRight = 5;
			gl.numColumns = 2;
			shell.setLayout(gl);

			final Label label = new Label(shell, SWT.BORDER);
			label.setText("ASIN:");
			
			final Text text = new Text(shell, SWT.BORDER);
			text.addKeyListener(new KeyAdapter() {
				public void keyPressed(final KeyEvent e) {
					if(e.keyCode == SWT.CR || e.keyCode == SWT.LF){
						result = text.getText();
						text.getParent().dispose();
					}
				}
			});
			text.setTextLimit(10);
			text.forceFocus();
			GridData gd = new GridData();
			gd.horizontalAlignment = GridData.FILL;
			gd.widthHint = 100;
			text.setLayoutData(gd);
			shell.pack();
			shell.open();
			Display display = parent.getDisplay();
			while (!shell.isDisposed()) {
				if (!display.readAndDispatch()) display.sleep();
			}
			return result;
		}
	 }

	public class ArtistDialog extends Dialog {
		String result;
			
		public ArtistDialog (Shell parent, int style) {
			super (parent, style);
		}
		public ArtistDialog (Shell parent) {
			this (parent, 0);
		}
		public String open () {
			Shell parent = getParent();
			Shell shell = new Shell(parent, SWT.DIALOG_TRIM | SWT.APPLICATION_MODAL);
			shell.setText("Enter Artistname");
			GridLayout gl = new GridLayout();
			gl.horizontalSpacing = 10;
			gl.makeColumnsEqualWidth = false;
			gl.marginBottom = 5;
			gl.marginTop = 5;
			gl.marginLeft = 5;
			gl.marginRight = 5;
			gl.numColumns = 2;
			shell.setLayout(gl);

			final Label label = new Label(shell, SWT.BORDER);
			label.setText("Artist:");
			final Text text = new Text(shell, SWT.BORDER);
			text.addKeyListener(new KeyAdapter() {
				public void keyPressed(final KeyEvent e) {
					if(e.keyCode == SWT.CR || e.keyCode == SWT.LF){
						result = text.getText();
						text.getParent().dispose();
					}
				}
			});
			text.forceFocus();
			GridData gd = new GridData();
			gd.horizontalAlignment = GridData.FILL;
			gd.widthHint = 200;
			gd.grabExcessHorizontalSpace = true;
			text.setLayoutData(gd);
			shell.pack();
			shell.open();
			Display display = parent.getDisplay();
			while (!shell.isDisposed()) {
				if (!display.readAndDispatch()) display.sleep();
			}
			return result;
		}
	 }
	
	public class FloatDialog extends Dialog {
		float result;
			
		public FloatDialog (Shell parent, int style) {
			super (parent, style);
			result = (float)-1.0;
		}
		public FloatDialog (Shell parent) {
			this (parent, 0);
			result = (float)-1.0;
		}
		public float open () {
			Shell parent = getParent();
			Shell shell = new Shell(parent, SWT.DIALOG_TRIM | SWT.APPLICATION_MODAL);
			shell.setText("Enter New Price");
			GridLayout gl = new GridLayout();
			gl.horizontalSpacing = 10;
			gl.makeColumnsEqualWidth = false;
			gl.marginBottom = 5;
			gl.marginTop = 5;
			gl.marginLeft = 5;
			gl.marginRight = 5;
			gl.numColumns = 2;
			shell.setLayout(gl);
			final Label label = new Label(shell, SWT.BORDER);
			label.setText("NewPrice:");
			final Text text = new Text(shell, SWT.BORDER);
			text.addKeyListener(new KeyAdapter() {
				public void keyPressed(final KeyEvent e) {
					if(e.keyCode == SWT.CR || e.keyCode == SWT.LF){
						try{
							result = Float.parseFloat(text.getText());
							text.getParent().dispose();
						}catch(NumberFormatException ex){
						}
					}
				}
			});
			GridData gd = new GridData();
			gd.horizontalAlignment = GridData.FILL;
			gd.widthHint = 100;
			gd.grabExcessHorizontalSpace = true;
			text.setLayoutData(gd);
			text.forceFocus();
			shell.pack();
			shell.open();
			Display display = parent.getDisplay();
			while (!shell.isDisposed()) {
				if (!display.readAndDispatch()) display.sleep();
			}
			return result;
		}
	 }
	
	public MusicDBGraphicUI(){
		musicDB = new MusicDB(Config.getProperty("database","dbprak"));
	}
	

	/**
	 * Launch the application
	 * @param args
	 */
	public static void main(String[] args) {
		logger.debug("LIBRARYPATH: " + System.getProperty("java.library.path"));
		logger.debug("CLASSPATH: " + System.getProperty("java.class.path"));
		try {
			MusicDBGraphicUI window = new MusicDBGraphicUI();
			window.open();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Open the window
	 */
	public void open() {
		final Display display = Display.getDefault();
		final Shell shell = new Shell();
		shell.setText("SWT Application");
		GridLayout rl = new GridLayout();
		rl.horizontalSpacing = 10;
		rl.makeColumnsEqualWidth = false;
		rl.numColumns = 2;
	    rl.marginLeft = 5;
	    rl.marginTop = 5;
	    rl.marginRight = 5;
	    rl.marginBottom = 5;
		shell.setLayout(rl);
		
	    final Composite c1 = new Composite(shell, SWT.NONE);
		GridData gd = new GridData();
		gd.verticalAlignment = 	GridData.VERTICAL_ALIGN_CENTER;
		gd.horizontalAlignment = GridData.HORIZONTAL_ALIGN_BEGINNING;
	    c1.setLayoutData(gd);
	    GridLayout c1GL = new GridLayout();
		c1GL.horizontalSpacing = 0;
		c1GL.verticalSpacing = 4;
		c1GL.makeColumnsEqualWidth = false;
		c1GL.numColumns = 1;
		c1GL.marginLeft = 2;
		c1GL.marginTop = 2;
		c1GL.marginRight = 2;
		c1GL.marginBottom = 2;
		c1.setLayout(c1GL);
		
		final Button quitButton = new Button(c1, SWT.PUSH);
		quitButton.addMouseListener(new MouseAdapter() {
			public void mouseDown(final MouseEvent e) {
				shell.setVisible(false);
				shell.dispose();
			}
		});
		quitButton.setText("Quit");
		gd = new GridData();
		gd.horizontalAlignment = GridData.FILL;
		gd.verticalAlignment = GridData.VERTICAL_ALIGN_CENTER;
		quitButton.setLayoutData(gd);

		final Button listAllCdsButton = new Button(c1, SWT.NONE);
		listAllCdsButton.addMouseListener(new MouseAdapter() {
			public void mouseDown(final MouseEvent e) {
				StringWriter strWriter = new StringWriter();
				PrintWriter out = new PrintWriter(strWriter,true);
				musicDB.showAllCDs(out);
				text.append(strWriter.toString());
				text.forceFocus();
			}
		});
		gd = new GridData();
		gd.horizontalAlignment = GridData.FILL;
		gd.verticalAlignment = GridData.VERTICAL_ALIGN_CENTER;
		listAllCdsButton.setLayoutData(gd);
		listAllCdsButton.setText("List All CDs");

		final Button listCdByButton = new Button(c1, SWT.NONE);
		listCdByButton.addMouseListener(new MouseAdapter() {
			public void mouseDown(final MouseEvent e) {
				final ASINDialog asind = new ASINDialog(shell, SWT.NONE);
				String asin = asind.open();
				if(asin!=null){
					StringWriter strWriter = new StringWriter();
					PrintWriter out = new PrintWriter(strWriter,true);
					musicDB.showSingleCD(asin,out);
					text.append(strWriter.toString());
				}
				text.forceFocus();
			}
		});
		gd = new GridData();
		gd.horizontalAlignment = GridData.FILL;
		gd.verticalAlignment = GridData.VERTICAL_ALIGN_CENTER;
		listCdByButton.setLayoutData(gd);
		listCdByButton.setText("List CD by ASIN");

		final Button insertFromXmlButton = new Button(c1, SWT.NONE);
		insertFromXmlButton.addMouseListener(new MouseAdapter() {
			public void mouseDown(final MouseEvent e) {
				final FileDialog fd = new FileDialog(shell, SWT.NONE);
				fd.setText("XML Load File");
				String ext[] = new String[1];
				ext[0] = new String("*.xml");
				fd.setFilterExtensions(ext);
				String fn = fd.open();
				if(fn!=null)
					ParseMusicXML.loadXMLMusicContentIntoDB(fn,Config.getProperty("database","dbprak"));
				text.forceFocus();
			}
		});
		insertFromXmlButton.setText("Insert From XML");
		gd = new GridData();
		gd.horizontalAlignment = GridData.FILL;
		gd.verticalAlignment = GridData.VERTICAL_ALIGN_CENTER;
		insertFromXmlButton.setLayoutData(gd);

		final Button updateCdNewpriceButton = new Button(c1, SWT.NONE);
		updateCdNewpriceButton.addMouseListener(new MouseAdapter() {
			public void mouseDown(final MouseEvent e) {
				final ASINDialog asind = new ASINDialog(shell, SWT.NONE);
				String asin = asind.open();
				final FloatDialog floatd = new FloatDialog(shell, SWT.NONE);
				float newPrice = floatd.open();
				if(asin!=null){
					musicDB.updateNewPrice(asin,newPrice);
				}
				text.forceFocus();
			}
		});
		updateCdNewpriceButton.setText("Update CD NewPrice");
		gd = new GridData();
		gd.horizontalAlignment = GridData.FILL;
		gd.verticalAlignment = GridData.VERTICAL_ALIGN_CENTER;
		updateCdNewpriceButton.setLayoutData(gd);

		final Button deleteCdButton = new Button(c1, SWT.NONE);
		deleteCdButton.addMouseListener(new MouseAdapter() {
			public void mouseDown(final MouseEvent e) {
				final ASINDialog asind = new ASINDialog(shell, SWT.NONE);
				String asin = asind.open();
				if(asin!=null){
					musicDB.deleteSingleCD(asin);
				}
				text.forceFocus();
			}
		});
		deleteCdButton.setText("Delete CD");
		gd = new GridData();
		gd.horizontalAlignment = GridData.FILL;
		gd.verticalAlignment = GridData.VERTICAL_ALIGN_CENTER;
		deleteCdButton.setLayoutData(gd);

		final Button showAvgButton = new Button(c1, SWT.NONE);
		showAvgButton.addMouseListener(new MouseAdapter() {
			public void mouseDown(final MouseEvent e) {
				StringWriter strWriter = new StringWriter();
				PrintWriter out = new PrintWriter(strWriter,true);
				musicDB.showGroupAllPrices(out);
				text.append(strWriter.toString());
				text.forceFocus();
			}
		});
		showAvgButton.setText("Show AVG All");
		gd = new GridData();
		gd.horizontalAlignment = GridData.FILL;
		gd.verticalAlignment = GridData.VERTICAL_ALIGN_CENTER;
		showAvgButton.setLayoutData(gd);

		final Button showAvgArtistButton = new Button(c1, SWT.NONE);
		showAvgArtistButton.addMouseListener(new MouseAdapter() {
			public void mouseDown(final MouseEvent e){
				final ArtistDialog artistd = new ArtistDialog(shell, SWT.NONE);
				String artist = artistd.open();
				if(artist!=null){
					StringWriter strWriter = new StringWriter();
					PrintWriter out = new PrintWriter(strWriter,true);
					musicDB.showGroupPricesArtist(artist,out);
					text.append(strWriter.toString());
				}
				text.forceFocus();
			}
		});
		showAvgArtistButton.setText("Show AVG By Artist");
		gd = new GridData();
		gd.horizontalAlignment = GridData.FILL;
		gd.verticalAlignment = GridData.VERTICAL_ALIGN_CENTER;
		showAvgArtistButton.setLayoutData(gd);

		text = new Text(shell, SWT.BORDER|SWT.READ_ONLY|SWT.MULTI| SWT.H_SCROLL | SWT.V_SCROLL);
		GridData gd1 = new GridData();
		gd1.heightHint = 400;
		gd1.widthHint = 600;
		gd1.verticalAlignment = 	GridData.FILL;
		gd1.horizontalAlignment = GridData.FILL;
		gd1.grabExcessHorizontalSpace = true;
		gd1.grabExcessVerticalSpace = true;
		text.setLayoutData(gd1);

		final Button clearTextButton = new Button(c1, SWT.NONE);
		clearTextButton.addMouseListener(new MouseAdapter() {
			public void mouseDown(final MouseEvent e){
				text.setText("");
				text.forceFocus();
			}
		});
		clearTextButton.setText("clear Textwindow");
		gd = new GridData();
		gd.horizontalAlignment = GridData.FILL;
		gd.verticalAlignment = GridData.VERTICAL_ALIGN_CENTER;
		clearTextButton.setLayoutData(gd);

		text.forceFocus();
		shell.pack();
		shell.open();
		while (!shell.isDisposed()) {
			if (!display.readAndDispatch())
				display.sleep();
		}
	}
}
