
#include <Arduboy.h>

Arduboy arduboy;

// =========================================================
// Config
// =========================================================

#define NORTH 1
#define SOUTH 2
#define EAST 3
#define WEST 4

typedef struct Point {
  int x;
  int y;
} Point;

/**
 * Represents a single section within a snake.
 */
typedef struct Section {
  unsigned int direction; // NORTH, SOUTH, EAST or WEST
  unsigned int length;    // number of blocks in this section
} Section;

/**
 * A snake is made up of one or more sections relative to a given 
 * "head" position.  For example, consider this snake:
 *
 *  OXXXX
 *      X
 *      X
 *      
 * This is made up of two sections: {{EAST,5},{SOUTH,2}}
 */
typedef struct Snake {
  /**
   * The head of the snake.  That is the point in the arena
   * where the snake starts from.
   */
  Point head;
  /**
   * The sections making up the snake.  Every snake has at 
   * least one section.
   */
  Section sections[10];
  /**
   * The number of sections in this snake.  Must be at least 
   * one or more.
   */
  unsigned int numberOfSections = 1;
} Snake;

Snake snake;

// =========================================================
// Setup
// =========================================================

void setup() {
  arduboy.start();
  arduboy.setFrameRate(30);
  // Configure the snake
  snake.head = {8,6};
  snake.numberOfSections = 1;
  snake.sections[0] = { EAST, 1 };
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
   Check whether any buttons are pressed which signals a change of direction.
*/
void updateDirection() {
  int oDirection = snake.sections[0].direction;
  int nDirection;
  //
  if (arduboy.pressed(LEFT_BUTTON)) {
    nDirection = EAST;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    nDirection = WEST;
  } else if (arduboy.pressed(UP_BUTTON)) {
    nDirection = SOUTH;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    nDirection = NORTH;
  }
  //
  if (oDirection != nDirection) {
    // Yes, there was a change of direction.
    // Therefore, push a new section onto the snake
    newSection(nDirection);
  }
}

/**
   Push a new head onto the snake.
*/
void newSection(unsigned int direction) {
  // First, move all existing points one
  // position down the array.
  for (int i = snake.numberOfSections; i > 0; i = i - 1) {
    snake.sections[i] = snake.sections[i - 1];
  }
  // Second, put in the new head
  snake.sections[0] = {direction,1};
  // Finally, update the size of the snake
  snake.numberOfSections = snake.numberOfSections + 1;
}

/**
   Move head of snake in current direction; trim tail as needed
*/
void moveSnake() {
  moveHead();
  trimTail();
}

void moveHead() {
  int direction = snake.sections[0].direction;
  switch (direction) {
    case NORTH:
      snake.head.y = snake.head.y + 1;
      break;
    case SOUTH:
      snake.head.y = snake.head.y - 1;
      break;
    case EAST:
      snake.head.x = snake.head.x - 1;
      break;
    case WEST:
      snake.head.x = snake.head.x + 1;
      break;
  }
  snake.sections[0].length = snake.sections[0].length + 1;
}

/**
   Trim the tail of the snake as it moves.  The snake is made up of one of or more
*/
void trimTail() {
  int last = snake.numberOfSections - 1;
  int len = snake.sections[last].length;
  if(len > 1) {
    snake.sections[last].length = len-1;
  } else {
    snake.numberOfSections = snake.numberOfSections - 1;
  }
}

void drawSnake() {
  // Check for change of direction
  //
  Point pos = snake.head;
  //
  for (int i = 0; i < snake.numberOfSections; ++i) {
    pos = drawSnakeSection(pos,snake.sections[i]);
  }
}

Point drawSnakeSection(Point from, Section section) {
   Point to = from;
   switch(section.direction) {
    case NORTH:
      to.y -= section.length;
      break;
    case SOUTH:
      to.y += section.length;
      break;
    case EAST:
      to.x += section.length;
      break;
    case WEST:    
      to.x -= section.length;
      break;
   }
   drawBlockSection(from,to);
   return to;
}

void drawBlockSection(Point from, Point to) {
  if (from.x != to.x) {
    // horizontal setion
    int s = min(from.x, to.x);
    int e = max(from.x, to.x);
    int y = from.y * 4;
    for (int i = s; i <= e; i = i + 1) {
      arduboy.fillRect(i * 4, y, 4, 4, WHITE);
    }
  } else {
    // vertical section
    int s = min(from.y, to.y);
    int e = max(from.y, to.y);
    int x = from.x * 4;
    for (int i = s; i <= e; i = i + 1) {
      arduboy.fillRect(x, i * 4, 4, 4, WHITE);
    }
  }
}

