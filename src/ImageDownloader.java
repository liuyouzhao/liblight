import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.BufferedWriter;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.Socket;
import java.net.URL;


public class ImageDownloader {

    public static boolean tryDownloadAsImage(String urlString, BufferedWriter proxyToClientBw, Socket clientSocket) throws IOException {

        // This allows the files on stored on disk to resemble that of the URL it was taken from
        int fileExtensionIndex = urlString.lastIndexOf(".");
        String fileExtension;

        // Get the type of file
        fileExtension = urlString.substring(fileExtensionIndex);

        // Trailing / result in index.html of that directory being fetched
        if(fileExtension.contains("/")){
            fileExtension = fileExtension.replace("/", "__");
            fileExtension = fileExtension.replace('.','_');
            fileExtension += ".html";
        }

        if((fileExtension.contains(".png")) || fileExtension.contains(".jpg") ||
            fileExtension.contains(".jpeg") || fileExtension.contains(".gif")) {
            // Create the URL
            URL remoteURL = new URL(urlString);
            BufferedImage image = ImageIO.read(remoteURL);

            if(image != null) {

                // Send response code to client
                String line = "HTTP/1.0 200 OK\n" +
                        "Proxy-agent: ProxyServer/1.0\n" +
                        "\r\n";
                proxyToClientBw.write(line);
                proxyToClientBw.flush();

                // Send them the image data
                ImageIO.write(image, fileExtension.substring(1), clientSocket.getOutputStream());

                // No image received from remote server
            } else {
                System.out.println("Sending 404 to client as image wasn't received from server "
                        + urlString);
                String error = "HTTP/1.0 404 NOT FOUND\n" +
                        "Proxy-agent: ProxyServer/1.0\n" +
                        "\r\n";
                proxyToClientBw.write(error);
                proxyToClientBw.flush();
            }
            return true;
        }
        else {
            return false;
        }
    }

}
