import java.util.*;
import java.io.*;
import java.util.ArrayList;

public class client_java {
	final static int PORT_NUM = 8765;
	static ArrayList<String> commands = new ArrayList<String>();

	public static void main (String args[]) {
		/*
		 *	- Read in test file using Scanner
		 *	- Store line commands into arraylist
		 *	- open up server on PORT_NUM
		 *	- Iterate through ArrayList and send commands to server
		 *
		 * 
		 */
		String file_name = getFileName();
		ArrayList<String> commands = loadFile(file_name);
		for (int i = 0; i < commands.size(); i++) {
			System.out.println(commands.get(i));
		}
		
	}

	public static String getFileName() {
		Scanner reader = new Scanner(System.in);
		System.out.print("Please provide a local test file: ");
		String file_name = reader.nextLine();
		return file_name;
	}

	public static ArrayList<String> loadFile(String file_name) {
		File file = new File(file_name);
		try {
			Scanner reader = new Scanner(file);
			ArrayList<String> commands = new ArrayList<String>();
			while (reader.hasNextLine()) {
				String line = reader.nextLine();
				System.out.println(line);
				commands.add(line);
			}
		} catch (FileNotFoundException e) {}
		return commands;
	}

}	
