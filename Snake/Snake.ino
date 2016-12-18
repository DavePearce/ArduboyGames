
#include <Arduboy.h>

Arduboy arduboy;

// =========================================================
// Config
// =========================================================

typedef struct Point {
  int x;
  int y;
} Point;

/**
 * Number of turns to be drawn.  Everytime a turn is made it is recorded into the queue.  
 * Turns are removed from the queue eventually
 */
unsigned int numberOfPoints = 3;

/**
 * A snake is made up from one or more "points".  The point at [0] identifies the head 
 * of the snake.  The remaining points are the internal joints for the snake.
 */
Point points[10] = {{8,4},{9,4},{9,6}};

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4
unsigned int direction = WEST;

// =========================================================
// Setup
// =========================================================

void setup() {  
    arduboy.start();
    arduboy.setFrameRate(30);
}

// =========================================================
// Loop
// =========================================================

void loop() {
  if (!(arduboy.nextFrame())) {
    // Not time for the next frame yet
    return;
  } else {
    // Time for the next frame!
    // Check for change of direction
    updateDirection();
    // Move snake in current direction
    moveSnake();
    // Draw snake
    arduboy.clearDisplay();
    drawSnake();
    arduboy.display();
    //
    delay(1000);
  }
}

/**
 * Check whether any buttons are pressed which signals a change of direction.
 */
void updateDirection() {
  int oDirection = direction;
  //
  if (arduboy.pressed(LEFT_BUTTON)) {
    direction = WEST;    
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    direction = EAST;
  } else if (arduboy.pressed(UP_BUTTON)) {
    direction = NORTH;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    direction = SOUTH;
  }
  //
  if(oDirection != direction) {
    // Yes, there was a change of direction
    for(int i=numberOfPoints;i>0;i=i-1) {
      points[i] = points[i-1];
    }
    points[0] = points[1];
    numberOfPoints = numberOfPoints+1;
  }
}

/**
 * Move head of snake in current direction; trim tail as needed
 */
void moveSnake() {
  moveHead();
}

void moveHead() {
  switch(direction) {
    case NORTH:
      points[0].y = points[0].y - 1;
      break;
    case SOUTH:
      points[0].y = points[0].y + 1;
      break;
    case EAST:
      points[0].x = points[0].x + 1;
      break;
    case WEST:
      points[0].x = points[0].x - 1;
      break;
  }
}

void drawSnake() {
  // Check for change of direction
  //
  for(int i=1;i<numberOfPoints;++i) {
    Point p1 = points[i-1];
    Point p2 = points[i];
    drawSnakeSection(p1,p2);
  }
}

void drawSnakeSection(Point from, Point to) {
  if(from.x != to.x) {
    // horizontal setion
    int s = min(from.x,to.x);
    int e = max(from.x,to.x);
    int y = from.y * 4;
    for(int i=s;i<=e;i=i+1) {
      arduboy.fillRect(i*4,y,4,4,WHITE);
    }
  } else {
    // vertical section
    int s = min(from.y,to.y);
    int e = max(from.y,to.y);
    int x = from.x * 4;
    for(int i=s;i<=e;i=i+1) {
      arduboy.fillRect(x,i*4,4,4,WHITE);
    }
  }
}

