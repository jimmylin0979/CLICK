import java.util.*;

class Integral {

    // 測量維度，目前僅考慮 Z軸 1維
    final int DIMENSION = 1;
    
    public Integral() {

    }

    private float integral(IntegralInfo pre, IntegralInfo cur) {
        // DEBUG
        System.out.println("===========");
        System.out.println(pre);
        System.out.println(cur);

        float integralResult = 0;
        float timeInterval = cur.getTimeStamp() - pre.getTimeStamp();

        if ((cur.getValue() <= 0 && pre.getValue() <= 0) || (cur.getValue() >= 0 && pre.getValue() >= 0)) {
            // both acceleration is negative

            // get the delta Velocity, and plus the delta to preV to get curV
            integralResult = (cur.getValue() + pre.getValue()) * timeInterval / 2; 
        } else {
            // one acceleration is positive and the other one is negative
            // get the delta Velocity, and plus the delta to preV to get curV
            float absCurA = abs(cur.getValue()), absPreA = abs(pre.getValue());
            // QUSETION: 可能溢位
            float preTimeInterval = (absPreA / (absCurA + absPreA)) * timeInterval;
            float curTimeInterval = timeInterval - preTimeInterval;

            integralResult = preTimeInterval * pre.getValue() + curTimeInterval * cur.getValue();
            integralResult = integralResult / 2;
        }

        // DEBUG
        System.out.printf("timeInterval = %f, integralResult = %f\n", timeInterval, integralResult);

        return integralResult;
    }
    
    private float abs(float num) {
        if(num < 0)  return -num;
        return num;
    }
 
    /**
    * 
    * A = Acceleration
    * V = Velocity
    * D = Displacement
    */
    public float getDeltaV(IntegralInfo preA, IntegralInfo curA) {
        // 
        
        // get deltaV as Velocity change
        float deltaV = integral(preA, curA);
        return deltaV;
    }

    public float getDeltaD(IntegralInfo preV, IntegralInfo curV) {
        //

        // get deltaD as Displacement
        float deltaD = integral(preV, curV);
        return deltaD;
    }

    public List<IntegralInfo> getCurVD(IntegralInfo curA, IntegralInfo preA, IntegralInfo preV, IntegralInfo preD) {
        // 
        List<IntegralInfo> res = new ArrayList<IntegralInfo>();

        // Delat can be negative or positive
        // Current Velocity = Previous Velocity + delta Velocity
        float curV_Value = getDeltaV(preA, curA) + preV.getValue();
        // System.out.println(curV_Value);
        // System.out.println(preV);
        IntegralInfo curV = new IntegralInfo(curA.getTimeStamp(), curV_Value);

        System.out.println("curV : " + curV);

        // Current Displacement = Previous Displacement + delta Displacement
        float curD_Value = getDeltaD(preV, curV) + preD.getValue();
        // System.out.println(curD_Value);
        // System.out.println("============");
        IntegralInfo curD = new IntegralInfo(curV.getTimeStamp(), curD_Value);
        
        System.out.println("curD : " + curD);

        // [0] current Velocity
        // [1] current Displacement
        res.add(curV);
        res.add(curD);
        return res;
    }
}
