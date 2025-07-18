



bottomThickness = 1.5;
wallThickness = bottomThickness;

width = 20;
height = 17;
depth = 38.5;

powerSocketHolder();

module powerSocketHolder(width = 20, height = 17) {
    socketOuterRadius = 5.8;
    depth = 38.5;

    difference() {
        difference() {
            cube([width, depth, height]);
            
            translate([width / 2, 0, height / 2])
            union() {
                rotate([-90, 0, 0])
                cylinder(r=socketOuterRadius, 11);

                translate([0, 11, 0])
                rotate([-90, 0, 0])
                cylinder(r=socketOuterRadius + .5, 13);
                
                translate([0, 24, 0])
                rotate([-90, 0, 0])
                cylinder(r=5, 13);
                
                translate([0, 37, 0])
                rotate([-90, 0, 0])
                cylinder(r=2, wallThickness);
            }
        }
        translate([0, wallThickness, height / 2])
        cube([width, depth, height]);
    }
}














// LIB


module boxWithRoundedCorners(width, depth, height, cornerRadius) {
    innerWidth = width - cornerRadius * 2;
    innerDepth = depth - cornerRadius * 2;
    
    translate([cornerRadius, 0, 0])
    cube([innerWidth, depth, height]);
    translate([0, cornerRadius, 0])
    cube([width, innerDepth, height]);
    
    translate([cornerRadius, cornerRadius, 0])
    cylinder(h=height, r=cornerRadius);
    translate([width - cornerRadius, cornerRadius, 0])
    cylinder(h=height, r=cornerRadius);
    translate([width - cornerRadius, depth - cornerRadius, 0])
    cylinder(h=height, r=cornerRadius);
    translate([cornerRadius, depth - cornerRadius, 0])
    cylinder(h=height, r=cornerRadius);
}