class Asteroid extends Unit
{
  int angDir; //the direction the asteroid rotates, -1 or 1
  
  Asteroid(PShape sprite)
  {
    super(sprite);
    angDir = Utilities.sign(random(-1,1)); //set angDir to -1 or 1 randomly
  }
  
  void update()
  {
    rotation += angDir * PI / (radius * scale * 3); //rotate at a rate relative to radius/scale
    super.update();
    Draw();
  }
  
  void explode(ArrayList<Asteroid> asteroids, ArrayList<PShape> sprites)
  {
    if (scale >= 0.4) //if the asteroid is not too small to split up
    {
      PVector spawnDir = PVector.fromAngle(rotation);
      spawnDir.normalize();
      for(int i = 0; i < 3; i++) //spawn 3 new asteroids
      {
        Asteroid ast = new Asteroid(sprites.get((int)random(0,asteroidSprites.size())));
        ast.scale = scale - 0.3; //with a smaller scale
        ast.pos = PVector.add(pos, PVector.mult(spawnDir,radius*scale/2.5)); //positioned radially
        ast.vel = PVector.mult(spawnDir.get(),1+(1-ast.scale) * 2.5); //and set to move out radially
        spawnDir.rotate(TWO_PI/3);
        asteroids.add(ast);
      }
    }
  }
}
