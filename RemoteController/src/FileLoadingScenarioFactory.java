import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.io.IOException;
import java.io.Reader;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;

class FileLoadingScenarioFactory extends ScenarioFactory {
    private static final String DEF_SECTION_KEYWORD = "[definitions]";
    private static final String DEP_SECTION_KEYWORD = "[dependencies]";
    private static final String COMMENT_SYMBOL = "#";

    private List<String> definitions;
    private List<String> dependencies;
    private Map<String, Scenario.Node>nodes;
    private BufferedReader reader;

    private enum Status { PROLOGUE, DEFINITIONS, DEPENDENCIES }

    FileLoadingScenarioFactory(Reader reader) {
        this.reader = new BufferedReader(reader);
        definitions = new ArrayList<String>();
        dependencies = new ArrayList<String>();
        nodes = new HashMap<String, Scenario.Node>();
    }

    private void load() throws IOException {
        String line;
        Status status = Status.PROLOGUE;
        while ((line = reader.readLine()) != null) {
            if (line.equals(DEF_SECTION_KEYWORD)) {
                if (status != Status.PROLOGUE) {
                    throw new IOException("definition section was duplicated");
                } else {
                    status = Status.DEFINITIONS;
                    continue;
                }
            } else if (line.equals(DEP_SECTION_KEYWORD)) {
                if (status != Status.DEFINITIONS) {
                    throw new IOException("dependency section must be after definition section");
                } else {
                    status = Status.DEPENDENCIES;
                    continue;
                }
            }

            switch (status) {
                case PROLOGUE:
                    continue;
                case DEFINITIONS:
                    if (!line.equals("") && !line.startsWith(COMMENT_SYMBOL)) {
                        definitions.add(line);
                    }
                    break;
                case DEPENDENCIES:
                    if (!line.equals("") && !line.startsWith(COMMENT_SYMBOL)) {
                        dependencies.add(line);
                    }
                    break;
            }
        }
    }

    private void loadDefinitions(Scenario scenario) throws Exception {
        CommandParser parser = new CommandParser();

        for (String definition : definitions) {
            String[] parts = definition.split(":");
            for (int i = 0; i < parts.length; i++) {
                parts[i] = parts[i].trim();
            }
            if (parts.length != 2 || parts[0].equals("") || parts[1].equals("")) {
                throw new IOException(definition + " is not a valid defition format");
            }

            Command command = parser.parse(parts[1]);
            Scenario.Node node = scenario.makeNode(command);
            nodes.put(parts[0], node);
            scenario.addNode(node);
        }
    }

    private void loadDependencies(Scenario scenario) throws Exception {

    }

    public void make(Scenario scenario) {
        try {
            load();

            loadDefinitions(scenario);
        } catch (Exception e) {
            throw new RuntimeException(e);
        } finally {
            try {
                reader.close();
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
    }

    public static void main(String[] args) {
        InputStream in = FileLoadingScenarioFactory.class.getResourceAsStream("scenario.ini");
        InputStreamReader reader = null;
        try {
            reader = new InputStreamReader(in);

            FileLoadingScenarioFactory factory = new FileLoadingScenarioFactory(reader);
            Scenario scenario = factory.makeScenario();

            Command cmd = scenario.getFirstCommand();
            cmd.print();
            scenario.notifyCommandDone(cmd.getId());
            while (!scenario.isFinished()) {
                List<Command> nextCommands = scenario.getNextCommands(cmd.getId());
                cmd = nextCommands.get(0);
                cmd.print();
                scenario.notifyCommandDone(cmd.getId());
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
