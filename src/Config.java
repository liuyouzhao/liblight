import java.util.HashMap;

public class Config {

    public HashMap<String, Boolean> filterKeys;

    private static Config sConfig;

    private Config() {
        filterKeys = new HashMap<>();
        filterKeys.put("Set-Cookie", true);
        filterKeys.put("Content-Type", true);
    }

    public static Config instance() {
        if(sConfig == null) {
            sConfig = new Config();
        }
        return sConfig;
    }
}
