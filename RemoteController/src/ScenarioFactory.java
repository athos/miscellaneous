import java.io.InputStream;
import java.io.InputStreamReader;

abstract public class ScenarioFactory {
    public static ScenarioFactory getDefault() {
        InputStream in = ScenarioFactory.class.getResourceAsStream("scenario.ini");
        try {
            InputStreamReader reader = new InputStreamReader(in);
            return new FileLoadingScenarioFactory(reader);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    public Scenario makeScenario() {
        Scenario scenario = new Scenario();
        make(scenario);

        return scenario;
    }

    abstract protected void make(Scenario scenario);
}
