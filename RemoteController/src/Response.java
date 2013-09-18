public class Response {
    private Command com;

    public Command getData() {
        return com;
    }

    public int getCommandId() {
        return com.getId();
    }

    public static Response createFromByteArray(byte[] array, int len) {
        Response response = new Response();
        
        try {
            response.com = Command.createFromByteArray(array);
        } catch (Exception e) {
        }
        return response;
    }

    // for debug
    public void print() {
        com.print();
    }
}
