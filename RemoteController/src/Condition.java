import java.nio.ByteBuffer;

public class Condition {
    public static final int AND_TYPE = 0;
    public static final int OR_TYPE = 1;
    public static final int OTHER_TYPE = 2;

    public static final int EQ_BITS = 000;
    public static final int NE_BITS = 010;
    public static final int GT_BITS = 020;
    public static final int LT_BITS = 030;
    public static final int GE_BITS = 040;
    public static final int LE_BITS = 050;

    public static final int DICT_BITS = 0000;
    public static final int GYRO_BITS = 0100;
    public static final int SPEED_BITS = 0200;

    private int conditionType;
    private int targetValue;
    private Condition left, right;

    private Condition() {}
    
    public Condition(int conditionType, int targetValue) {
        this.conditionType = conditionType;
        this.targetValue = targetValue;
    }

    public Condition(int conditionType, Condition left, Condition right) {
        this.conditionType = conditionType;
        this.left = left;
        this.right = right;
    }

    private static boolean hasSubconditions(Condition cond) {
        return (cond.conditionType == AND_TYPE) || (cond.conditionType == OR_TYPE);
    }

    public boolean equals(Object o) {
        if (o == null || !(o instanceof Condition))
            return false;

        Condition cond = (Condition)o;
        
        if (hasSubconditions(cond)) {
            return conditionType == cond.conditionType
                && targetValue == cond.targetValue;
        } else {
            return conditionType == cond.conditionType
                && left.equals(cond.left)
                && right.equals(cond.right);
        }
    }

    public byte[] toByteArray() {
        ByteBuffer buffer;
        
        if (hasSubconditions(this)) {
            byte[] leftBytes = left.toByteArray();
            byte[] rightBytes = right.toByteArray();

            buffer = ByteBuffer.allocate(4 + leftBytes.length + rightBytes.length);

            buffer.putInt(conditionType);
            buffer.put(leftBytes);
            buffer.put(rightBytes);
        } else {
            buffer = ByteBuffer.allocate(8);

            buffer.putInt(conditionType);
            buffer.putInt(targetValue);
        }

        return buffer.array();
    }

    public static Condition createFromByteBuffer(ByteBuffer buffer) {
        Condition cond = new Condition();

        cond.conditionType = buffer.getInt();
        if (hasSubconditions(cond)) {
            cond.left = createFromByteBuffer(buffer);
            cond.right = createFromByteBuffer(buffer);
        } else {
            cond.targetValue = buffer.getInt();
        }
        return cond;
    }

    // for debug
    public void print() {
        print(0);
    }

    public void print(int depth) {
        println_with_indent("conditionType: " + conditionType, depth);
        if (hasSubconditions(this)) {
            left.print(depth + 2);
            right.print(depth + 2);
        } else {
            println_with_indent("targetValue: " + targetValue, depth);
        }
    }

    private void println_with_indent(String msg, int depth) {
        for (int i = 0; i < depth; i++) {
            System.out.print(" ");
        }
        System.out.println(msg);
    }
}
