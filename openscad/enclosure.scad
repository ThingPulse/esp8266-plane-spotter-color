$fn = 100;
pcbWidth = 42.72;
pcbLength = 77.18;

displayBorderX = 1.86;
displayBorderY = 16.2;
displayLength = 53;
spacing = 6;
wall = 2.5;
lidWall = 2;
rimHeight = 3;
holeRadius = 3.2 / 2 - 0.5;
bottomLeftX = 3;
bottomLeftY = 6.92;
bottomRightX = pcbWidth - bottomLeftX;
bottomRightY = bottomLeftY;
topLeftX = 3;
topLeftY = pcbLength - 3;
topRightX = pcbWidth - bottomLeftX;
topRightY = pcbLength - 3;
poleHeight = wall + 5.6;

boxWidth = 2 * (spacing + wall) + pcbWidth;
boxLength = 2 * (spacing + wall) + pcbLength;
boxHeight = 20;

screwHoleRadius = 1.5;
lidHoleRadius = 1.4;
screwHolePoleRadius = 3;
screwHoleHeight = 5;
lidPusherWidth = 30;
lidPusherLength = 2;
lidPusherHeight = 12.3;
lidPusherOffsetY = 20;

connectorHoleX =  pcbWidth + 10;
connectorHoleY = 14 + spacing + wall;
connectorHoleZ = 7 + wall;

module bottomCase() {
    difference() {
        cube([boxWidth, boxLength, boxHeight]);
        translate([wall, wall, wall])
            cube([boxWidth - 2 * wall, boxLength - 2 * wall, boxHeight]);
    }
};
module poles() {
    pcbOffset = wall + spacing;
   
    translate([pcbOffset + bottomLeftX, pcbOffset + bottomLeftY, 0])
        cylinder(r=holeRadius, h = poleHeight);
    translate([pcbOffset + bottomRightX, pcbOffset + bottomRightY, 0])
        cylinder(r=holeRadius, h = poleHeight);
    translate([pcbOffset + topLeftX, pcbOffset + topLeftY, 0])
        cylinder(r=holeRadius, h = poleHeight);
    translate([pcbOffset + topRightX, pcbOffset + topRightY, 0])
        cylinder(r=holeRadius, h = poleHeight);
}
module screwHoles() {
    difference() {
        union() {
            translate([screwHolePoleRadius, screwHolePoleRadius, rimHeight])
                cylinder(r = screwHolePoleRadius, h = boxHeight - rimHeight);
            translate([boxWidth - screwHolePoleRadius, boxLength - screwHolePoleRadius, rimHeight])
                cylinder(r = screwHolePoleRadius, h = boxHeight - rimHeight);
            translate([screwHolePoleRadius, boxLength - screwHolePoleRadius, rimHeight])
                cylinder(r = screwHolePoleRadius, h = boxHeight - rimHeight);
            translate([boxWidth - screwHolePoleRadius, screwHolePoleRadius, rimHeight])
                cylinder(r = screwHolePoleRadius, h = boxHeight - rimHeight);
        }
        union() {
            translate([screwHolePoleRadius, screwHolePoleRadius, boxHeight - screwHoleHeight])
                cylinder(r = screwHoleRadius, h = 5);
            translate([boxWidth - screwHolePoleRadius, boxLength - screwHolePoleRadius, boxHeight - screwHoleHeight])
                cylinder(r = screwHoleRadius, h = 5);
            translate([screwHolePoleRadius, boxLength - screwHolePoleRadius, boxHeight - screwHoleHeight])
                cylinder(r = screwHoleRadius, h = 5);
            translate([boxWidth - screwHolePoleRadius, screwHolePoleRadius, boxHeight - screwHoleHeight])
                cylinder(r = screwHoleRadius, h = 5);

        }
    }
}
module connectorHole() {
    translate([connectorHoleX, connectorHoleY, connectorHoleZ])
        cube([10, 15, 20]);
}
module case() {
    difference() {
        union() {
            bottomCase();

        }
        union() {
            //translate([wall + spacing + displayBorderX, wall + spacing + displayBorderY, -5])
            //    cube([pcbWidth - 2 * displayBorderX, displayLength, 30]);
            cutAways();
            connectorHole();
        }
    }
}
module lid() {
    translate([(boxWidth - lidPusherWidth) / 2, boxLength - lidPusherOffsetY, lidWall])
    cube([lidPusherWidth, lidPusherLength, lidPusherHeight]);
    difference() {
       cube([boxWidth, boxLength, lidWall]); 

       union() {
            translate([screwHolePoleRadius, screwHolePoleRadius, -2])
             cylinder(r = lidHoleRadius, h = 5);
            translate([boxWidth - screwHolePoleRadius, boxLength - screwHolePoleRadius, -2])
             cylinder(r = lidHoleRadius, h = 5);  
            translate([boxWidth - screwHolePoleRadius, screwHolePoleRadius, -2])
             cylinder(r = lidHoleRadius, h = 5);  
            translate([screwHolePoleRadius, boxLength - screwHolePoleRadius, -2])
             cylinder(r = lidHoleRadius, h = 5); 
       }
    }
}
module cutAways() {
    increment = 2;

    hull() {
        translate([wall + spacing + displayBorderX, wall + spacing + displayBorderY, wall - 0.5])
                    cube([pcbWidth - 2 * displayBorderX, displayLength, 0.5]);
        translate([wall + spacing + displayBorderX - increment, wall + spacing + displayBorderY - increment, -0.5])
                    cube([pcbWidth - 2 * displayBorderX + 2*increment, displayLength + 2 * increment, 0.5]);
    };
    translate([0, 0, rimHeight]) 
        rotate([0, 150, 0])  
            cube([5, boxLength, 20]); 
    translate([0,0, rimHeight])
    rotate([210, 0, 0])  
        cube([boxWidth, 5, 20]);
    translate([boxWidth, 0, rimHeight])
        rotate([0, 210, 0])
            translate([-5, 0, 0])
            cube([5, boxLength, 20]); 
    translate([0, boxLength, rimHeight])
        rotate([150, 0, 0])
            translate([0, -5, 0])
            cube([boxWidth, 5, 20]); 
}
module allCase() {
  //cutAways();
    screwHoles();
    case();

    poles();  
}
//allCase();
lid();





