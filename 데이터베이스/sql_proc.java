package database_subject;
import java.sql.*;

public class sql_proc {

	public static void sqlproc(String str) {

		Connection conn = null;
		Statement stmt = null;
		ResultSet rs = null;
		try{

			//Register JDBC driver
			Class.forName("com.mysql.jdbc.Driver");

			//Open a connection
			System.out.println("Connecting to database...");
			conn = DriverManager.getConnection("jdbc:mysql://localhost/mydb?useUnicode=true&useJDBCCompliantTimezoneShift=true&useLegacyDatetimeCode=false&serverTimezone=UTC", "root", "fkdldjs1");

			//Execute a query
			stmt = conn.createStatement();
			//rs = stmt.executeQuery("SELECT * FROM instructor");
			//rs = stmt.executeQuery("SELECT name FROM instructor WHERE dept_name='Comp. Sci.' and salary>70000");
			//rs = stmt.executeQuery("SELECT name, course_id FROM instructor as i, teaches as t where i.ID=t.ID and i.dept_name='Biology'");
			//stmt.executeUpdate("update instructor set salary = salary*1.05");
			//stmt.executeUpdate("insert into course values('CS-637','Database System','Comp. Sci', 4)");
			rs = stmt.executeQuery(str);
			//Print results
            while(rs.next()) {
            	//System.out.println(rs.getString("dept_name") + "|" + rs.getString(2) + "|" + rs.getString(3) + "|" + rs.getDouble("salary"));
            	System.out.println(rs.getString(1) + "|" + rs.getString(2) + "|" + rs.getString(3) + "|" + rs.getDouble(4));
            	//System.out.println(rs.getString(1)+ "|" + rs.getString(2));

            }


            // close a connection
            stmt.close();
            conn.close();

		}catch (SQLException ex) {
			//Handle errors for JDBC
			ex.printStackTrace();
		} catch (Exception e){
		    //Handle errors for Class.forName
			e.printStackTrace();
		}
	}
}