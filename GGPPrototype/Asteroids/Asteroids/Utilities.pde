static class Utilities
{
  //allows use of PApplet methods and variables
  public static PApplet app;
  //generates a polyLoop of perlin noise, given many variable parameters
  static PShape GenerateClosedLoopPerlin(float seed, int resolution, float sampleSize, float radius, float weight, int fill, int stroke)
  {
    PShape temp = app.createShape();
    temp.beginShape();
    temp.stroke(stroke);
    if(fill != -1) //makes no fille of fill = -1
      temp.fill(fill);
    else
      temp.noFill();
    temp.strokeWeight(weight);
    temp.strokeJoin(ROUND);
    
    float initNoise = 1; //initNoise is the initial noise of the shape, used to close the shape seamlessly later on
    for (int i = 0; i < resolution; i++)
    {
      float ang = (float)i/resolution; //the percent of TWO_PI the angle is at
      float n = app.noise(seed + i * sampleSize); //the current noise step
      if (i == 0)
        initNoise = n; //sets initial noise
      if (ang <= 0.75) //proceed normally if less than or equal to 75% of the way done
        temp.vertex(cos(TWO_PI * ang) * n * radius, sin(TWO_PI * ang) * n * radius);
      else
      {
        float delimiterRange = (ang-0.75)/0.25; //the range at which the new noise can move freely
        float delimiter = map(n,0,1,initNoise*delimiterRange,1-(1 - initNoise)*delimiterRange); //the new noise is mapped with the delimiter
        temp.vertex(cos(TWO_PI * ang) * delimiter * radius, sin(TWO_PI * ang) * delimiter * radius); //modified noise is applied
      }
    }
    temp.endShape(CLOSE);
    return temp;
  }
  //simple sign method, I like using sign
  static int sign(float num)
  {
    if (num > 0)
      return 1;
    if (num < 0)
      return -1;
    return 0;
  }
}
