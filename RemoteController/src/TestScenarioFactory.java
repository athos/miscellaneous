class TestScenarioFactory extends ScenarioFactory {
    protected void make(Scenario scenario) {
        Scenario.Node node1 = scenario.makeNode(new Command(1, 10, 11, new Condition(12, 13)));
        scenario.addNode(node1);
        Scenario.Node node2 = scenario.makeNode(new Command(1, 20, 21, new Condition(22, 23)));
        scenario.addNode(node2);
        Scenario.Node node3 = scenario.makeNode(new Command(1, 30, 31, new Condition(32, 33)));
        scenario.addNode(node3);
    }
}
