//audio import and decleration
import ddf.minim.*;
Minim minim;
AudioPlayer music;
AudioPlayer shipExplosion;
ArrayList<AudioPlayer> explosions = new ArrayList<AudioPlayer>();
ArrayList<AudioPlayer> pews = new ArrayList<AudioPlayer>();

//unit based declerations
Ship ship;
ArrayList<Asteroid> asteroids = new ArrayList<Asteroid>();
ArrayList<PShape> asteroidSprites = new ArrayList<PShape>();
ArrayList<PShape> explosionSprites = new ArrayList<PShape>();
int maxSprites = 16; //number of different sprites to be made
ArrayList<Bullet> bullets = new ArrayList<Bullet>();
int maxBullets = 8; //number of bullets to exist
int spawnTime = 0; //counter for asteroid spawning
int spawnDelay = 65; //time between asteroid spawns
float asteroidPopulation = 0; //current asteroid population
int maxAsteroidPopulation = 8; //max population for asteroids

boolean darkness = true; //the initial gamemode

int time = 0; //current time alive
int bestTime = 0; //best time alive
int startTime; //millis() where timer started
PFont font; //the font

void setup()
{
  minim = new Minim(this);//initiate minim, then load files
  music = minim.loadFile("audio/Anxiety.mp3");
  shipExplosion = minim.loadFile("audio/ShipExplode.mp3");
  shipExplosion.setGain(-10);
  for(int i = 1; i <= 3; i++)
  {
    explosions.add(minim.loadFile("audio/Exploding"+i+".mp3"));
    explosions.get(i-1).setGain(-5);
    pews.add(minim.loadFile("audio/Pew"+i+".mp3"));
    pews.get(i-1).setGain(-5);
  }
  
  Utilities.app = this;//initiate my Utilities class
  noCursor();//hide the cursor
  size(displayWidth, displayHeight, P2D);//set to fullscreen dimensions
  
  font = createFont("Rockwell Extra Bold",16,true);
  //generate the sprites and bullets
  for (int i = 0; i < maxSprites; i++)
  {
    asteroidSprites.add(Utilities.GenerateClosedLoopPerlin(random(10000), 100, 0.034, 100, 5, -1, 0));
    explosionSprites.add(Utilities.GenerateClosedLoopPerlin(random(10000), 200, 0.175, 85, 3, -1, 255));
  }
  for (int i = 0; i < maxBullets; i++)
  {
    fill(0);
    PShape s = createShape(ELLIPSE,-6,-6,12,12);
    bullets.add(new Bullet(s));
  }
  //sets up the ship and starts the music/timer
  ship = new Ship(makeShip());
  ship.pos.set(width/2,height/2);
  ship.rotation = -PI/2;
  music.loop();
  startTime = millis();
}

void draw()
{
  //background color control
  if (darkness)
    background(255 * ship.invincibilityTimer / ship.invincibilityDelay);
  else
    background(255);
  if (ship.alive == false)
    background(255);
  
  //spawn asteroids, update ship then bullets, then check bullet collisions, then update asteroids, then check ship collision
  spawnAsteroids();
  ship.update(darkness);
  for (int i = 0; i < bullets.size(); i++)
  {
    bullets.get(i).update();
    checkBulletCollision(bullets.get(i));
  }
  for (int i = 0; i < asteroids.size(); i++)
  {
    asteroids.get(i).update();
    checkShipCollision(asteroids.get(i));
  }
  drawTime(); //draw the HUD
}
//draws the HUD
void drawTime()
{
  if (ship.invincible || !ship.alive)//dont start counting until you are playing proper
    startTime = millis();
  time = millis() - startTime;//get time alive
  bestTime = max(bestTime,time);//check against best time
  textFont(font,36);//start fonting
  fill(168);//color the font nuetral gray, so it can always be seen
  //display the HUD
  text("BestTime: " + floor(bestTime/1000.0/60.0) + "m" + floor((bestTime/1000.0)%60) + "s" + bestTime%1000 + "ms", 15, 40);
  text("Time: " + floor(time/1000.0/60.0) + "m" + floor((time/1000.0)%60) + "s" + time%1000 + "ms", 15, 80);
}
//spawns asteroids based on population
void spawnAsteroids()
{
  //generates population count based on asteroid scale
  asteroidPopulation = 0;
  for(int i = 0; i < asteroids.size(); i++)
  {
    if (asteroids.get(i).scale < 1)
      asteroidPopulation += asteroids.get(i).scale / 1.5;
    else
      asteroidPopulation += asteroids.get(i).scale;
  }
  //if its time to spawn and population has not been met, spawn
  if(spawnTime == 0 && asteroidPopulation < maxAsteroidPopulation)
  {
    spawnTime = spawnDelay; //reset spawn timer
    Asteroid ast = new Asteroid(asteroidSprites.get((int)random(0,asteroidSprites.size()))); //new asteroid
    asteroids.add(ast);
    int scatter = (int)random(0,4); //randomly select what side of the screen to come from
    switch(scatter)
    {
      case 0:
        ast.pos.set(random(-1 * ast.radius, width + ast.radius), -1 * ast.radius);
        break;
      case 1:
        ast.pos.set(-1 * ast.radius, random(-1 * ast.radius, height + ast.radius));
        break;
      case 2:
        ast.pos.set(random(-1 * ast.radius, width + ast.radius), height + ast.radius);
        break;
      case 3:
        ast.pos.set(width + ast.radius, random(-1 * ast.radius, height + ast.radius));
        break;
    }
    ast.vel = PVector.sub(ship.pos, ast.pos); //head to the ship
    ast.vel.normalize();
  }
  else if (asteroidPopulation < maxAsteroidPopulation) //update the timer
    spawnTime--;
}
//check ship to asteroid collisions
void checkShipCollision(Asteroid ast)
{
  if(ship.invincible == false)
  {
    for (int i = 0; i < ship.sprite.getVertexCount(); i++) //loop through each vertex on the ship
    {
      PVector vertVec = ship.sprite.getVertex(i).get();
      vertVec.rotate(ship.rotation);
      vertVec.add(ship.pos); //make a window oriented vector of the vertex
      if (ast.pointHitTest(vertVec) && ship.alive) //pointHitTest the vertex with the asteroid given as ast
      {
        ship.explode(); //explode!
        playSound("shipExplosion");
        for(int j = 0; j < bullets.size(); j++) //reset all bullets
        {
          bullets.get(j).reset();
        }
        break;
      }
    }
  }
}
//check bullet-to-asteroid collisions
void checkBulletCollision(Bullet b)
{
  for (int i = 0; i < asteroids.size(); i++) //check each asteroid to the given bullet b
  {
    if (asteroids.get(i).pointHitTest(b.pos) && b.alive) //if asteroid hit
    {
      b.reset(); //reset the bullet
      asteroids.get(i).explode(asteroids, asteroidSprites); //explode the asteroid
      asteroids.remove(i); //remove it from the ArrayList
      i--; //double back
      playSound("explosion");
      ship.lightTimer = ship.lightDelay; //reset ship light decay timer
      if (ship.lightPower < 1) //power up the ship's headlights
      {
        ship.lightPower += ship.lightDecay * 25;
        if (ship.lightPower > 1)
          ship.lightPower = 1;
      }
      break;
    }
  }
}
//plays sounds in a condensed method manner, given sound "name"
void playSound(String s)
{
  int file = (int)random(0,3); //random sound play
  if (s == "pew")
  {
    pews.get(file).rewind();
    pews.get(file).play();
  }
  else if (s == "explosion")
  {
    explosions.get(file).rewind();
    explosions.get(file).play();
  }
  else if (s == "shipExplosion")
  {
    shipExplosion.rewind();
    shipExplosion.play();
  }
}
//key input
void keyPressed()
{
  if (key == CODED)
  {
    if (keyCode == LEFT)
      ship.bearingLeft = true;
    if (keyCode == RIGHT)
      ship.bearingRight = true;
    if (keyCode == UP)
      ship.thrusting = true;
    if (keyCode == DOWN)
      ship.teleport();
  }
  else
  {
    if (key == ' ' && ship.alive)
      if(ship.shoot(bullets)) //returns true if a bullet is succesfully shot
        playSound("pew");
    if (key == '=')
      darkness = !darkness;
  }
}
//key input
void keyReleased()
{
  if (key == CODED)
  {
    if (keyCode == LEFT)
      ship.bearingLeft = false;
    if (keyCode == RIGHT)
      ship.bearingRight = false;
    if (keyCode == UP)
      ship.thrusting = false;
  }
}
//generates the PShape for the ship, makes for neater code
PShape makeShip()
{
  PShape s = createShape();
  s.beginShape();
  s.stroke(255);
  s.fill(0);
  s.strokeWeight(4);
  s.strokeJoin(ROUND);
  s.vertex(35,0);
  s.vertex(-35,-20);
  s.vertex(-15,0);
  s.vertex(-35,20);
  s.endShape(CLOSE);
  return s;
}
//enables fullscreen
boolean sketchFullScreen()
{
  return true;
}
