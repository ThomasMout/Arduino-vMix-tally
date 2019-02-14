class Flake {
  public:
    float x,y;
    float vx,vy;

   void reset() {
     y = 0;
     x = random(8000)/1000.0;
     vx = random(-1000, 1000) / 4000.0;
      vy = random(500, 1000) / 2000.0;
   }
    
    public:
    Flake() {
      reset();
    }
    
    void loop() {
      x = x + vx;
      y = y + vy;
      if ( x >=8 ) x = 0;
      if ( x < 0 ) x = 7;
      if ( y >= 8 ) {
        reset();
      }
    }

  
};

