import java.util.*;

public class Plotter extends PApplet {
    // isNewDataComing floag
    boolean isNewDataComing = false;
    float curY, preY;
    // int xPos = 0;
    
    // history, in order to support dynamic plot resizing
    List<Float> Ys;
    Float maximal, minimal;
    
    // Windows Properties
    final int WIDTH = 500, HEIGHT = 500;
    String TITLE = "";
    
    // ======================================== //
    // Override function that inherits from PApplet
    
    public Plotter(String TITLE) {
        super();
        
        this.TITLE = TITLE;
    }
    
    public void settings() {
        size(WIDTH, HEIGHT);
    }
    
    public void setup() {
        //
        isNewDataComing = false;
        
        // let the screen to be resizeable 
        surface.setResizable(true);
        surface.setTitle(TITLE);
        
        // history initialization
        Ys = new ArrayList<Float>();
        Ys.add(preY);
        maximal = 0.0;
        minimal = 0.0;
    }
    
    public void draw() {
        if (!isNewDataComing)    return;
        else    isNewDataComing = false;
        
        // draw the line:
        plotOnCanvas();
    }
    
    // ======================================== //
    
    private void plotOnCanvas() {
        // // map the data to Canvas' height or width first
        maximal = (maximal < curY) ? curY : maximal;
        minimal = (minimal > curY) ? curY : minimal;
        
        // float cur = curY, pre = preY;
        
        // clear canvas
        background(255);
        
        // Plot
        int xPos = (Ys.size() > width) ? Ys.size() - width : 1;
        int xPos_onPlot = 0;
        for (; xPos < Ys.size(); xPos++) {
            // map the data to Canvas' height or width first
            float cur = map(Ys.get(xPos - 1), maximal, minimal, 0, HEIGHT);
            float pre = map(Ys.get(xPos), maximal, minimal, 0, HEIGHT);
            
            xPos_onPlot ++;
            
            // plot a point & line 
            stroke(255, 0, 0);
            strokeWeight(3);
            point(xPos_onPlot, cur);
            if (xPos != 0) {
                line(xPos_onPlot - 1, pre, xPos_onPlot, cur);
            }
            
            // // plot negative & positive
            // if (cur >= 0)    stroke(0, 255, 0);
            // else  stroke(255, 0, 0);
            // line(xPos, height / 2, xPos, cur);
        }
        
        // remove element, keep the element in list less than certain amount
        if (Ys.size() >= 1000)
            Ys.remove(0);
        
        // Text
        fill(0);
        textSize(30);
        text(curY, width - 150, 50);
        
        // plot Coordinate
        float xAxis = map(0, maximal, minimal, 0, HEIGHT);
        stroke(220, 220, 220);
        strokeWeight(3);
        // line(0, height / 2, width, height / 2);
        line(0, xAxis, width, xAxis);
        line(width / 2, 0, width / 2, height);
    }   
    
    public void setValue(float curY) {
        isNewDataComing = true;
        this.curY = curY;
        
        // append on history
        Ys.add(curY);
    }
    
}
