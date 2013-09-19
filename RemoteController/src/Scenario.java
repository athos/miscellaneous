import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

public class Scenario {
    public class Node {
        private List<Node> nextNodes;
        private List<Node> prevNodes;
        private Command cmd;
        private boolean done = false;

        private Node(Command cmd) {
            this.cmd = cmd;
            nextNodes = new ArrayList<Node>();
            prevNodes = new ArrayList<Node>();
        }

        public Command getCommand() {
            return cmd;
        }

        private boolean isReadyToStart() {
            for (Node prev : prevNodes) {
                if (!prev.done) {
                    return false;
                }
            }
            return true;
        }
    }

    private Map<Integer, Node> map;
    private Node start, end;

    public Scenario() {
        start = null;
        end = null;
        map = new HashMap<Integer, Node>();
    }

    public void connectNodes(Node parent, Node node) {
        parent.nextNodes.add(node);
        node.prevNodes.add(parent);
        if (parent == end) {
            end = node;
        }
    }

    public Node makeNode(Command cmd, Node... parents) {
        Node node = new Node(cmd);
        for (Node parent : parents) {
            connectNodes(parent, node);
        }
        map.put(cmd.getId(), node);

        return node;
    }

    public void addNode(Node node) {
        if (start == null) {
            start = node;
            end = node;
        } else {
            connectNodes(end, node);
            end = node;
        }
    }

    public Command getFirstCommand() {
        return start.cmd;
    }

    public boolean isFinished() {
        return end.done;
    }

    private Node findNodeByCommandId(int commandId) {
        return map.get(commandId);
    }

    public void notifyCommandDone(int commandId) {
        Node node = findNodeByCommandId(commandId);
        node.done = true;
    }

    public List<Command> getNextCommands(int commandId) {
        Node node = findNodeByCommandId(commandId);

        List<Command> ret = new ArrayList<Command>();
        for (Node next : node.nextNodes) {
            if (next.isReadyToStart()) {
                ret.add(next.cmd);
            }
        }
        return ret;
    }
}
