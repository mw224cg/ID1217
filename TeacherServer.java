import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.*;
import java.util.LinkedList;
import java.util.Queue;

public class TeacherServer {
    private ServerSocket serverSocket;
    private final int classSize;
    private int connectedStudents = 0;

    private final Queue<ClientInfo> waitingQueue = new LinkedList<>();

    public TeacherServer(int port, int classSize)throws IOException{
        this.serverSocket = new ServerSocket(port);
        this.classSize = classSize;
        System.out.println("Server started at port: " + port);
        System.out.println("Class Size: " + classSize);
    }

    public void start() throws IOException{
        while (true) { 
            Socket socket = serverSocket.accept();
            Thread clientThread = new Thread(new ClientHandler(socket, this));
            clientThread.start();
        }
    }


    public void handleStudent(Socket socket){
        try {
            ClientInfo student = readStudent(socket); //Läs elevens id + socket som anslutit
            ClientInfo partner = pairStudent(student); //Para ihop med elev i kön

            if (partner == null){
                return;
            }

            sendPairInfo(student, partner);

            student.socket.close();
            partner.socket.close();

        } catch (Exception e) {
        }
    }

    /**
     * Läser studentens id från socket
     * @param socket
     * @return
     * @throws IOException
     */
    private ClientInfo readStudent(Socket socket) throws IOException{
        BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

        String id = in.readLine();
        System.out.println("Student " + id + " connected.");

        return new ClientInfo(id, socket);

    }

    /**
     * Returnerar en partner till en student
     * @param current
     * @return
     */
    private synchronized ClientInfo pairStudent(ClientInfo current){
            connectedStudents++;

            if(!waitingQueue.isEmpty()){ //finns elev i kö --> returnera
                return waitingQueue.poll();
            }

            waitingQueue.add(current); //annars lägg till i kön och returnera null

            if(connectedStudents == classSize){ //edgecase
                return waitingQueue.poll();
            }

            return null; 
    }

    /**
     * Skickar id till respektive elev i ett par
     * @param partner1
     * @param partner2
     * @throws IOException
     */
    private void sendPairInfo(ClientInfo partner1, ClientInfo partner2)throws IOException{
        if (partner2 == null){
            return;
        }

        PrintWriter out1 = new PrintWriter(partner1.socket.getOutputStream(),true);
        
        if (partner1 != partner2){
            PrintWriter out2 = new PrintWriter(partner2.socket.getOutputStream(), true);
            out1.println(partner2.id);
            out2.println(partner1.id);
        } else {
            out1.println(partner1.id);
        }

        System.out.println("Server: Student " + partner1.id + " paired with Student " + partner2.id);


    }

    public static void main(String[] args) throws IOException {
        TeacherServer server = new TeacherServer(5001, 5);
        server.start();
    }
}
