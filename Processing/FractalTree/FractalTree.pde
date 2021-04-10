/* 
 * Creates a recursive fractal tree. The following interactions are supported:
 *  - Click the mouse to add in new branches
 *  - The mouse's y-position controls how much of the tree is rendered (move mouse to render more)
 *  - Press spacebar to save a tree to a file 
 *  - Press the down arrow to have leaves drop from the tree (with animation)
 *
 * By Jon Froehlich
 * http://makeabilitylab.io
 *
 * This example is (loosely) based on:
 *  - The Coding Train Fractal Tree: https://www.youtube.com/watch?v=fcdNSZ9IzJM&t=2s. The code for this is:
 *    https://github.com/CodingTrain/website/tree/master/CodingChallenges/CC_015_FractalTreeArray/Processing/CC_015_FractalTreeArray
 *
 * Other inspirations:
 *  - http://gregtatum.com/interactive/2014/recursivity-2-branching/
 *
 * FUTURE IDEAS:
 *  - Add lighting
 *  - Gradient background (though I like the simplicity currently). Maybe sun or moon in background?
 *  - [done] Change width progressively from root to outter branches
 *  - [done] Add leaves
 *  - [done] Have leaves fall down
 *  - Have easing function for leaves falling down (start slow then speed up slightly)
 *  - Add in sine wave function on x-position to have leaves sway back and forth as they fall
 *  - Create wind
 *  - Add a forest (i.e., more than one tree, would likely have to draw to off screen buffer to limit cpu overhead)
 *  - add purple/pink flowers (like blooming cherry blossoms)
 *  - Have two trees morph into each other
 *  - [done] animate the growth (easing functions) rather than discrete growth?
 *  - now that animated growth is done, animate reverse growth as well?
 *  - add in rain? see https://youtu.be/KkyIDI6rQJI
 *  - make it so leaves have a little glow?
 *  - don’t react to new tree size input if the tree is currently growing
 */

int _branchDepthCount = 0;
int _addLeavesAfterDepth = 9;
int _maxDepth = 11;
boolean _leavesFall = false;
Branch _tree;

void setup() {
  size(640, 480);
  //fullScreen();

  //create the tree! 
  createTree();
}

void createTree() {
  PVector a = new PVector(width / 2, height);
  PVector b = new PVector(width / 2, height - height/4.0);
  _tree = new Branch(null, a, b);
  _branchDepthCount = 0;
  _leavesFall = false;
}

void mousePressed() {
  if (_branchDepthCount <= _maxDepth) {
    _tree.addBranchesToEnd(_branchDepthCount >= _addLeavesAfterDepth);
    _branchDepthCount++;
  }
}

void keyPressed() {
  if (key == CODED) {
    // If the user pressed the 'down' arrow, start the leaves falling animation
    if (keyCode == DOWN) {
      _leavesFall = true;
    }
  } else if (key == ' ') { // if the user hits the spacebar, take a screenshot and save
    println("Saving current tree!");
    saveFrame("tree-######.png");
  } else { // else if the user hits any other key, clear the tree
    createTree();
  }
}

void draw() {
  background(51); // draw the background

  // use the current mouse position to draw only part of the tree
  int maxDepth = (int)map(mouseY, height, 0, 0, _maxDepth);
  _tree.draw(maxDepth);

  // if true, animate leaves falling from tree
  if (_leavesFall) {
    _tree.leavesFall();
  }

  // println(frameRate);
}
