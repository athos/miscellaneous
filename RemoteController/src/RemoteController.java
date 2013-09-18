import java.util.List;
import gnu.io.CommPortIdentifier;
import java.util.Enumeration;

public class RemoteController {
    private PortManager manager;
    private Scenario scenario;

    public RemoteController(PortManager manager, Scenario scenario) {
        this.manager = manager;
        this.scenario = scenario;
    }

    public void mainLoop() {
        while (true) {
            try {
                Response res = manager.receiveResponse();
                int commandId = res.getCommandId();

                System.out.println("data received");
                res.print();
                
                scenario.notifyCommandDone(commandId);
                if (scenario.isFinished()) {
                    break;
                }

                List<Scenario.Node> nodes = scenario.getNext(commandId);
                for (Scenario.Node node : nodes) {
                    System.out.println("sending command");
                    node.cmd.print();
                    manager.send(node);
                }
            } catch (InterruptedException e) {
                System.out.println("interrupted");
            }
        }
    }
    
    public static void main(String arg[]) {
        int maxPorts = 4;
        String appName = "RemoteController";
        int timeout = 5000;
        int baudrate = 57600;

        // Enumeration identifiers = CommPortIdentifier.getPortIdentifiers();
        // while (identifiers.hasMoreElements()) {
        //     CommPortIdentifier identifier = (CommPortIdentifier)identifiers.nextElement();
        //     System.out.println(identifier.getName());
        // }

        //System.exit(0);

        PortManager manager = new PortManager(maxPorts, appName, timeout, baudrate);
        manager.addPort(1, new CommunicationPort("COM18"));
        //manager.addPort(2, new CommunicationPort("COM17"));
        Scenario scenario = ScenarioFactory.makeScenario();
        
        try {
            RemoteController controller = new RemoteController(manager, scenario);

            manager.connect();
            
            System.out.print("type enter key to start\n> ");
            System.out.flush();
            System.in.read();

            manager.send(scenario.getFirstNode());

            System.out.println("after sending command");

            controller.mainLoop();
        } catch(Exception e) {
            e.printStackTrace();
        } finally {
            try {
                manager.disconnect();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
