import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.SocketTimeoutException;

public class HttpTransferer implements Runnable {

    InputStream proxyToClientIS;
    OutputStream proxyToServerOS;

    String firstLine = "";

    /**
     * Creates Object to Listen to Client and Transmit that data to the server
     * @param proxyToClientIS Stream that proxy uses to receive data from client
     * @param proxyToServerOS Stream that proxy uses to transmit data to remote server
     */
    public HttpTransferer(InputStream proxyToClientIS, OutputStream proxyToServerOS) {
        this.proxyToClientIS = proxyToClientIS;
        this.proxyToServerOS = proxyToServerOS;
    }

    public void setFirstLine(String fl) {
        firstLine = fl;
    }

    @Override
    public void run(){
        try {
            // Write first line to server
            proxyToServerOS.write(firstLine.getBytes(), 0, firstLine.length());
            System.out.println("[SEND]" + firstLine);

            // Read byte by byte from client and send directly to server
            byte[] buffer = new byte[4096];
            int read;
            do {
                read = proxyToClientIS.read(buffer);
                if (read > 0) {
                    proxyToServerOS.write(buffer, 0, read);
                    System.out.println("[SEND]" + buffer.toString());
                    if (proxyToClientIS.available() < 1) {
                        proxyToServerOS.flush();
                    }
                }
            } while (read >= 0);
        }
        catch (SocketTimeoutException ste) {
            // TODO: handle exception
        }
        catch (IOException e) {
            System.out.println("Proxy to client HTTP read timed out");
            e.printStackTrace();
        }
    }
}
