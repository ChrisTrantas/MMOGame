class Ship extends Unit
{
  //controls
  Boolean thrusting = false;
  Boolean bearingLeft = false;
  Boolean bearingRight = false;
  
  //teleport delay
  int tpDelay = 0;
  
  //light modifiers
  float lightWidth = 250;
  float lightDepth = 768;
  float lightPower = 1; //1 = on 0 = off with gradient support
  float lightDecay = 0.001; //rate at which light decays
  float lightDelay = frameRate * 3; //how long it takes for the light to fade (if not shooting asteroids)
  float lightTimer = lightDelay; //the light fade timer
  
  //life management
  int respawnTimer = 0;
  int respawnDelay = 80;
  boolean alive = true;
  
  //invincibility management
  int invincibilityTimer = 0;
  int invincibilityDelay = 240;
  boolean invincible = false;
  
  Ship(PShape sprite)
  {
    super(sprite);
  }
  
  void update(boolean darkness)
  {
    if(respawnTimer == 0 && alive == false) // stay dead until respawn, then become invincible
    {
      alive = true;
      invincible = true;
      invincibilityTimer = invincibilityDelay;
    }
    else
      respawnTimer--;
    
    if (alive)
    {
      if (invincibilityTimer == 0) //invincibility decay
        invincible = false;
      else
        invincibilityTimer--;
      //rotate from controls
      if (bearingRight)
        rotation += PI/56;
      if (bearingLeft)
        rotation -= PI/56;
      if (thrusting) //move in the direction of rotation
        accel = PVector.div(PVector.fromAngle(rotation),3.5);
      else
        accel.set(0,0); //stop accelerating if your not thrusting
        
      vel.mult(0.99); //always slow down at a decay rate of 1%
      
      if(tpDelay > 0) //manage tp delay
        tpDelay--;
        
      if (lightTimer > 0) //check light timer, and apply light ddecay if needed
        lightTimer --;
      else
        lightPower -= lightDecay;
      
      Draw(darkness); //draw
      super.update();
    }
  }
  
  void Draw(boolean darkness)
  {
    if (darkness) //if it is dark, draw the headlights
    {
      PVector beam = PVector.fromAngle(rotation); //vector direction of beam
      PVector beamEnd = new PVector(beam.y, beam.x * -1); //vector direction of end of beam
      PVector beamTip = new PVector(pos.x + beam.x * (radius * 0.5), pos.y + beam.y * (radius * 0.5)); //narrow tip position
      //the outmost corners of the beam
      PVector beamCornerA = new PVector(beamTip.x + beam.x * lightDepth + beamEnd.x * lightWidth, beamTip.y + beam.y * lightDepth + beamEnd.y * lightWidth);
      PVector beamCornerB = new PVector(beamTip.x + beam.x * lightDepth - beamEnd.x * lightWidth, beamTip.y + beam.y * lightDepth - beamEnd.y * lightWidth);
      beginShape(QUAD);//draw primary headlights
      fill(255 * lightPower);
      noStroke();
      vertex(beamTip.x, beamTip.y);
      fill(255 * invincibilityTimer / invincibilityDelay);
      vertex(beamCornerA.x, beamCornerA.y);
      vertex(beamCornerB.x, beamCornerB.y);
      fill(255 * lightPower);
      vertex(beamTip.x, beamTip.y);
      endShape();
      //wrap the corners, to check for light wrapping
      PVector beamCornerAWrap = beamCornerA.get();
      PVector beamCornerBWrap = beamCornerB.get();
      wrap(beamCornerAWrap,radius*2,false);
      wrap(beamCornerBWrap,radius*2,false);
      //if this corner is wrapped, draw the headlights offset to the wrap
      if (!beamCornerA.equals(beamCornerAWrap))
      {
        float xOff = beamCornerAWrap.x - beamCornerA.x;
        float yOff = beamCornerAWrap.y - beamCornerA.y;
        beginShape(QUAD);
        fill(255 * lightPower);
        noStroke();
        vertex(beamTip.x + xOff, beamTip.y + yOff);
        fill(255 * invincibilityTimer / invincibilityDelay);
        vertex(beamCornerAWrap.x, beamCornerAWrap.y);
        vertex(beamCornerB.x + xOff, beamCornerB.y + yOff);
        fill(255 * lightPower);
        vertex(beamTip.x + xOff, beamTip.y + yOff);
        endShape();
      }
      //same as above, but the other corner
      if (!beamCornerB.equals(beamCornerBWrap))
      {
        float xOff = beamCornerBWrap.x - beamCornerB.x;
        float yOff = beamCornerBWrap.y - beamCornerB.y;
        beginShape(QUAD);
        fill(255 * lightPower);
        noStroke();
        vertex(beamTip.x + xOff, beamTip.y + yOff);
        fill(255 * invincibilityTimer / invincibilityDelay);
        vertex(beamCornerA.x + xOff, beamCornerA.y + yOff);
        vertex(beamCornerBWrap.x, beamCornerBWrap.y);
        fill(255 * lightPower);
        vertex(beamTip.x + xOff, beamTip.y + yOff);
        endShape();
      }
      //draw with black fill white borders if its dark
      sprite.setFill(color(0));
      sprite.setStroke(color(255));
    }
    else
    {//else draw with white fill black borders if its not dark
      sprite.setFill(color(255));
      sprite.setStroke(color(0));
    }
    super.Draw();
  }
  //move to a random location within the window area
  void teleport()
  {
    if(tpDelay == 0)
    {
      pos.set(random(radius,width-radius),random(radius,height-radius));
      tpDelay = (int)(frameRate/3);
    }
  }
  //look for a dead bullet
  boolean shoot(ArrayList<Bullet> bullets)
  {
    for(int i = 0; i < bullets.size(); i++)
    {
      if(bullets.get(i).alive == false) //if found, fire it
      {
        PVector fireVec = PVector.fromAngle(rotation);
        bullets.get(i).fire(PVector.add(pos,PVector.mult(fireVec,radius)), fireVec, 12.75);
        return true; //and return true
      }
    }
    return false; //else return false
  }
  //kills the player and resets the ship
  void explode()
  {
    respawnTimer = respawnDelay;
    alive = false;
    vel.set(0,0);
    accel.set(0,0);
    pos.set(width/2,height/2);
    rotation = -PI / 2;
    lightPower = 1;
    lightTimer = lightDelay;
  }
}
