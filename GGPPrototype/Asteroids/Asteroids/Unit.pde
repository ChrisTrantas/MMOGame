class Unit
{
  PVector pos = new PVector(-width,-height);
  PVector vel = new PVector(0,0);
  float maxSpeed = 10;
  PVector accel = new PVector(0,0);
  PShape sprite;
  float rotation = 0; //angle of rotation
  float scale = 1; //scale, where 1 is 100%
  float radius = 0; // the general radius of the PShape
  
  Unit(PShape sprite)
  {
    this.sprite = sprite;
    for(int i = 0; i < sprite.getVertexCount(); i++) //calculates the approximate radius of the PShape
      radius = max(radius, sprite.getVertex(i).mag());
  }
  
  void Draw()
  {//generic matrix based drawing
    pushMatrix();
    translate(pos.x, pos.y);
    rotate(rotation);
    scale(scale);
    shape(sprite);
    popMatrix();
  }
  
  void update()
  {
    rotation %= TWO_PI; //keep rotation in range of 0 to TWO_PI
    vel.add(accel); //physics
    vel.limit(maxSpeed);
    pos.add(vel);
    if(isOutOfScreen()) //wrap the unit if off screen
      wrap(pos, radius, true); //wrap the position, with girth of radius, absolute (no relative offsets)
  }
  //returns true if the unit is completely off screen
  boolean isOutOfScreen()
  {
    return (pos.x + radius * scale < 0 || pos.y + radius * scale < 0 || pos.x - radius * scale > width || pos.y - radius * scale > height);
  }
  //dynamic wrapping, with girth and absolute mode
  void wrap(PVector vec, float girth, boolean absolute)
  {
    if (absolute)
    {
      if (vec.x > width)
        vec.x = -1 * girth * scale;
      else if (vec.x < 0)
        vec.x = width + girth * scale;
      if (vec.y > height)
        vec.y = -1 * girth * scale;
      else if (vec.y < 0)
        vec.y = height + girth * scale;
    }
    else
    {
      if (vec.x > width)
        vec.x -= width + girth * scale;
      else if (vec.x < 0)
        vec.x += width + girth * scale;
      if (vec.y > height)
        vec.y -= height + girth * scale;
      else if (vec.y < 0)
        vec.y += height + girth * scale;
    }
  }
  //This is the collision detection algorithm that literally everything is using
  //1) define a vector of POINT to CENTER of the PShape
  //2) find the angle between said vector and the facing angle of the unit, getting the relative local angle of the vector
  //3) map the angle from 0,TWO_PI to 0,vertexCount-1, ie: 90deg = 25%, 25% of 80 vertices is 20
  //4) apply some logic to correct the vertex number (i had some math trouble with -180 to 180 values)
  //5) check if the magnitude of the vertex is greater than or equal to the length of the originally found vector
  //6) if true, the point is in the poly loop, else it is outside of it.
  //this has a big O notation of 1, this is something that I am proud of, since its always the same CPU footprint now matter
  //how many vertices are in the loop!
  boolean pointHitTest(PVector point)
  {
    if (radius * scale >= PVector.dist(pos,point))
    {
      float angle = atan2(point.y-pos.y,point.x-pos.x) - rotation;
      if (angle < 0)
        angle += TWO_PI;
      int vertex = floor(map(angle,0,TWO_PI,0,sprite.getVertexCount() - 1));
      if (vertex < 0)
        vertex += sprite.getVertexCount();
      vertex %= sprite.getVertexCount();
      return sprite.getVertex(vertex).mag() >= PVector.dist(pos,point);
    }
    else
      return false;
  }
}
