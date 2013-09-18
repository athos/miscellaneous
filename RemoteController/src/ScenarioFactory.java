public class ScenarioFactory {
    public static Scenario makeScenario() {
        Scenario scenario = new TestScenario();
        scenario.make();

        return scenario;
    }
}
