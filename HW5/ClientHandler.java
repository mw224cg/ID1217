import java.net.*;

public class ClientHandler implements Runnable{
    
    private Socket socket;
    private TeacherServer server;
    
    public ClientHandler(Socket socket, TeacherServer server){
        this.socket = socket;
        this.server = server;
    }

    public void run(){
        server.handleStudent(socket);
    }
    
}
