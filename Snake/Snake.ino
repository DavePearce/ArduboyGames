
#include <Arduboy.h>

Arduboy arduboy;

// =========================================================
// Config
// =========================================================

typedef enum Direction { 
  NORTH, 
  SOUTH, 
  EAST,
  WEST 
} Direction;

typedef struct Point {
  int x;
  int y;
} Point;

/**
 * Represents a single section within a snake.
 */
typedef struct Section {
  Direction direction; // NORTH, SOUTH, EAST or WEST
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
  resetGame();
}

void resetGame() {
  // Configure the snake
  snake.head = {8,6};
  snake.numberOfSections = 1;
  snake.sections[0] = { EAST, 5 };
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
    // Check for self collision
    if(isTouchingSelf()) {
      gameOver();
    }
    //
    delay(1000);
  }
}

void gameOver() {
  arduboy.clearDisplay();
  resetGame();
  delay(1000);
}

/**
 * Check whether or not the snake is touching itself.
 */
bool isTouchingSelf() {
  Point head = snake.head;
  Point p = head;
  for(int i=0;i!=snake.numberOfSections;i++) {
    Section section = snake.sections[i];
    // Ignore first section, since the head always
    // intersects this!
    if(i != 0 && isPointInSection(head,p,section)) {
      return true;
    } else {
      p = getEndPoint(p,section);
    }
  }
  return false;
}

bool isPointInSection(Point p, Point from, Section section) {
  Point to = getEndPoint(from,section);
  if(p.x == from.x && p.x == to.x) {
    int s = min(from.y, to.y);
    int e = max(from.y, to.y);
    return s <= p.y && p.y <= e;
  } else if(p.y == from.y && p.y == to.y) {
    int s = min(from.x, to.x);
    int e = max(from.x, to.x);
    return s <= p.x && p.x <= e;
  } else {
    return false;
  }
}

/**
 *  Check whether any buttons are pressed which signals a change of direction.
 */
void updateDirection() {
  Direction oDirection = getCurrentDirection();
  Direction nDirection;
  //
  if (arduboy.pressed(LEFT_BUTTON)) {
    nDirection = WEST;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    nDirection = EAST;
  } else if (arduboy.pressed(UP_BUTTON)) {
    nDirection = NORTH;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    nDirection = SOUTH;
  } else {
    return;
  }
  //
  if (oDirection != nDirection) {
    // Yes, there was a change of direction.
    // Therefore, push a new section onto the snake
    newSection(nDirection);
  }
}

/**
 * Push a new section onto the snake.
 */
void newSection(Direction direction) {
  // First, move all existing points one
  // position down the array.
  for (int i = snake.numberOfSections; i > 0; i = i - 1) {
    snake.sections[i] = snake.sections[i - 1];
  }
  // Second, put in the new head
  snake.sections[0] = {direction,0};
  // Finally, update the size of the snake
  snake.numberOfSections = snake.numberOfSections + 1;
}

/**
   Move head of snake in current direction; trim tail as needed
*/
void moveSnake() {
  // First, move the head
  snake.head = movePoint(getCurrentDirection(),snake.head);
  // Second, update the first and last sections
  Section *first = firstSection();
  Section *last = lastSection();
  lengthenSection(first);  
  trimSection(last);
  if(last->length == 0) {
    // The last section of the snake has now reduced to have zero 
    // length, in which case it can be dropped altogether.
    removeLastSection();
  }
}

Direction getCurrentDirection() {
  return snake.sections[0].direction;
}

/**
 * Move a given point in a given direction
 */
Point movePoint(Direction direction, Point p) {
  switch (direction) {
    case NORTH:
      p.y -= 1;
      break;
    case SOUTH:
      p.y += 1;
      break;
    case EAST:
      p.x += 1;
      break;
    case WEST:
      p.x -= 1;
      break;
  }
  //
  return p;
}

/**
 * Access the first section of the snake
 */
Section *firstSection() {
  return &snake.sections[0];
}

/**
 * Access the last section of the snake
 */
Section *lastSection() {
  return &snake.sections[snake.numberOfSections-1];
}

/**
 * Increase the length of a given section
 */
void lengthenSection(Section *section) {
  section->length += 1;
}

/**
 * Trim the tail of the snake as it moves.  The snake is made up of one of or more
 */
void trimSection(Section *section) {
  section->length -= 1;
}

/**
 * Remove the last section of the snake (e.g. because it no longer has any length)
 */
void removeLastSection() {  
    snake.numberOfSections = snake.numberOfSections - 1;
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

Direction invert(Direction d) {
  switch(d) {
    case NORTH:
      return SOUTH;
    case SOUTH:
      return NORTH;
    case EAST:
      return WEST;
    case WEST:    
      return EAST;
   }
}

Point drawSnakeSection(Point from, Section section) {
   Point to = getEndPoint(from,section);   
   drawBlockSection(from,to);
   return to;
}

Point getEndPoint(Point start, Section section) {
  switch(section.direction) {
    case NORTH:
      start.y += section.length;
      break;
    case SOUTH:
      start.y -= section.length;
      break;
    case EAST:
      start.x -= section.length;
      break;
    case WEST:    
      start.x += section.length;
      break;
   }
   return start;
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

