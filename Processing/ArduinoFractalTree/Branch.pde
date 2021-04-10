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

  public boolean isGrowing(boolean recursive){
    if(recursive){
      return isGrowing(this);
    }else{
      if(this.growthAmt >= 1 && this.growthStep > 0){
        return true;  
      }else if(this.growthAmt <= 0 && this.growthStep < 0){
        return true; 
      }
      return false;
    }
  }
  
  private boolean isGrowing(Branch branch){
    boolean isGrowing = true;
    for (Branch childBranch : branch.childrenBranches) {
      isGrowing = isGrowing & isGrowing(childBranch);
    }
    
    if(branch.growthAmt >= 1 && branch.growthStep > 0){
      isGrowing = true;  
    }else if(branch.growthAmt <= 0 && branch.growthStep < 0){
      isGrowing= true; 
    }else{
      isGrowing = false;
    }  
    return isGrowing;
  }

  public void leavesFall() {
    leavesFall(this);
  }

  private void leavesFall(Branch branch) {
    for (Branch childBranch : branch.childrenBranches) {
      leavesFall(childBranch);
    }

    for (Leaf leaf : branch.leaves) {
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

    Branch a = createBranch(random(24f, 26f));
    Branch b = createBranch(random(-26f, -24f));

    this.childrenBranches.add(a);
    this.childrenBranches.add(b);

    // add leaves
    if (createLeaves) {
      PVector leafALoc = new PVector(a.end.x + random(-2, 2), a.end.y + random(-2, 2));
      a.leaves.add(new Leaf(leafALoc));
      PVector leafBLoc = new PVector(b.end.x + random(-2, 2), b.end.y + random(-2, 2));
      b.leaves.add(new Leaf(leafBLoc));
    }
  }

  private Branch createBranch(float angleInDegrees) {
    PVector dir = PVector.sub(this.end, this.begin);
    
    // Convert angle to radians
    float theta = radians(angleInDegrees);
    dir.rotate(theta);
    dir.mult(random(0.5, 0.85));
    PVector newEnd = PVector.add(this.end, dir);
    Branch newBranch = new Branch(this, this.end, newEnd);
    
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
    //println(" branch.growthAmt=" + branch.growthAmt + " branch.growthStep=" + branch.growthStep);
    if(branch.growthAmt >= 1f && branch.growthStep > 0){
      line(branch.begin.x, branch.begin.y, branch.end.x, branch.end.y); 
      branch.growthAmt = 1f;  
    }else if(branch.growthStep > 0){
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
