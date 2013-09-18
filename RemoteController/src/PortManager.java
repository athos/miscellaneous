import java.util.concurrent.ArrayBlockingQueue;
import java.util.Map;
import java.util.HashMap;

public class PortManager implements ResponseListener {
    private String appName;
    private int timeout;
    private int baudrate;

    private ArrayBlockingQueue<Response> responseQueue;
    private Map<Integer, CommunicationPort> ports;

    public PortManager(int maxNumPorts, String appName, int timeout, int baudrate) {
        this.appName = appName;
        this.timeout = timeout;
        this.baudrate = baudrate;
        responseQueue = new ArrayBlockingQueue<Response>(maxNumPorts);
        ports = new HashMap<Integer, CommunicationPort>();
    }

    public CommunicationPort getPortById(int id) {
        return ports.get(id);
    }

    public void addPort(int portId, CommunicationPort port) {
        port.addListener(this);
        ports.put(portId, port);
    }

    public void connect() throws Exception {
        for (CommunicationPort port : ports.values()) {
            port.appName = appName;
            port.timeout = timeout;
            port.baudrate = baudrate;
            port.connect();
        }
    }

    public void disconnect() throws Exception {
        for (CommunicationPort port : ports.values()) {
            port.disconnect();
        }
    }

    public void send(Scenario.Node node) {
        CommunicationPort port = getPortById(node.portId);
        port.send(node.cmd);
    }
    
    public void responseReceived(Response res) {
        responseQueue.add(res);
    }

    public Response receiveResponse() throws InterruptedException {
        return responseQueue.take();
    }
}
