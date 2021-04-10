/* 
 * The branch recursive object, which stores the root and all the branches
 *
 * By Jon Froehlich
 * http://makeabilitylab.io
 */

class Branch {

  public PVector begin;
  public PVector end;
  
  PVector growthPt;
  float thickness;
  Branch parentBranch = null;
  ArrayList<Branch> childrenBranches = new ArrayList<Branch>();
  ArrayList<Leaf> leaves = new ArrayList<Leaf>();
  
  float growthAmt = 0; // between [0,1] where 1 represents fully grown
  float growthStep = 0.05f; // amount to grow each draw cycle

  public Branch(Branch parent, PVector begin, PVector end) {
    this.parentBranch = parent;
    this.begin = begin;
    this.end = end;
    this.growthPt = this.begin;

    float mag = PVector.sub(end, begin).mag();
    this.thickness = max(1, mag / random(8f, 10f));

    //println("depth=" + this.getDepth() + " mag = " + mag + " thickness=" + this.thickness);
  }

  public void leavesFall() {
    leavesFall(this);
  }

  private void leavesFall(Branch branch) {
    for (Branch childBranch : branch.childrenBranches) {
      leavesFall(childBranch);
    }

    for (Leaf leaf : branch.leaves) {
      // check if leaves have hit the ground, if not, keep falling
      if (leaf.loc.y <= height - 2) { 
        leaf.loc.y += random(0.5, 4);
        leaf.loc.x += random(-2, 2);
      }
    }
  }

  public void createBranches() {
    createBranches(false);
  }

  public void createBranches(boolean createLeaves) { //TODO, in future take in an int numBranches

    if(random(0,1) >= 0.05){
      Branch a = createBranch(random(24f, 26f), createLeaves);
      Branch b = createBranch(random(-26f, -24f), createLeaves);
      this.childrenBranches.add(a);
    this.childrenBranches.add(b);
    }else{
      Branch a = createBranch(random(24f, 26f), createLeaves);
      Branch b = createBranch(random(-2f, 2f), createLeaves);
      Branch c = createBranch(random(-26f, -24f), createLeaves);
      this.childrenBranches.add(a);
      this.childrenBranches.add(b);
      this.childrenBranches.add(c);
    }

    

  }

  private Branch createBranch(float angleInDegrees, boolean createLeaves) {
    PVector dir = PVector.sub(this.end, this.begin);
    
    // Convert angle to radians
    float theta = radians(angleInDegrees);
    dir.rotate(theta);
    dir.mult(random(0.5, 0.9));
    PVector newEnd = PVector.add(this.end, dir);
    Branch newBranch = new Branch(this, this.end, newEnd);
    
    if(createLeaves && random(0,1) >= 0.7){
      float leafXLoc = newBranch.end.x + random(-6, 6);
      float leafYLoc = newBranch.end.y + random(-6, 6);
      PVector leafLoc = new PVector(leafXLoc, leafYLoc);
      newBranch.leaves.add(new Leaf(leafLoc));
    }
    
    return newBranch;
  }

  public void addBranchesToEnd(boolean createLeaves) {
    addBranchesToEnd(this, createLeaves);
  }

  private void addBranchesToEnd(Branch branch, boolean createLeaves) {
    for (Branch childBranch : branch.childrenBranches) {
      addBranchesToEnd(childBranch, createLeaves);
    }

    if (!branch.hasChildren()) {
      branch.createBranches(createLeaves);
    }
  }

  public boolean hasChildren() {
    return childrenBranches.size() > 0;
  }

  public Branch getParent() {
    return this.parentBranch;
  }

  public int getDepth() {
    int depth = 0;
    Branch parent = this.parentBranch;
    while (parent != null) {   
      parent = parent.getParent();
      depth++;
    }
    return depth;
  }

  public void draw() {
    //stroke(255);
    //line(this.begin.x, this.begin.y, this.end.x, this.end.y);  
    draw(this, -1);
  }

  public void draw(int maxDepth) {
    draw(this, maxDepth);
  }

  private void draw(Branch branch, int maxDepth) {

    // Controls how much of the tree to show. If maxDepth == -1, shows 
    // the whole tree
    if (maxDepth != -1 && branch.getDepth() > maxDepth) {
      return;
    }

    for (Branch childBranch : branch.childrenBranches) {
      draw(childBranch, maxDepth);
    }
    //stroke(255, 255, 255, 200);
    stroke(255);
    //println("depth=" + branch.getDepth() + " mag = " + branch.end.mag() + "mag2 = " + dir.mag());
    //println("strokeWidth = " + strokeWidth);
    strokeWeight(branch.thickness);
    
    if(branch.growthAmt >= 1f){
      line(branch.begin.x, branch.begin.y, branch.end.x, branch.end.y); 
    }else{
      branch.growthPt = PVector.lerp(branch.begin, branch.end, branch.growthAmt);
      branch.growthAmt += branch.growthStep;
      
      line(branch.begin.x, branch.begin.y, branch.growthPt.x, branch.growthPt.y); 
    }

    // draw leaves
    for (Leaf leaf : branch.leaves) {
      leaf.draw();
    }
  }
}
