package com.breath;

public class MessupKey {

    /**
     * MIIBVgIBADANBgkqhkiG9w0BAQEFAASCAUAwggE8AgEAAkEAlDGA77tTZkvPoJdlLRU6oEbFQ9geOnjDwEYVYntbAeNnOFofzP3J90UCttzDgq4BfkwkxIEtytAaTsFZOX71KwIDAQABAkAk+klDCIoh727KkpxjaHWtgps8d5PrYTDvLE2vujoWOSb22PAqfbetIPxtDKRcViMJhJXENpJVB+bInVW91kSBAiEA3sE/+Wf7K9j8smQl4QeTq3f5AByDyUfaiff6yFuiKUECIQCqT39bXPMt+JNmeU1Nsx97BaBQ5jovxwGW5+Rh4mr3awIhAMntIuIi6WSBKelIsbHBHZMViYoZTznaQfhJ7HNN5b1BAiEAkyqk25cDT+snu/n/0a+B+XVQJff7ucNWXUc+s1LKqccCIQDBDXH0gbWrcwq85TvlIuMJz9Jlv9FIpc7JgafXHpvibg==

     __pt1__ == 77tT
     __pt2__ == Cttz
     __pt3__ == bg==

     IIBVgIBADANBgkqhkiG9w0BAQEFAASCAUAwggE8AgEAAkEAlDGA + (__p1__) +
     ZkvPoJdlLRU6oEbFQ9geOnjDwEYVYntbAeNnOFofzP3J90U + (__p2__) +
     Dgq4BfkwkxIEtytAaTsFZOX71KwIDAQABAkAk+klDCIoh727KkpxjaHWtgps8d5PrYTDvLE2vujoWOSb22PAqfbetIPxtDKRcViMJhJXENpJVB+bInVW91kSBAiEA3sE/+Wf7K9j8smQl4QeTq3f5AByDyUfaiff6yFuiKUECIQCqT39bXPMt+JNmeU1Nsx97BaBQ5jovxwGW5+Rh4mr3awIhAMntIuIi6WSBKelIsbHBHZMViYoZTznaQfhJ7HNN5b1BAiEAkyqk25cDT+snu/n/0a+B+XVQJff7ucNWXUc+s1LKqccCIQDBDXH0gbWrcwq85TvlIuMJz9Jlv9FIpc7JgafXHpvi
     + (__p3__)
     */
    /// l == 1, m == 4
    /// i == 0, 1, 2, 3
    /// h == 2, __h == 2
    /// 77tT
    ///
    public static String __p1__(int l, int m, int h, int __h) {
        StringBuffer rt = new StringBuffer();
        for(int i = 0; i < l * m - 1; i ++) {
            int r = (int) (Math.random() * Integer.MAX_VALUE) >> (int)(Math.random() * 24);

            r &= 1;
            if(r % 2 != 0) {
                r = r << __h >> 12;
            }
            if(rt.length() == 0) {
                if (r % 2 == 0) {
                    rt.append("" + ((((~r) ^ 0xfffffffe) << (h << __h) >> 5) - 1));
                }
            }
            else if(rt.length() < h) {
                rt.append(rt);
            }
            else {
                rt.append((char)(int)(h * (__h << 1) * 10 + m + m * 8));
                rt.append((char)(((int)rt.charAt(rt.length() - 1)) - (m - 1) * 10 - __h));
            }
        }
        return rt.toString();
    }

    /// __s1 == "GotoHellCCPNtmd!"
    /// __h1 == 5
    /// __h2 == 12
    /// Cttz
    public static String __p2__(String __s1, int __h1, int __h2) {
        String rt = "";
        int i = 1;
        while(i % (__h2 >> 2) != 0) {
            if(rt.length() == 0 && __s1.length() < 2) {
                __s1 += "AU";
                rt += __s1;
            }
            else if(__s1.length() <= 3) {
                StringBuffer sb = new StringBuffer();
                __s1 += sb.append((char)__h1 + "XQab5");
                rt += __s1;
            }
            else if(__s1.length() < 7) {
                StringBuffer sb = new StringBuffer();
                __s1 += sb.append((char)__h2 + "&e");
                rt += __s1;
            }
            else if(__s1.length() < 9) {
                StringBuffer sb = new StringBuffer();
                __s1 += sb.append((char)__h2 + "&pC");
                rt += __s1;
            }
            else if(__s1.length() < 12) {
                rt += "Ttt";
            }
            else {
                if(rt.length() == 0) {
                    rt += __s1.charAt((__h1 - 1) << 1);
                    i = __h1 - (__h2 >> 2) + 2;
                }
                else if(rt.charAt(rt.length() - 1) == 'U'){
                    rt += __s1.charAt((__h1 - 4) << 1) + 't';
                    i = __h1 - (__h2 >> 2) + 3;
                }
                else if(rt.charAt(rt.length() - 1) == '5') {
                    rt += __s1.charAt((__h1) << 1);
                    i = __h1 - (__h2 >> 2) + 5;
                }
                else if(rt.charAt(rt.length() - 1) == 'C'){
                    rt += __s1.charAt(__h2) + "" + __s1.charAt(__h1 - 3);
                    i = __h1 - (__h2 >> 2) + 8;
                }
                else if(rt.charAt(rt.length() - 1) == 't'){
                    rt += 'z';
                    i = __h1 - (__h2 >> 2) + 1;
                }
                else if(rt.charAt(rt.length() - 1) == 'e') {
                    rt += __s1.charAt((__h2 >> 3) << 1);
                }
                else if(rt.charAt(rt.length() - 1) == 'z') {
                    rt += 'C';
                }
            }
        }
        return rt;
    }

    /// a1 == 2
    /// a2 == 49
    /// bg==
    public static String __p3__(int a1, int a2) {
        return String.valueOf((char)(a1 * a2)) + "g==";
    }

    public static void messup(String privKey) {

    }
}
