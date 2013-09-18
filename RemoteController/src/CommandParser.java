public class CommandParser {
    class Pair<F,S> {
        F v1;
        S v2;

        Pair(F v1, S v2) {
            this.v1 = v1;
            this.v2 = v2;
        }
    }

    public CommandParser() {}

    private void error(String msg) {
        throw new IllegalArgumentException(msg);
    }

    private void complain(String cond) {
        error("\"" + cond + "\" is not a valid Command format");
    }

    public Command parse(String s) {
        String[] parts = s.split(",");

        if (parts.length != 3) {
            complain(s);
        }

        for (int i = 0; i < parts.length; i++) {
            parts[i] = parts[i].trim();
        }

        int commandType = Integer.parseInt(parts[0]);
        int parameter = Integer.parseInt(parts[1]);
        Condition condition = parseCondition(parts[2]);

        Command cmd = new Command(commandType, parameter, condition);

        return cmd;
    }

    private Condition parseCondition(String s) {
        Pair<Condition, Integer> ret = parseCondition(s, 0);

        return ret.v1;
    }

    private Pair<Condition, Integer> parseCondition(String s, int index) {
        char c = s.charAt(index);
        if (c != '(') {
            return parseSimpleCondition(s, index);
        } else {
            return parseCompoundCondition(s, index);
        }
    }

    private Pair<Condition, Integer> parseSimpleCondition(String s, int index) {
        Pair<Integer, Integer> var = parseVar(s, index);
        if (var.v2 == -1) {
            error("missing operator");
        }
        Pair<Integer, Integer> operator = parseOperator(s, var.v2);
        if (operator.v2 == -1) {
            error("missing parameter");
        }
        Pair<Integer, Integer> parameter = parseParameter(s, operator.v2);
        Condition cond = new Condition(commandType(var.v1, operator.v1), parameter.v1);
        return new Pair<Condition, Integer>(cond, skipWhitespaces(s, parameter.v2));
    }

    private int commandType(int var, int op) {
        return var | op | Condition.OTHER_TYPE;
    }

    private Pair<Integer, Integer> parseVar(String s, int index) {
        String subs = s.substring(index);
        if (subs.startsWith("dist")) {
            return parseWord(s, index, "dict", Condition.DICT_BITS);
        } else if (subs.startsWith("gyro")) {
            return parseWord(s, index, "gyro", Condition.GYRO_BITS);
        } else if (subs.startsWith("speed")) {
            return parseWord(s, index, "speed", Condition.SPEED_BITS);
        } else {
            error("variable was expected, but got " + subs);
            /* NOT REACHED */
            return null;
        }
    }

    private Pair<Integer, Integer> parseOperator(String s, int index) {
        String subs = s.substring(index);
        if (subs.startsWith("==")) {
            return parseWord(s, index, "==", Condition.EQ_BITS);
        } else if (subs.startsWith("!=")) {
            return parseWord(s, index, "!=", Condition.NE_BITS);
        } else if (subs.startsWith(">=")) {
            return parseWord(s, index, ">=", Condition.GE_BITS);
        } else if (subs.startsWith("<=")) {
            return parseWord(s, index, "<=" , Condition.LE_BITS);
        } else if (subs.startsWith(">")) {
            return parseWord(s, index, ">", Condition.GT_BITS);
        } else if (subs.startsWith("<")) {
            return parseWord(s, index, "<", Condition.LT_BITS);
        } else {
            error("operator was expected, but got " + subs);
            /* NOT REACHED */
            return null;
        }
    }

    private Pair<Integer, Integer> parseWord(String s, int index, String word, int returnVal) {
        return new Pair<Integer, Integer>(returnVal, skipWhitespaces(s, index+word.length()));
    }

    private Pair<Integer, Integer> parseParameter(String s, int index) {
        boolean negative = false;

        if (s.charAt(index) == '-') {
            negative = true;
            index++;
        }

        int i;
        boolean found = false;
        for (i = index; i < s.length() && Character.isDigit(s.charAt(i)); i++) {
            found = true;
        }
        if (!found) {
            error("missing parameter");
        }
        int val = Integer.parseInt(s.substring(index, i));
        if (negative) {
            val = -val;
        }
        return new Pair<Integer, Integer>(val, skipWhitespaces(s, i));
    }

    private Pair<Condition, Integer> parseCompoundCondition(String s, int index) {
        assert s.charAt(index) == '(';

        int nextIndex = skipWhitespaces(s, index+1);
        Pair<Condition, Integer> subCond = parseCondition(s, nextIndex);
        if (subCond.v2 == -1) {
            error("missing \')\'");
        } else if (s.charAt(subCond.v2) != ')') {
            error("\')\' was expected, but got \'" + s.charAt(subCond.v2) + "\'");
        }

        nextIndex = skipWhitespaces(s, subCond.v2+1);
        if (nextIndex == -1) {
            return new Pair<Condition, Integer>(subCond.v1, -1);
        } else if (s.charAt(nextIndex) == '&') {
            if (s.charAt(nextIndex+1) != '&') {
                error("\"&&\" was expected, but got \"&" + s.charAt(nextIndex+1) + "\"");
            }
            nextIndex = skipWhitespaces(s, nextIndex+2);
            if (nextIndex == -1) {
                error("missing right hand side expression of &&");
            }
            Pair<Condition, Integer> right = parseCondition(s, nextIndex);
            Condition cond = new Condition(Condition.AND_TYPE, subCond.v1, right.v1);
            return new Pair<Condition, Integer>(cond, right.v2);
        } else if (s.charAt(nextIndex) == '|') {
            if (s.charAt(nextIndex+1) != '|') {
                error("\"||\" was expected, bug got \"|" + s.charAt(nextIndex+1) + "\"");
            }
            nextIndex = skipWhitespaces(s, nextIndex+2);
            if (nextIndex == -1) {
                error("missing right hand side expression of &&");
            }
            Pair<Condition, Integer> right = parseCondition(s, nextIndex);
            Condition cond = new Condition(Condition.OR_TYPE, subCond.v1, right.v1);
            return new Pair<Condition, Integer>(cond, right.v2);
        }
        return new Pair<Condition, Integer>(subCond.v1, nextIndex);
    }

    private int skipWhitespaces(String s, int index) {
        if (index == -1) {
            return -1;
        }

        int i;
        for (i = index; i < s.length() && s.charAt(i) == ' '; i++) {
        }

        if (i == s.length()) {
            return -1;
        }
        return i;
    }

    public static void main(String[] args) {
        try {
            CommandParser parser = new CommandParser();
            Command cmd;

            cmd = parser.parse("1, 10, dist >= 30");
            cmd.print();

            cmd = parser.parse("2, 30, gyro <= 20");
            cmd.print();

            cmd = parser.parse("3, 40, (dist > 30) && (gyro == 0)");
            cmd.print();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
