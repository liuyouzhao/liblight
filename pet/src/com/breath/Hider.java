package com.breath;

import java.util.ArrayList;

public class Hider {

    private static final String VERBOSE = "whofollowccpwilldie";

    public static void conseal(String content, String pub1, String priv2, String picture) {
        ArrayList<String> arrayList = RSA.encode(pub1, content);

        String target = "";
        for(int i = 0; i < arrayList.size(); i ++) {
            target += arrayList.get(i);
        }

        Stegraph.encode(picture, target, VERBOSE, priv2);
    }

    public static String reveal(String priv1, String priv2, String picture) {
        String tail = "";
        for(int i = picture.length() - 1; i >= 0; i --) {
            if(picture.charAt(i) == '.') {
                tail = picture.substring(i);
                picture = picture.substring(0, i) + "-converted" + tail;
                break;
            }
        }

        String c2 = Stegraph.decode(picture, VERBOSE, priv2);
        String rt = RSA.decode(c2, priv1);

        return rt;
    }
}
