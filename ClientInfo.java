
import java.net.Socket;

public class ClientInfo {
    public final String id;
    public final Socket socket;

    public ClientInfo(String id, Socket socket){
        this.id = id;
        this.socket = socket;
    }
    
}
