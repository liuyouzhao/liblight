import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.URL;
import javax.imageio.ImageIO;

public class RequestProcessorCore implements Runnable {

	/**
	 * Socket connected to client passed by Proxy server
	 */
	Socket clientSocket;

	/**
	 * Read data client sends to proxy
	 */
	BufferedReader proxyToClientBr;

	/**
	 * Send data from proxy to client
	 */
	BufferedWriter proxyToClientBw;
	

	/**
	 * Thread that is used to transmit data read from client to server when using HTTPS
	 * Reference to this is required so it can be closed once completed.
	 */
	private Thread httpsClientToServer;


	/**
	 * Creates a ReuqestHandler object capable of servicing HTTP(S) GET requests
	 * @param clientSocket socket connected to the client
	 */
	public RequestProcessorCore(Socket clientSocket){
		this.clientSocket = clientSocket;
		try{
			this.clientSocket.setSoTimeout(10000);
			proxyToClientBr = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
			proxyToClientBw = new BufferedWriter(new OutputStreamWriter(clientSocket.getOutputStream()));
		} 
		catch (IOException e) {
			e.printStackTrace();
		}
	}


	
	/**
	 * Reads and examines the requestString and calls the appropriate method based 
	 * on the request type. 
	 */
	@Override
	public void run() {

		// Get Request from client
		String requestString;
		try{
			requestString = proxyToClientBr.readLine();
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("Error reading request from client");
			return;
		}

		if(requestString == null) {
			return;
		}

		// Parse out URL

		System.out.println("Request Received " + requestString);
		// Get the Request type
		String request = requestString.substring(0,requestString.indexOf(' '));

		// remove request type and space
		String urlString = requestString.substring(requestString.indexOf(' ')+1);

		// Remove everything past next space
		urlString = urlString.substring(0, urlString.indexOf(' '));

		// Prepend http:// if necessary to create correct URL
		if(!urlString.substring(0,4).equals("http")){
			String temp = "http://";
			urlString = temp + urlString;
		}


		// Check request type
		if(request.equals("CONNECT")){
			System.out.println("HTTPS Request for : " + urlString + "\n");
			handleHTTPSRequest(urlString);
		}
		else if(request.equals("GET")) {
			System.out.println("HTTP GET Request for : " + urlString + "\n");
			doGetTransferRequest(urlString);
		}
		else{
			System.out.println("[POST] HTTP " + request + " Request for : " + urlString + "\n");
			doPostPutTransfer(requestString, urlString);
		}
	} 


	private void doPostPutTransfer(String firstLine, String urlString) {

		String url = urlString.substring(7);
		int slashIndex = url.length();
		for(int i = 0; i < url.length(); i ++) {
			if(url.charAt(i) == '/') {
				slashIndex = i;
			}
		}
		url = url.substring(0, slashIndex);
		int port = 80;

		try{
			// Get actual IP associated with this URL through DNS
			InetAddress address = InetAddress.getByName(url);

			// Open a socket to the remote server
			System.out.println("Socket connection for " + address + " port " + port);
			Socket proxyToServerSocket = new Socket(address, port);
			proxyToServerSocket.setSoTimeout(30000);

			String header;
			while(true) {
				header = proxyToClientBr.readLine();

				if(header.length() == 0) {
					break;
				}
				String []keyValue = header.split(":");
				System.out.println(keyValue[0] + " : " + keyValue[1]);
			}

			// Send Connection established to the client

			// Client and Remote will both start sending data to proxy at this point
			// Proxy needs to asynchronously read data from each party and send it to the other party


			//Create a Buffered Writer betwen proxy and remote
			BufferedWriter proxyToServerBW = new BufferedWriter(new OutputStreamWriter(proxyToServerSocket.getOutputStream()));

			// Create Buffered Reader from proxy and remote
			BufferedReader proxyToServerBR = new BufferedReader(new InputStreamReader(proxyToServerSocket.getInputStream()));



			// Create a new thread to listen to client and transmit to server
			HttpTransferer clientToServerHttps =
					new HttpTransferer(clientSocket.getInputStream(), proxyToServerSocket.getOutputStream());
			clientToServerHttps.setFirstLine(firstLine);

			httpsClientToServer = new Thread(clientToServerHttps);
			httpsClientToServer.start();


			// Listen to remote server and relay to client
			try {
				byte[] buffer = new byte[4096];
				int read;
				do {
					read = proxyToServerSocket.getInputStream().read(buffer);
					if (read > 0) {
						System.out.println("[RECV]" + buffer.toString());
						clientSocket.getOutputStream().write(buffer, 0, read);
						if (proxyToServerSocket.getInputStream().available() < 1) {
							clientSocket.getOutputStream().flush();
						}
					}
				} while (read >= 0);
			}
			catch (SocketTimeoutException e) {
				e.printStackTrace();
				System.out.println("Issue URL: " + urlString);
			}
			catch (IOException e) {
				e.printStackTrace();
				System.out.println("Issue URL: " + urlString);
			}


			// Close Down Resources
			if(proxyToServerSocket != null){
				proxyToServerSocket.close();
			}

			if(proxyToServerBR != null){
				proxyToServerBR.close();
			}

			if(proxyToServerBW != null){
				proxyToServerBW.close();
			}

			if(proxyToClientBw != null){
				proxyToClientBw.close();
			}


		} catch (SocketTimeoutException e) {
			String line = "HTTP/1.0 504 Timeout Occured after 10s\n" +
					"User-Agent: ProxyServer/1.0\n" +
					"\r\n";
			try{
				proxyToClientBw.write(line);
				proxyToClientBw.flush();
			} catch (IOException ioe) {
				ioe.printStackTrace();
			}
		}
		catch (Exception e){
			System.out.println("Error on HTTPS : " + urlString );
			e.printStackTrace();
		}
	}

	private void doGetTransferRequest(String urlString){

		try{

			URL remoteURL = new URL(urlString);
			// Create a connection to remote server
			HttpURLConnection proxyToServerCon = (HttpURLConnection)remoteURL.openConnection();

			// Create the URL
			proxyToServerCon.setDoOutput(true);


			StringBuffer stringBuffer = new StringBuffer();
			int i = 0;
		    while(true) {
				String key = proxyToServerCon.getHeaderFieldKey(i);
		    	String responseHeader = proxyToServerCon.getHeaderField(i ++);
		    	if(responseHeader == null) {
		    		break;
				}
		    	if(key == null || key.equals("null")) {
					stringBuffer.append(responseHeader);
					stringBuffer.append("\n");
				}
		    	else if(Config.instance().filterKeys.get(key) != null) {
					stringBuffer.append(key + ":" + responseHeader);
					stringBuffer.append("\n");
				}
			}

			// Create Buffered Reader from remote Server
			BufferedReader proxyToServerBR = new BufferedReader(new InputStreamReader(proxyToServerCon.getInputStream()));

			// Send success code to client
			stringBuffer.append("Proxy-agent: ProxyServer/1.0\n\r\n");
			System.out.println(stringBuffer.toString());
			proxyToClientBw.write(stringBuffer.toString());

			if(ImageDownloader.tryDownloadAsImage(urlString, proxyToClientBw, clientSocket)) {
				System.out.println("Download as Image OK!");
				return;
			}


			// Read from input stream between proxy and remote server
			try {
				int len = 0;
				do {
					char[] buffer = new char[2048];
					len = proxyToServerBR.read(buffer);
					if(len > 0) {
						proxyToClientBw.write(buffer, 0, len);
					}
				} while (len >= 0);
			} catch(Exception ex) {
				ex.printStackTrace();
				if(clientSocket.isOutputShutdown()) {
					System.out.println("Socket is closed unexpectedly");
				}
			}
//			proxyToClientBw.flush();
			//String line;
//			while((line = proxyToServerBR.readLine()) != null){
//				// Send on data to client
//				proxyToClientBw.write(line);
//			}
			proxyToClientBw.flush();

			// Ensure all data is sent by this point


			// Close Down Resources
			if(proxyToServerBR != null){
				proxyToServerBR.close();
			}

			if(proxyToClientBw != null){
				proxyToClientBw.close();
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	
	/**
	 * Handles HTTPS requests between client and remote server
	 * @param urlString desired file to be transmitted over https
	 */
	private void handleHTTPSRequest(String urlString){
		// Extract the URL and port of remote 
		String url = urlString.substring(7);
		String pieces[] = url.split(":");
		url = pieces[0];
		int port  = Integer.valueOf(pieces[1]);

		try{
			// Only first line of HTTPS request has been read at this point (CONNECT *)
			// Read (and throw away) the rest of the initial data on the stream
			for(int i = 0; i < 5; i ++) {
				System.out.println(proxyToClientBr.readLine());
			}

			// Get actual IP associated with this URL through DNS
			InetAddress address = InetAddress.getByName(url);
			
			// Open a socket to the remote server 
			Socket proxyToServerSocket = new Socket(address, port);
			proxyToServerSocket.setSoTimeout(30000);

			// Send Connection established to the client
			String line = "HTTP/1.0 200 Connection established\r\n" +
					"Proxy-Agent: ProxyServer/1.0\r\n" +
					"\r\n";
			proxyToClientBw.write(line);
			proxyToClientBw.flush();

			// Client and Remote will both start sending data to proxy at this point
			// Proxy needs to asynchronously read data from each party and send it to the other party


			//Create a Buffered Writer betwen proxy and remote
			BufferedWriter proxyToServerBW = new BufferedWriter(new OutputStreamWriter(proxyToServerSocket.getOutputStream()));

			// Create Buffered Reader from proxy and remote
			BufferedReader proxyToServerBR = new BufferedReader(new InputStreamReader(proxyToServerSocket.getInputStream()));



			// Create a new thread to listen to client and transmit to server
			ClientToServerHttpsTransmit clientToServerHttps = 
					new ClientToServerHttpsTransmit(clientSocket.getInputStream(), proxyToServerSocket.getOutputStream());
			
			httpsClientToServer = new Thread(clientToServerHttps);
			httpsClientToServer.start();
			
			
			// Listen to remote server and relay to client
			try {
				byte[] buffer = new byte[4096];
				int read;
				do {
					read = proxyToServerSocket.getInputStream().read(buffer);
					if (read > 0) {
						clientSocket.getOutputStream().write(buffer, 0, read);
						if (proxyToServerSocket.getInputStream().available() < 1) {
							clientSocket.getOutputStream().flush();
						}
					}
				} while (read >= 0);
			}
			catch (SocketTimeoutException e) {
				e.printStackTrace();
				System.out.println("Issue URL: " + urlString);
			}
			catch (IOException e) {
				e.printStackTrace();
				System.out.println("Issue URL: " + urlString);
			}


			// Close Down Resources
			if(proxyToServerSocket != null){
				proxyToServerSocket.close();
			}

			if(proxyToServerBR != null){
				proxyToServerBR.close();
			}

			if(proxyToServerBW != null){
				proxyToServerBW.close();
			}

			if(proxyToClientBw != null){
				proxyToClientBw.close();
			}
			
			
		} catch (SocketTimeoutException e) {
			String line = "HTTP/1.0 504 Timeout Occured after 10s\n" +
					"User-Agent: ProxyServer/1.0\n" +
					"\r\n";
			try{
				proxyToClientBw.write(line);
				proxyToClientBw.flush();
			} catch (IOException ioe) {
				ioe.printStackTrace();
			}
		} 
		catch (Exception e){
			System.out.println("Error on HTTPS : " + urlString );
			e.printStackTrace();
		}
	}
}




