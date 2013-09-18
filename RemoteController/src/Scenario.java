import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

abstract public class Scenario {
    public static class Node {
        List<Node> nextNodes;
        List<Node> prevNodes;
        Command cmd;
        int portId;
        boolean done = false;

        static Map<Integer, Node> map = new HashMap<Integer, Node>();

        Node(Command cmd, int portId, Node... nodes) {
            this.cmd = cmd;
            this.portId = portId;
            nextNodes = new ArrayList<Node>();
            prevNodes = new ArrayList<Node>();
            map.put(cmd.getId(), this);
            for (Node prev : nodes) {
                prevNodes.add(prev);
                prev.nextNodes.add(this);
            }
        }

        static Node findByCommandId(int commandId) {
            return map.get(commandId);
        }

        boolean isReadyToStart() {
            for (Node prev : prevNodes) {
                if (!prev.done) {
                    return false;
                }
            }
            return true;
        }
    }

    protected Node start, end;

    abstract public void make();

    public Node getFirstNode() {
        return start;
    }

    public boolean isFinished() {
        return end.done;
    }
    
    public void notifyCommandDone(int commandId) {
        Node node = Node.findByCommandId(commandId);
        node.done = true;
    }

    public List<Node> getNext(int commandId) {
        Node node = Node.findByCommandId(commandId);

        List<Node> ret = new ArrayList<Node>();
        for (Node next : node.nextNodes) {
            if (next.isReadyToStart()) {
                ret.add(next);
            }
        }
        return ret;
    }
}
