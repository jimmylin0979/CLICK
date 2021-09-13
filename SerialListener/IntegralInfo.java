import java.lang.StringBuilder;

class IntegralInfo {
    
    // Attributes
    private long timeStamp;
    private float value;

    // Constructor, also be a SET
    public IntegralInfo() {
        this.timeStamp = 0;
        this.value = 0;
    }
    public IntegralInfo(long timeStamp, float value) {
        this.timeStamp = timeStamp;
        this.value = value;
    }
    
    // SET
    public void setValue(float value) {this.value = value;}

    // GET
    public long getTimeStamp() {return this.timeStamp;}
    public float getValue() {return this.value;}

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("[\"timeStamp\": ");
        sb.append(this.timeStamp);
        sb.append(", \"Value\": ");
        sb.append(this.value);
        sb.append("]");

        return sb.toString();
    }
}
