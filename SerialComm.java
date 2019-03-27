import jssc.*;
import java.util.ArrayList;

public class SerialComm {

	SerialPort port;

	private boolean debug;  // Indicator of "debugging mode"
	private ArrayList<Byte> buffer = new ArrayList<Byte>();
	
	// This function can be called to enable or disable "debugging mode"
	void setDebug(boolean mode) {
		debug = mode;
	}	
	

	// Constructor for the SerialComm class
	public SerialComm(String name) throws SerialPortException {
		port = new SerialPort(name);		
		port.openPort();
		port.setParams(SerialPort.BAUDRATE_9600,
			SerialPort.DATABITS_8,
			SerialPort.STOPBITS_1,
			SerialPort.PARITY_NONE);
		
		debug = false; // Default is to NOT be in debug mode
	}
		
	// TODO: Add writeByte() method 
	public void writeByte(byte b) {
		if (debug) {
			System.out.println("Wrote: 0x" + b);
		}
		try {
			port.writeByte(b);
		} catch (SerialPortException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public boolean available() {
		try {
			if (port.getInputBufferBytesCount() > 0) {
				return true;
			} else {
				return false;
			}
		} catch (SerialPortException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
	}
	
	// TODO: Add available() method
	
	// TODO: Add readByte() method	
	public void buffer() throws SerialPortException {
		while (true) {
			//System.out.println("Buffer loop");
			if (this.available()) {
				//System.out.println("Buffer avaial");
				for (byte b : port.readBytes()) {
					//System.out.println("Buffer added");
					buffer.add(b);
					if (debug) {
						System.out.println("Heard: 0x" + b);
					}
				}
			}
		}
	}
	
	
	public boolean bufferAvailable() {
		//System.out.println("bufferAvail method");
		if(buffer.size()>8) {
			return true;
		} else {
			//System.out.println("buffer avail false");
			return false;
		}
	}
	
	public byte readNext() {
		byte out = buffer.get(0);
		String toPrint = buffer.get(0).toString();
		buffer.remove(0);
		//System.out.println("Buffer Contents: " + toPrint);
		return out;
	}
	
	
}
