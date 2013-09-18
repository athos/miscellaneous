import gnu.io.SerialPort;
import gnu.io.SerialPortEventListener;
import gnu.io.SerialPortEvent;
import gnu.io.CommPortIdentifier;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.util.List;
import java.util.ArrayList;

public class CommunicationPort implements SerialPortEventListener {
    private String portIdentifier;
    private SerialPort port;
    public String appName = "";
    public int timeout = 5000;
    public int baudrate = 57600;
    private List<ResponseListener> listeners; 

    public CommunicationPort(String portIdentifier) {
        this.portIdentifier = portIdentifier;
        listeners = new ArrayList<ResponseListener>();
    }

    public void connect() throws Exception {
        CommPortIdentifier com = CommPortIdentifier.getPortIdentifier(portIdentifier);
        SerialPort port = (SerialPort)com.open(appName , timeout);
        port.setSerialPortParams(baudrate,
                                 SerialPort.DATABITS_5,
                                 SerialPort.STOPBITS_1,
                                 SerialPort.PARITY_NONE);
        port.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
        port.addEventListener(this);
        port.notifyOnDataAvailable(true);
        this.port = port;
    }

    public void addListener(ResponseListener listener) {
        listeners.add(listener);
    }
    
    public void disconnect() throws IOException {
        port.getOutputStream().close();
        port.getInputStream().close();
        port.close();
    }

    private void send(byte[] data) {
        try {
            OutputStream out = port.getOutputStream();

            out.write(data);
            out.flush();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void send(Command com) {
        send(com.toByteArray());
    }

    public void serialEvent(SerialPortEvent event) {
        if (event.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
            try {
                InputStream in = port.getInputStream();
                byte[] buffer = new byte[64];

                int len = in.read(buffer);
                if (len != -1) {
                    Response res = Response.createFromByteArray(buffer, len);
                    for (ResponseListener listener : listeners) {
                        listener.responseReceived(res);
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
