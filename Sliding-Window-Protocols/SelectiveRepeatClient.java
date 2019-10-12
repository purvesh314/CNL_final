import java.net.Socket;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.BufferedReader;
import java.util.Random;
import java.util.Scanner;
import java.io.File;

public class SelectiveRepeatClient {
    public static void main(String[] args) throws Exception {
        Socket client = new Socket("localhost", 6263);
        ObjectInputStream ois = new ObjectInputStream(client.getInputStream());
        ObjectOutputStream oos = new ObjectOutputStream(client.getOutputStream());
        System.out.println("Connected with server.");
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        
        System.out.print("Enter message to send: ");
        String sentence = br.readLine();
        char[] data = sentence.toCharArray();
        System.out.println();
        System.out.print("Enter the value of m : ");
        int m = Integer.parseInt(br.readLine());
        int x = (int) (Math.pow(2, m - 1));

        // counter is for how much data is sent, it will be used to terminate the
        // sending process
        int counter = 0;

        // h is used to identify the frame in each window
        int h = 0;
        Random r = new Random(10);
        int mod = r.nextInt(10);
        while (mod == 1 || mod == 0) // if mod is 1 or 0 then choose next random number
            mod = r.nextInt(10);
        System.out.println();
        oos.writeObject(mod); 
        oos.writeObject(data.length); 
        boolean flag = false; 

        // this is used to know whether the frame is already dropped or not
        // if the frame is dropped, then frame wont be dropped another time
        boolean anotherflag[] = new boolean[2 * x + 1];
        for (int i = 0; i < 2 * x + 1; i++) {
            anotherflag[i] = true;
        }

        // getting the start location of current frame of the window
        int strt = 0;
        h = 0;
        oos.writeObject(x);

        // this is the listner object(Thread) for getting the Acknowledgement
        SelectiveRepeatListener listener = new SelectiveRepeatListener(ois, x, counter);
        listener.t.start();

        // starting the sending process
        do {
            h = strt % (2 * x);
            for (int i = 0; i < x && counter + i < data.length; i++) {
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
                h = (h + 1) % (2 * x);
                Thread.sleep(2000);
            }
			listener.t.join(5000);
            if (listener.reply != x - 1 || listener.reply != x && !flag) {
                if (listener.reply == -2) {
                    System.out.println("Timeout for frame "+(listener.reply + 2) % (2 * x)+". No acknowlegdment was received. Resending frame");
                    anotherflag[listener.reply + 2] = !anotherflag[listener.reply + 2];
                    strt = listener.reply + 2;
                } else if (listener.reply + 1 < h) {
                    System.out.println("Timeout for frame "+(listener.reply + 1) % (2 * x)+". No acknowlegdment was received. Resending frame");
                    anotherflag[listener.reply + 1] = !anotherflag[listener.reply + 1];
                    strt = listener.reply + 1;
                } else {
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

class SelectiveRepeatListener implements Runnable {
    Thread t;
    ObjectInputStream ois;
    int reply, x, counter;

    SelectiveRepeatListener(ObjectInputStream o, int i, int counter) {
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
                reply = (Integer) ois.readObject();
                if (temp == (2 * x) - 1)
                    temp = 0;
                if (reply != -1 && reply != temp + 1)
                    reply = temp;
                if (reply != -1) {
                    temp = reply;
                    Thread.sleep(1000);
                    System.out.println("Acknowledgement of frame number " + (reply % (2 * x)) + " recieved.");
                    counter++;
                    System.out.println();
                }
            }
            reply = temp;
        } catch (Exception e) {
            System.out.println("Exception => " + e);
        }
    }
}
