import java.net.ServerSocket;
import java.net.Socket;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.Random;

public class SelectiveRepeatServer {
    public static void main(String[] args)
            throws Exception {
        ServerSocket server = new ServerSocket(6263);
        System.out.println("Server established");
        Socket client = server.accept();
        ObjectOutputStream oos = new ObjectOutputStream(client.getOutputStream());
        ObjectInputStream ois = new ObjectInputStream(client.getInputStream());
        System.out.println("Client is now connected.");
        System.out.println();
        
        // reading the mod used for dropping the frame
        int mod2 = (Integer) ois.readObject();
        // reading filesize for characters
        int filesize = (Integer) ois.readObject();
        char[] finaldata = new char[filesize];
        int counter = 0;
        // read the window size
        int x = (Integer) ois.readObject();
        // k is used for getting frame number
        int k = (Integer) ois.readObject();
        int j = 0;
        // i is used for getting data of frame
        char i = (Character) ois.readObject();
        // flag is used to know if Acknowledge is already dropped
        // then dont drop it later
        boolean flag = true;
boolean[] buffered = new boolean[2 * x];
        for (int t = 0; t < 2 * x; t++) {
            buffered[t] = false;
        }
        Random r = new Random(10);
        int mod = r.nextInt(10);
        while (mod == 1 || mod == 0)
            mod = r.nextInt(10);

        while (true) {
            int c = j;
            for (int h = 0; h < x; h++) {
                System.out.print("|" + c );
                c = (c + 1) % (2 * x);
            }
            System.out.print("|");
            System.out.println();
            System.out.println();
            // if current frame no j and recieved frame no k is same
            // then Acknowledge is sent
            if (k == j) {
                if (buffered[j]) {
                    System.out.println("Frame " + k + " recieved" + "\n" + "Data:" + i);
                    System.out.println("Duplicate Frame");
                    oos.writeObject(k);
                    System.out.println("Acknowledgement sent");
                    counter++;
                    buffered[j] = false;
                } else {
                    System.out.println("Frame " + k + " recieved" + "\n" + "Data:" + i);
                    oos.writeObject(k);
                    System.out.println("Acknowledgement sent");
                    finaldata[counter] = i;
                    counter++;
                }
                j = (j + 1) % (2 * x);
            } else {
                // otherwise there is mismatch so Acknowledge is not sent
                // if mismatch then buffer the data
                System.out.println("Error" + "\n" + " Expected frame: " + j + "\n"
                        + " Recieved frame: " + k);
                System.out.println(" Frame " + k + " is buffered");
                if (counter < k)
                    finaldata[k] = i;
                else
                    finaldata[counter + (k % (2 * x)) + 1] = i;
                buffered[k] = true;
                System.out.println("Error. Acknowledgement not sent.");
                flag = !flag; 
                if (j % mod2 != 0) {
                    flag = !flag;
                    // if j is 0 then sent back the previous frame i.e x otherwise sent j-- frame
                    if (j == 0)
                        j = 2 * x - 1;
                    else
                        j--;
                }
            }

            System.out.println();
            if (j % mod == 0)
                flag = !flag;
            k = (Integer) ois.readObject();
            if (k == -1)
                break;
            i = (Character) ois.readObject();
        }
        System.out.println();
        System.out.print("Message received: ");
        for (i = 0; i < filesize; i++) {
            System.out.print(finaldata[i]);
        }
        System.out.println();
        oos.writeObject(-1);
    }
}
