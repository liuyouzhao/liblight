package com.breath;

import javax.crypto.Cipher;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.StringReader;
import java.security.*;
import java.security.interfaces.RSAPrivateKey;
import java.security.interfaces.RSAPublicKey;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;
import java.util.ArrayList;
import java.util.Base64;

public class RSA {

    protected static final String KEY_FACTORY_ALGORITHM = "RSA";
    protected static final int CHUNK_SIZE = 32;

    static class RSAKeys {
        public String pubKey;
        public String privKey;
        public String encoded;
    }

    public static PrivateKey generatePrivateKey(String privKeyBase64String) {
        try {

            StringBuilder pkcs8Lines = new StringBuilder();
            BufferedReader rdr = new BufferedReader(new StringReader(privKeyBase64String));
            String line;
            while ((line = rdr.readLine()) != null) {
                pkcs8Lines.append(line);
            }
            String pkcs8Pem = pkcs8Lines.toString();
            pkcs8Pem = pkcs8Pem.replace("-----BEGIN PRIVATE KEY-----", "");
            pkcs8Pem = pkcs8Pem.replace("-----END PRIVATE KEY-----", "");
            pkcs8Pem = pkcs8Pem.replaceAll("\\s+", "");

            // Base64 decode the result

            byte[] pkcs8EncodedBytes = Base64.getDecoder().decode(pkcs8Pem);

            // extract the private key
            PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(pkcs8EncodedBytes);
            KeyFactory kf = KeyFactory.getInstance("RSA");
            PrivateKey privKey = kf.generatePrivate(keySpec);

            return privKey;

        }catch (NoSuchAlgorithmException e) {
            throw new RuntimeException(e);
        } catch (InvalidKeySpecException e) {
            throw new IllegalArgumentException(e);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    public static PublicKey generatePublicKey(String encodedPublicKey) {
        try {
            byte[] decodedKey = Base64.getDecoder().decode(encodedPublicKey);
            KeyFactory keyFactory = KeyFactory.getInstance(KEY_FACTORY_ALGORITHM);
            return keyFactory.generatePublic(new X509EncodedKeySpec(decodedKey));
        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException(e);
        } catch (InvalidKeySpecException e) {
            throw new IllegalArgumentException(e);
        }
    }

    public static String encodePub(String pubKeyString, String src) {

        try {
            // Base64 decode the result

            RSAPublicKey rsaPublicKey = (RSAPublicKey) generatePublicKey(pubKeyString);

            X509EncodedKeySpec x509EncodedKeySpec2 = new X509EncodedKeySpec(rsaPublicKey.getEncoded());
            KeyFactory keyFactory2 = KeyFactory.getInstance(KEY_FACTORY_ALGORITHM);
            PublicKey publicKey2 = keyFactory2.generatePublic(x509EncodedKeySpec2);
            Cipher cipher2 = Cipher.getInstance(KEY_FACTORY_ALGORITHM);
            cipher2.init(Cipher.ENCRYPT_MODE, publicKey2);
            byte[] result2 = cipher2.doFinal(src.getBytes());
            String rt = Base64.getEncoder().encodeToString(result2);
            return rt;

        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static String decodePriv(String privKeyString, String src) {

        try {
            byte[] byteContent = Base64.getDecoder().decode(src);
            RSAPrivateKey rsaPrivateKey = (RSAPrivateKey) generatePrivateKey(privKeyString);
            PKCS8EncodedKeySpec pkcs8EncodedKeySpec5 = new PKCS8EncodedKeySpec(rsaPrivateKey.getEncoded());
            KeyFactory keyFactory5 = null;
            keyFactory5 = KeyFactory.getInstance(KEY_FACTORY_ALGORITHM);
            PrivateKey privateKey5 = keyFactory5.generatePrivate(pkcs8EncodedKeySpec5);
            Cipher cipher5 = Cipher.getInstance(KEY_FACTORY_ALGORITHM);
            cipher5.init(Cipher.DECRYPT_MODE, privateKey5);
            byte[] result5 = cipher5.doFinal(byteContent);
            return new String(result5);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static String encodePriv(String privKeyString, String src) {
        try {
            RSAPrivateKey rsaPrivateKey = (RSAPrivateKey) generatePrivateKey(privKeyString);

            PKCS8EncodedKeySpec pkcs8EncodedKeySpec5 = new PKCS8EncodedKeySpec(rsaPrivateKey.getEncoded());
            KeyFactory keyFactory5 = KeyFactory.getInstance(KEY_FACTORY_ALGORITHM);
            PrivateKey privateKey5 = keyFactory5.generatePrivate(pkcs8EncodedKeySpec5);
            Cipher cipher5 = Cipher.getInstance(KEY_FACTORY_ALGORITHM);
            cipher5.init(Cipher.ENCRYPT_MODE, privateKey5);
            byte[] result5 = cipher5.doFinal(src.getBytes());
            String rt = Base64.getEncoder().encodeToString(result5);
            return rt;

        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public static RSAKeys generateKeyAndEncode(String src){
        try {
            RSAKeys rsaKeys = new RSAKeys();
            KeyPairGenerator keyPairGenerator = KeyPairGenerator.getInstance("RSA");
            keyPairGenerator.initialize(512);
            KeyPair keyPair = keyPairGenerator.generateKeyPair();
            RSAPublicKey rsaPublicKey = (RSAPublicKey) keyPair.getPublic();
            RSAPrivateKey rsaPrivateKey = (RSAPrivateKey) keyPair.getPrivate();

            rsaKeys.pubKey = Base64.getEncoder().encodeToString(rsaPublicKey.getEncoded());
            rsaKeys.privKey = Base64.getEncoder().encodeToString(rsaPrivateKey.getEncoded());


            X509EncodedKeySpec x509EncodedKeySpec2 = new X509EncodedKeySpec(rsaPublicKey.getEncoded());
            KeyFactory keyFactory2 = KeyFactory.getInstance("RSA");
            PublicKey publicKey2 = keyFactory2.generatePublic(x509EncodedKeySpec2);
            Cipher cipher2 = Cipher.getInstance("RSA");
            cipher2.init(Cipher.ENCRYPT_MODE, publicKey2);
            byte[] result2 = cipher2.doFinal(src.getBytes());
            rsaKeys.encoded = Base64.getEncoder().encodeToString(result2);


            PKCS8EncodedKeySpec pkcs8EncodedKeySpec5 = new PKCS8EncodedKeySpec(rsaPrivateKey.getEncoded());
            KeyFactory keyFactory5 = KeyFactory.getInstance("RSA");
            PrivateKey privateKey5 = keyFactory5.generatePrivate(pkcs8EncodedKeySpec5);
            Cipher cipher5 = Cipher.getInstance("RSA");
            cipher5.init(Cipher.DECRYPT_MODE, privateKey5);
            byte[] result5 = cipher5.doFinal(result2);

            return rsaKeys;

        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    public static ArrayList<String> encode(String pubKeyString, String src) {
        ArrayList<String> encodedList = new ArrayList<>();

        String target = src;
        String sub = null;
        boolean over = false;
        while(!over) {
            if(target.length() <= CHUNK_SIZE) {
                sub = target;
                String result = encodePub(pubKeyString, sub);
                encodedList.add(result);
                over = true;
            }
            else {
                sub = target.substring(0, CHUNK_SIZE);
                target = target.substring(CHUNK_SIZE);
                String result = encodePub(pubKeyString, sub);
                encodedList.add(result);
            }
        }
        return encodedList;
    }

    public static String decode(String longString, String privKeyString) {
        String[] str = longString.split("==");

        String result = "";
        for(int i = 0; i < str.length; i ++) {
            String line = str[i] + "==";
            String decodedLine = decodePriv(privKeyString, line);
            result += decodedLine;
        }

        return result;
    }

}
