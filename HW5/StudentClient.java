
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class StudentClient {
    private int id;
    private int port;

    public StudentClient(int id, int port) {
        this.id = id;
        this.port = port;
    }

    public void start() throws IOException{
        try {
            Socket socket = new Socket("localhost", port);

            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

            out.println(id); //skicka eget ID till server
            String partnerID = in.readLine(); //ta emot partnerID (väntande)

            System.out.println("Student " + id + " partnered with: Student " + partnerID);

            socket.close();

        } catch (Exception e) {
        }
    }

    public static void main(String[] args) throws IOException {
        int id = Integer.parseInt(args[0]);
        int port = 5001;

        StudentClient client = new StudentClient(id, port);
        client.start();
    }
}
