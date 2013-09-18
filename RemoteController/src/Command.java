import java.nio.ByteBuffer;

public class Command {
    private int id;
    private int commandType;
    private int parameter;
    private Condition condition;

    private static int nextId = 0;
    
    private Command() {}
    
    public Command(int commandType, int parameter, Condition condition) {
        this.commandType = commandType;
        this.parameter = parameter;
        this.condition = condition;
        id = nextId;
        nextId++;
    }

    public int getId() {
        return id;
    }

    public boolean equals(Object o) {
        if (o == null || !(o instanceof Command))
            return false;

        Command com = (Command)o;
        
        return commandType == com.commandType
            && parameter == com.parameter
            && condition.equals(com.condition);
    }
    
    public byte[] toByteArray() {
        byte[] cond = condition.toByteArray();

        byte size = (byte)(13 + cond.length);
        ByteBuffer buffer = ByteBuffer.allocate(size);
        buffer.put(size);
        buffer.putInt(id);
        buffer.putInt(commandType);
        buffer.putInt(parameter);
        buffer.put(cond);

        return buffer.array();
    }

    public static Command createFromByteArray(byte[] array) {
        ByteBuffer buffer = ByteBuffer.wrap(array);
        Command com = new Command();

        buffer.get(); // omit command size
        com.id = buffer.getInt();
        com.commandType = buffer.getInt();
        com.parameter = buffer.getInt();
        com.condition = Condition.createFromByteBuffer(buffer);

        return com;
    }

    // for debug
    public void print() {
        System.out.println("id: " + id);
        System.out.println("commandType: " + commandType);
        System.out.println("parameter: " + parameter);
        condition.print();
    }
    
    public static void main(String[] args) {
        Command com = new Command(100,
                                  200,
                                  new Condition(0,
                                                new Condition(3, 300),
                                                new Condition(4, 400)));
        for (byte b : com.toByteArray()) {
            System.out.print(b + " ");
        }
        assert com.equals(Command.createFromByteArray(com.toByteArray()));
    }
}
