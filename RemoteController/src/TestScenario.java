public class TestScenario extends Scenario {
    public void make() {
        start = new Node(new Command(10, 11, new Condition(12, 13)), 1);
        end = new Node(new Command(30, 31, new Condition(32, 33)), 1,
                       new Node(new Command(20, 21, new Condition(22, 23)), 1, start));
    }
}
