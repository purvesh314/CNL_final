
/*
 * Go-Back-N Client Program
 */

import java.net.Socket;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.BufferedReader;
import java.util.Random;
import java.util.Scanner;
import java.io.File;

public class GoBackNClient {
    public static void main(String[] args) throws Exception {
    	// creating client socket for communication
        Socket client = new Socket("localhost", 6263);
        ObjectInputStream ois = new ObjectInputStream(client.getInputStream());
        ObjectOutputStream oos = new ObjectOutputStream(client.getOutputStream());
        System.out.println("Connected with server.");

        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        
        System.out.print("Enter the message to send: ");
        String sentence = br.readLine();
        char[] data = sentence.toCharArray();
        /*System.out.print("Input to be sent: ");
        for (int t = 0; t < data.length; t++) {
            System.out.print(data[t]);
        }*/
        System.out.println();

        // m is the number of bits in which each frame is represented, this will decide
        // window size
        System.out.print("Enter the value of m : ");
        int m = Integer.parseInt(br.readLine());

        // x is the window size
        int x = (int) ((Math.pow(2, m)) - 1);

        // counter is for how much data is sent, it will be used to terminate the
        // sending process
        int counter = 0;

        // h is used to identify the frame in each window
        int h = 0;

        // r is used to randomly generate a number (mod) which will be used to randomly
        // drop the data packets
        Random r = new Random(10);
        int mod = r.nextInt(10);
        while (mod == 1 || mod == 0) // if mod is 1 or 0 then choose next random number
            mod = r.nextInt(10);

        System.out.println();

        oos.writeObject(mod); // sending the random number generated for dropping packets
        oos.writeObject(data.length); // writing the size of file (no of character) to be read by client
        boolean flag = false; // it is used only to indicate all data is sent

        // this is used to know whether the frame is already dropped or not
        // if the frame is dropped, then frame wont be dropped another time
        boolean anotherflag[] = new boolean[x + 1];
        for (int i = 0; i < x + 1; i++) {
            anotherflag[i] = true;
        }

        // this is the listner object(Thread) for getting the Acknowledgement
        GoBackNListener listener = new GoBackNListener(ois, x, counter);
        listener.t.start();
        // getting the start location of current frame of the window
        int strt = 0;
        h = 0;

        oos.writeObject(x);

        // starting the sending process
        do {
            h = strt % (x + 1);
            // sending the entire window frame by frame
            for (int i = 0; i < x && counter + i < data.length; i++) {
                // logic to randomly drop the frame
                if (i % mod == 0 && anotherflag[i]) {
                    System.out.println("Sending frame:" + h);
                    System.out.println("Error...");
                    System.out.println();
                } else {
                    System.out.println("Sending frame:" + h);
                    System.out.println("Waiting for Acknowledgement ...");
                    System.out.println();
                    oos.writeObject(h);
                    oos.writeObject(data[counter + i]);
                }
                h = (h + 1) % (x + 1);
                Thread.sleep(2000);
            }

            // if the reply is not equal then timeout after 5000 ms, i.e. waiting for 5000ms
            // for Acknowledgement
            listener.t.join(5000);
            // if we dont got Acknowledgement in that time, its error. Again change the
            // start wrt given reply by server
            if (listener.reply != x - 1 || listener.reply != x && !flag) {
                // if the 0th frame is dropped then change pointer by 2 else change it by 1
                if (listener.reply == -2) {
                    System.out.println("Timeout for frame "+(listener.reply + 2) % (2 * x)+". No acknowlegdment was received. Resending frame");
                    // as packet is already not received then change the flag
                    anotherflag[listener.reply + 2] = !anotherflag[listener.reply + 2];
                    strt = listener.reply + 2;
                } else {
                    System.out.println("Timeout for frame"+(listener.reply + 1) % (2 * x)+"No acknowlegdment was received. Resending frame");
                    // as packet is already not received then change the flag
                    anotherflag[listener.reply + 1] = !anotherflag[listener.reply + 1];
                    strt = listener.reply + 1;
                }
                System.out.println();
                flag = false;
            }
            if (counter < data.length) {
                // change the counter wrt counter of listener based on no of frames Acknowledged
                counter = listener.counter;
            }
            if (counter == data.length) {
                // if complete then change flag
                flag = true;
            }
        } while (!flag);
        System.out.println("Message sent successfully.");
        oos.writeObject(-1);
    }
}

class GoBackNListener implements Runnable {
    Thread t;
    ObjectInputStream ois;
    int reply, x, counter;

    GoBackNListener(ObjectInputStream o, int i, int counter) {
        t = new Thread(this);
        ois = o;
        reply = -2;
        x = i;
        this.counter = counter;
    }

    @Override
    public void run() {
        try {
            int temp = 0;
            while (reply != -1) {
                // reading reply from server
                reply = (Integer) ois.readObject();
                if (temp == x)
                    temp = 0;
                if (reply != -1 && reply != temp + 1)
                    reply = temp;
                // if reply is not -1, then Acknowledgement is received
                if (reply != -1) {
                    temp = reply;
                    Thread.sleep(1000);
                    System.out.println("Acknowledgement of frame no " + (reply % (x + 1)) + " recieved.");
                    counter++;
                    System.out.println();
                }
            }
        } catch (Exception e) {
            System.out.println("Exception => " + e);
        }
    }
}
