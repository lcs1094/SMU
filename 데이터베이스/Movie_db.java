package database_subject;
// import java.sql.Connection;
// import java.sql.DriverManager;
// import java.sql.ResultSet;
// import java.sql.SQLException;
// import java.sql.Statement;
import java.sql.*;
import java.io.*;
import java.util.Scanner;

public class Movie_db {
	private static boolean isNum = false;
	public static void prints(){
		System.out.println("========================================");
			System.out.println("(0) ����");
			System.out.println("(1) �����̼� ���� �� ������ ����");
			System.out.println("(2) ������ �̿��� �˻�");
			System.out.println("(3) �������� �̿��� �˻�");
			System.out.println("(4) �������� �̿��� �˻�");
			System.out.println("========================================");
			System.out.print("���ϴ� ��ȣ�� �Է� �Ͻÿ� : ");
	}

	public static int strToInt(String str){
		int num=-1;
		try {
			num = Integer.parseInt(str);
		} catch(NumberFormatException e) {
			isNum = false;
			System.out.println("\n�߸��� �Է��Դϴ�.\n");
		}
		isNum = true;
		return num;
	}

	public static void main(String[] args) {

		String input;
		String sql;
		String sqlinput = "";
		int num = 0;
		int sqlnum;
		Scanner sc = new Scanner(System.in);
		Connection conn = null;
		Statement stmt = null;
		ResultSet rs = null;
		try{
			Class.forName("com.mysql.jdbc.Driver");
			System.out.println("Connecting to database...");
			conn = DriverManager.getConnection("jdbc:mysql://localhost/mydb?useUnicode=true&useJDBCCompliantTimezoneShift=true&useLegacyDatetimeCode=false&serverTimezone=UTC", "root", "fkdldjs1");
			stmt = conn.createStatement();
			
			do {
				prints();
				input = sc.nextLine();
				num = strToInt(input);
				if(isNum) {
					if(num>4 || num < 0)
						System.out.println("\n�߸��� �Է��Դϴ�.\n");
					switch(num) {
						case 0:
							System.out.println("���α׷��� �����մϴ�.\n");
							break;
						case 1:
							sql = "DROP table movie";
							stmt.executeUpdate(sql);
							sql = "create table movie ("
									+ "	id           char(3), "
									+ "	title        varchar (100), "
									+ "	company      varchar (50),"
									+ "	releasedate  date,"
									+ "	country      varchar (10)," 
									+ "	totalscreen  int,"
									+ "	profit       numeric (15,2),"
									+ "	totalnum     int,"
									+ "	grade        varchar (50),"
									+ "	primary key (id))";
							stmt.executeUpdate(sql);
							try {	
						   		// ���� ��ü ����
								File file = new File("C:\\movie_data.txt");
						   		// �Է� ��Ʈ�� ����
						   		FileReader filereader = new FileReader(file);
						   		// �Է� ���� ����
								BufferedReader bufReader = new BufferedReader(filereader);
								String line = "";
						   		while ((line = bufReader.readLine()) != null) {
									String[] keys = line.split("\\|");
									sql = "INSERT INTO movie values ("+"'"+keys[1]+"'"+","+"'"+keys[2]+"'"+","+"'"+keys[3]+"'"+","+"'"+keys[4]+"'"+","+"'"+keys[5]+"'"+","+keys[6]+","+keys[7]+","+keys[8]+","+"'"+keys[9]+"'"+")";
									stmt.executeUpdate(sql);
						   		}
						   		// .readLine()�� ���� ���๮�ڸ� ���� �ʴ´�.
				  
						   		bufReader.close();
				  
							} catch (FileNotFoundException e) {
							}
							catch (IOException e) {
						   		System.out.println(e);
							}
							break;
						case 2:
							System.out.print("����� �Է� : ");
							sqlinput = sc.nextLine();
							sql = "SELECT * FROM movie where title like '%"+sqlinput+"%'";
							rs = stmt.executeQuery(sql);
							while(rs.next()) {
								System.out.println(rs.getString(1) + "|" + rs.getString(2) + "|" + rs.getString(3) + 
										"|" + rs.getDate(4) + "|" + rs.getString(5) + "|" + rs.getInt(6) + "|" + 
										rs.getString(7) + "|" + rs.getInt(8) + "|" + rs.getString(9));
							}
							break;
						case 3:
							System.out.print("����� �Է� : ");
							sqlinput = sc.nextLine();
							sqlnum = strToInt(sqlinput);
							sql = "SELECT * FROM movie where totalnum > "+sqlnum+" ";
							rs = stmt.executeQuery(sql);
							while(rs.next()) {
								System.out.println(rs.getString(1) + "|" + rs.getString(2) + "|" + rs.getString(3) + 
										"|" + rs.getDate(4) + "|" + rs.getString(5) + "|" + rs.getInt(6) + "|" + 
										rs.getString(7) + "|" + rs.getInt(8) + "|" + rs.getString(9));
							}
							break;
						case 4:
							System.out.print("����� �Է� : ");
							sqlinput = sc.nextLine();
							String[] dates = sqlinput.split(",");
							//java.sql.Date d = java.sql.Date.valueOf(dates[0]);
							//java.sql.Date d2 = java.sql.Date.valueOf(dates[1]);
							sql = "SELECT * FROM movie where releasedate BETWEEN "+"'"+dates[0]+"'"+" and "+"'"+dates[1]+"'"+" ";
							rs = stmt.executeQuery(sql);
							while(rs.next()) {
								System.out.println(rs.getString(1) + "|" + rs.getString(2) + "|" + rs.getString(3) + 
										"|" + rs.getDate(4) + "|" + rs.getString(5) + "|" + rs.getInt(6) + "|" + 
										rs.getString(7) + "|" + rs.getInt(8) + "|" + rs.getString(9));
							}
							break;
						}
				}
			}while(num!=0);
			stmt.close();
	    	conn.close();
			sc.close();
		}catch (SQLException ex) {
			ex.printStackTrace();
		} catch (Exception e){
			e.printStackTrace();
		}
	}
}
