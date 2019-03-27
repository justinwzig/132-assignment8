import jssc.SerialPortException;
import java.awt.event.KeyEvent;

public class PCPlayer {

	final private SerialComm port;
	
	private long sendTimer = 0;
	private long sendDelay = 200;
	private boolean canSend = false;

	public PCPlayer(String portname) throws SerialPortException {
		port = new SerialComm(portname);

		port.setDebug(true);

		new Thread(new Runnable() {
			public void run() {
				try {
					port.buffer();
				} catch (SerialPortException e) {
					e.printStackTrace();
				}
			}
		}).start();

		System.out.println("DEBUG: Continued");
	}

	public void run() {
		while(true) {
		long now = java.lang.System.currentTimeMillis();
		if(now > sendTimer) {
			canSend = true;
			sendTimer = now + sendDelay;
		}
		
		if(canSend) {
			if(StdDraw.isKeyPressed(38)) { //DOWN
				System.out.println("Sending DOWN (2)");
				port.writeByte((byte)2);
			} else if(StdDraw.isKeyPressed(39)) { //RIGHT
				System.out.println("Sending RIGHT (1)");
				port.writeByte((byte)1);
			} else if(StdDraw.isKeyPressed(40)) { //UP
				System.out.println("Sending UP (0)");
				port.writeByte((byte)0);
			} else if(StdDraw.isKeyPressed(37)) { //LEFT
				System.out.println("Sending LEFT (3)");
				port.writeByte((byte)3);
			}
		}
			
		canSend = false;
		}
	}

	public static void main(String[] args) throws InterruptedException, SerialPortException {
		PCPlayer player = new PCPlayer("COM4");
		Thread.sleep(10);
		player.run();
	}
}