package  com.breath;

import java.util.ArrayList;

public class ToolMain {
    public static final String src = "rsa test";

    public static void main(String[] args) {
        String filename = "/home/hujia/Pictures/solvent.jpg";
        Hider.conseal("Hello, my name is ADAM. What's your name?", Config.PUB_KEY_FOR_CONTENT, Config.PRIV_KEY_FOR_SIGN, filename);

        String result = Hider.reveal(Config.PRIV_KEY_FOR_CONTENT, Config.PRIV_KEY_FOR_SIGN, filename);

        System.out.println(result);
    }
}