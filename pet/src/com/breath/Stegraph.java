package com.breath;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.Buffer;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Date;

public class Stegraph {

    public static final int CONTENT_LENGTH = 512;

    private static BufferedImage img = null;

    public static boolean compareRects(ArrayList<String> a1, ArrayList<String> a2) {
        StringBuffer sb1 = new StringBuffer();
        for(int i = 0; i < a1.size(); i ++) {
            sb1.append(a1.get(i));
        }

        StringBuffer sb2 = new StringBuffer();
        for(int i = 0; i < a2.size(); i ++) {
            sb2.append(a2.get(i));
        }

        return sb1.toString().equals(sb2.toString());
    }

    /**
     * [1] Make array into a square array
     * @param origin
     * @return
     */
    public static ArrayList<String> makeRoughSquar(ArrayList<String> origin) {
        int length = (origin.size() - 1) * origin.get(0).length() + origin.get(origin.size() - 1).length();
        double d = Math.sqrt((double)length);
        long D = Math.round(d + 0.5);
        ArrayList<String> rt = new ArrayList<>();

        int i1, j1, i2, j2;
        i2 = j2 = 0;

        StringBuffer sb = new StringBuffer();

        for(i1 = 0; i1 < origin.size(); i1 ++) {

            for(j1 = 0; j1 < origin.get(i1).length(); j1 ++) {
                sb.append(origin.get(i1).charAt(j1));
                j2 ++;
                if(j2 >= D) {
                    rt.add(sb.toString());
                    j2 = 0;
                    i2 ++;
                    sb = new StringBuffer();
                }
            }
        }

        rt.add(sb.toString());

        return rt;
    }

    public static String genDistributionSeed(String verbose, String privKey) {
        String encodedSeed = RSA.encodePriv(privKey, verbose);
        return encodedSeed;
    }


    private static void genCodeMap(int w, int h, ArrayList<String> content) {
        BufferedImage img = new BufferedImage(w, h, BufferedImage.TYPE_3BYTE_BGR);

        DateFormat df1 = DateFormat.getDateInstance(DateFormat.SHORT, Locale.CHINA);
        String fileName = df1.format(new Date());
        try {
            ImageIO.write(img, "jpg", new File(fileName));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * 1 Byte:
     * XXYY YYZZ
     *
     * R: 1111 11XX
     * G: 1111 YYYY
     * B: 1111 11ZZ
     *
     * @param position
     * @param c
     */
    private static void insert(int position, char c) {

        int col = position % img.getWidth();
        int row = position / img.getWidth();

        int rgb = img.getRGB(col, row);
        Color color = new Color(rgb, true);
        int r = color.getRed();
        int g = color.getGreen();
        int b = color.getBlue();

        System.out.println((int)c);


        r = (r & 0xFC | ((c >> 6) & 0x3));
        g = (g & 0xF0 | ((c >> 2) & 0xF));
        b = (b & 0xFC | (c & 0x3));

        System.out.println(r + ", " + g + ", " + b);

        Color color1 = new Color(r, g, b);
        img.setRGB(col, row, color1.getRGB());
    }

    /**
     *      * 1 Byte:
     *      * XXYY YYZZ
     *      *
     *      * R: 1111 11XX
     *      * G: 1111 YYYY
     *      * B: 1111 11ZZ
     * @param position
     * @return
     */
    private static char query(int position) {
        int col = position % img.getWidth();
        int row = position / img.getWidth();

        int rgb = img.getRGB(col, row);
        Color color = new Color(rgb, true);
        int r = color.getRed();
        int g = color.getGreen();
        int b = color.getBlue();

        System.out.println(r + ", " + g + ", " + b);

        char c = (char) (((r & 0x3) << 6) | ((g & 0xF) << 2) | (b & 0x3));

        System.out.println((int)c);
        return c;
    }

    /**
     * By the sizes of both the image and the text with the text-index,
     * calculate the position on the image where the text[index] should be insert.
     * @return position to insert
     */
    private static int calculatePositionFromWHI(int size, int size1, int index, String seed) {
        final int scope = size / (size1) - 1;
        if(scope <= 0) {
            return index;
        }
        int disturb = seed.charAt(index % seed.length()) % scope;
        return index * (scope + 1) + disturb;
    }

    private static void doNext(String content, int toInsert, String seed) {
        int pos = calculatePositionFromWHI(img.getWidth() * img.getHeight(), CONTENT_LENGTH, toInsert, seed);
        insert(pos, content.charAt(toInsert));
    }

    private static char queryNext(int toExtract, String seed) {
        int pos = calculatePositionFromWHI(img.getWidth() * img.getHeight(), CONTENT_LENGTH, toExtract, seed);
        return query(pos);
    }

    public static void encode(String filepath, String content, String verbose, String privKey) {

        /**
         * Content to a fixed length
         */
        if(content.length() >= CONTENT_LENGTH) {
            System.out.println("Picture encode failed, content too long, the max is 512");
            return;
        }

        String content2 = content;
        for(int i = 0; i < (CONTENT_LENGTH - content.length()); i ++) {
            content2 += "=";
        }

        System.out.println(content2);
        /**
         * 1. Generate seed
         */
        String seed = genDistributionSeed(verbose, privKey);
        System.out.println("seed1:" + seed);


        try {
            img = ImageIO.read(new File(filepath));

            for(int i = 0; i < content2.length(); i ++) {
                doNext(content2, i, seed);
            }

            String outfile = filepath.replace(".jpg", "-converted.jpg");
            ImageIO.write(img, "jpg", new File(outfile));
        } catch (IOException e) {
            e.printStackTrace();
            System.out.println(e.toString());
        }
    }

    public static String decode(String filepath, String verbose, String privKey) {
        /**
         * 1. Generate seed
         */
        String seed = genDistributionSeed(verbose, privKey);
        System.out.println("seed2:" + seed);

        String content = "";

        BufferedImage img = null;
        try {
            img = ImageIO.read(new File(filepath));

            for(int i = 0; i < CONTENT_LENGTH; i ++) {
                char c = queryNext(i, seed);
                String cstr = String.valueOf(c);
                content = content + cstr;
            }
            return content;
        } catch (IOException e) {
            e.printStackTrace();
            System.out.println(e.toString());
        }

        return null;
    }
}
