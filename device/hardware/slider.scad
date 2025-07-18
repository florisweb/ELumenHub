$fn = 100;

bottomThickness = 1.5;
wallThickness = 1.5;


screwRadiusM3 = 1.45;
screwRadiusM3Smooth = 1.6;

margin = 2;
width = 75 + 1 + margin * 2 + wallThickness * 2;
depth = 55 + 38.5 + margin * 2 + wallThickness * 2;
height = 35;
borderRadius = 7;

connectorDepth = 20;





difference() {
    union() {
        bottom();
//        translate([0, 0, 1.5])
//        top();
    }
    connectionHoles();
}


module top() {
    difference() 
    {
        difference() {
            boxWithRoundedCorners(width, depth, height, borderRadius);
            
            
            difference() {
                translate([wallThickness, wallThickness, -bottomThickness])
                boxWithRoundedCorners(width - wallThickness * 2, depth - wallThickness * 2, height, borderRadius);
                
                // Add the pillars into which the screws will fit
                
                pillarSize = borderRadius * 1;
                union() {
                    cube([pillarSize, pillarSize, height]);

                    translate([width - pillarSize, 0, 0])
                    cube([pillarSize, pillarSize, height]);
                    
                    translate([width - pillarSize, depth - pillarSize, 0])
                    cube([pillarSize, pillarSize, height]);
                    
                    translate([0, depth - pillarSize, 0])
                    cube([pillarSize, pillarSize, height]);
                }
            }
        }
        union() 
        {
            translate([borderRadius, 0, 0])
            cube([width - borderRadius * 2, bottomThickness * 2, max(height, 20) / 2]);

            translate([width / 2 + 10, 0, max(height, 20) / 2])
            cableHoles();
            
            translate([0, borderRadius, 0])
            cube([wallThickness, connectorDepth, max(height, 17) - bottomThickness]);
           
        }
    }

   

    
   
    difference() {
        translate([0, borderRadius, height - bottomThickness])
        rotate([0, 180, -90])
        powerSocketHolder(connectorDepth, max(height, 17) - bottomThickness);
        
        translate([0, borderRadius, -bottomThickness])
        cube([wallThickness, connectorDepth, max(height, 17)]);
    }
}

module bottom() {
    difference() 
    {
        boxWithRoundedCorners(width, depth, bottomThickness, borderRadius);
        
       
        translate([margin + 2, margin + connectorDepth + 10, bottomThickness])
        contents();
    }


    difference() 
    {
        translate([borderRadius, 0, bottomThickness])
        cube([width - borderRadius * 2, bottomThickness * 2, max(height, 20) / 2]);

        translate([width / 2 + 10, 0, max(height, 20) / 2 + bottomThickness])
        cableHoles();
    }

    translate([0, borderRadius + connectorDepth, bottomThickness])
    rotate([0, 0, -90])
    powerSocketHolder(connectorDepth, max(height, 17) - bottomThickness);
}

module connectionHoles() {
    offset = borderRadius * .6;
    translate([offset, offset, 0]) {
        cylinder(r=screwRadiusM3, height);
        
        translate([width - offset * 2, 0, 0])
        cylinder(r=screwRadiusM3, height);
        
        translate([width - offset * 2, depth - offset * 2, 0])
        cylinder(r=screwRadiusM3, height);
        
        translate([0, depth - offset * 2, 0])
        cylinder(r=screwRadiusM3, height);
    }
}




module cableHoles() {
    translate([-15/2, 0, -2])
    cube([15, 10, 4]);

    translate([15/2 + 5, 0, -2])
    cube([4, 10, 4]);
}


module contents() {
    relay();

    translate([0, 35 + 30, 0])
    rotate([90, 0, 0])
    ESP();
}



module relay() {
    relayWidth = 75;
    relayDepth = 55;
    relayHeight = 19;
    cube([relayWidth, relayDepth, relayHeight]);
    
    translate([3, 8.5, -10])
    cylinder(r=screwRadiusM3, 20);
    
    translate([relayWidth - 6, relayDepth - 6, -10])
    cylinder(r=screwRadiusM3, 20);
}



module ESP() {
    ESPWidth = 52.3;
    ESPDepth = 28.2;
//    ESPHeight = 13.3;
    ESPHeight = 30;
    cube([ESPWidth, ESPDepth, ESPHeight]);
}







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