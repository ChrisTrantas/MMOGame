class Bullet extends Unit
{
  int life = 0; //the life of the bullet
  boolean alive = false; //if the bullet is alive
  
  Bullet(PShape sprite)
  {
    super(sprite);
    maxSpeed *= 2; //double the base max speed
  }
  
  void update()
  {
    life--; //die a slow death
    if (life == 0)
    {
      reset(); //reset upon death
    }
      
    if (alive) //update and draw if alive
    {
      super.update();
      Draw();
    }
  }
  
  void fire(PVector point, PVector dir, float speed) //brings the bullet back to life, giving it a position, direction, and speed
  {
    pos = point.get();
    vel = PVector.mult(dir.get(),speed);
    life = (int)(frameRate * 3.75);
    alive = true;
  }
  
  void reset() //moves/kills the bullet, binning it for later use
  {
    alive = false;
    life = 0;
    pos.set(-width,-height);
    vel.set(0,0);
    accel.set(0,0);
    rotation = 0;
  }
}
