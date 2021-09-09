public class LogManager {
    // Log Recording 
    PrintWriter printWriter;
    String filePath = "content.csv";
    
    public LogManager() {
        // ================================== //
        // Log file's Header : Columns' Name
        printWriter = createWriter(filePath);
        printWriter.println("time, A, V, D");
    }
    
    public void flush() {
        // flush the data into log file
        printWriter.flush();
    }
    
    public void close() {
        // and then close the printWriter
        printWriter.close();
    }
    
    public void writeLog(String log) {
        // log
        printWriter.println(log);
        printWriter.flush();
    }
}